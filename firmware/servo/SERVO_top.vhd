----------------------------------------------------------------------------------
-- Company: Armadeus Project
-- Engineer: Yvan ROCH
-- 
-- Create Date: 2007-12-03
-- Design Name:
-- Module Name: SERVO_top - structural 
-- Project Name: SERVO
-- Target Devices: Spartan3 Armadeus
-- Tool versions: ISE 9.2.02i
-- Description: Servo Contoller
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments:
-- Ce projet est un firmware FPGA de contolleur de servo de telecommande
-- Il est inspirer du projet "Atmel AVR 2004 Design Contest Entry Project Number A3722"
-- de Circuit Cellar par Eric Gagnon :
-- http://www.circuitcellar.com/avr2004/HA3722.html
-- 
-- Ce controleur de servo permet de controle jusqu'a 24 servos.
-- Il est facilement extensible a plus.
--
-- Chaque registre de servo (0x12000000 - 0x1200002E) est un registre 16 bits 
-- avec 12 bits significatifs contenant la position du servo concerne.
-- 0000 ecrit a l'adresse 0x12000000 positionnera le servo 0 a la position maximale anti-horaire
-- 0800 ecrit a l'adresse 0x12000000 positionnera le servo 0 a la position mediane
-- 0FFF ecrit a l'adresse 0x12000000 positionnera le servo 0 a la position maximale horaire
-- Les registres 0x12000030 et 0x12000032 sont les registres de validation des servos
-- 0001 ecrit a l'adresse 0x12000030 activera le servo 0
-- 0001 ecrit a l'adresse 0x12000032 activera le servo 16
-- 
-- Decodage d'adresse:
-- 0x12000000 servo 0
-- 0x12000002 servo 1
-- 0x12000004 servo 2
-- 0x12000006 servo 3
-- 0x12000008 servo 4
-- 0x1200000A servo 5
-- 0x1200000C servo 6
-- 0x1200000E servo 7
-- 0x12000010 servo 8
-- 0x12000012 servo 9
-- 0x12000014 servo 10
-- 0x12000016 servo 11
-- 0x12000018 servo 12
-- 0x1200001A servo 13
-- 0x1200001C servo 14
-- 0x1200001E servo 15
-- 0x12000020 servo 16
-- 0x12000022 servo 17
-- 0x12000024 servo 18
-- 0x12000026 servo 19
-- 0x12000028 servo 20
-- 0x1200002A servo 21
-- 0x1200002C servo 22
-- 0x1200002E servo 23
-- 0x12000030 Registre de validation LSB
-- 0x12000032 Registre de validation MSB

-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity SERVO_top is
	port ( -- MC9328MXL pins
		-- Bus de donnees 16 bits
		DataxD : inout std_logic_vector(15 downto 0);
		
		-- Bus d'adresses A0-A12
		-- L'adresse de base du Spartan3 est 0x12000000
		AddrxDI : in std_logic_vector(12 downto 0);
		
		-- Acces en ecriture (APF9328: CTRL9, Spartan3 pin 60, MC9328MXL: EB3n pin M5) Actif Bas
		-- Ce signal du MC9328MXL est actif au niveau bas en ecriture ET en lecture
		-- Selon le Data Sheet du MC9328MXL P4:
		-- LSB Byte Strobe?Active low external enable byte signal that controls D [7:0].
		-- Voir timing Write P28
		WRxBI : in std_logic;
		
		-- Acces en lecture (APF9328: CTRL10, Spartan3 pin 63, MC9328MXL: OEn pin R5) Actif Bas
		-- Ce signal du MC9328MXL est actif au niveau bas en lecture UNIQUEMENT
		-- Selon le Data Sheet du MC9328MXL P4:
		-- Memory Output Enable?Active low output enables external data bus.
		-- Voir timing Read P26
		RDxBI : in std_logic;
		
		-- Remarque sur WRxBI et RDxBI:
		-- RDxBI est en fait OEn, actif lors de la lecture UNIQUEMENT sur le bus externe du MC9328MXL
		-- WRxBI est en fait EB3n, actif lors de l'accès a l'octet de poids faible du bus de données en lecture ET en ecriture
		-- Ce n'est pas très clair car ces deux signaux ne sont pas mutuellement exclusifs!!!
		-- En resumé:
		-- Lors d'une lecture: RDxBI=0 et WRxBI=0 (les 2 actifs a 0)
		-- Lors d'une ecriture: RDxBI=1 et WRxBI=0 (les 2 actifs a 0)
		
		-- Chip select (APF9328: CTRL1, Spartan3 pin 59, MC9328MXL: CS1n M8) Actif Bas
		CSxBI : in std_logic; 
		
		-- Entree de l'horloge a 96 MHz provenant du MC9328MXL sur la broche P55 du Spartan3
		clkInImx96MHz : in  STD_LOGIC;
		
		-- Declaration des 24 registres de 1 bit contenant la sortie PWM
		pwmOut : out std_logic_vector(23 downto 0));

end entity SERVO_top;

architecture structural of SERVO_top is
	-- Declaration du type reg12Array24 qui contient 24 signaux de 12 bits
	-- Ce type est utilise pour les 24 registres de position des servos
	type reg12Array24 is array(0 to 23) of std_logic_vector(11 downto 0);
	
	-- Declaration des 24 registres de 16 bits contenant la position des servos
	-- Initialisation en position mediane des servos
	-- Position Min: 0
	-- Position Max: FFF
	signal servosReg : reg12Array24 := (X"800",X"800",X"800",X"800",X"800",X"800",
	X"800",X"800",X"800",X"800",X"800",X"800",X"800",X"800",X"800",X"800",X"800",
	X"800",X"800",X"800",X"800",X"800",X"800",X"800");
	-- Declaration des 24 registres de 1 bit contenant la validation des servos
	-- Initailisation: tous les servo sont desactives
	signal enableReg : std_logic_vector(31 downto 0) := X"00000000";

	-- Declaration du diviseur d'horloge
	component CLK_GENERATOR
		-- Declaration des ports utilises
		port (clkIn : in  STD_LOGIC;
			counterOut16Bits	: out std_logic_vector(15 downto 0);
			writeValue : out STD_LOGIC);
	end component CLK_GENERATOR;

	-- Declaration du generateur PWM
	component PWM_MODULE
		-- Declaration des ports utilises
		Port (count : in std_logic_vector(15 downto 0);
			servosReg : in std_logic_vector( 11 downto 0);
			enableReg: in std_logic;
			pwmOut : out std_logic;
			writeValue : in std_logic);
	end component PWM_MODULE;
	
	-- Declaration des signaux
	-- Reset asynchrone actif bas
	signal resetn : std_logic;
	-- Signal de synchro de lecture
	signal readAccessPulse : std_logic;
	-- Signal buffer pour compteur principal
	signal count : std_logic_vector(15 downto 0);
	-- Signal en provenance de compteur principal signifiant que les valeurs
	-- servosReg(X) et enableReg(X) peuvent mis a jour sans perturber la generation
	-- de la partie utile de l'impulsion
	signal writeValue : std_logic;
	
	-- Definition de l'architecture
	begin
		-- Instantiation du generateur d'horloge
		clockGen : CLK_GENERATOR
		-- Mapping des ports du generateur d'horloge
		port map (clkIn => clkInImx96MHz,
			counterOut16Bits => count,
 			writeValue => writeValue);
-- 			testOut => testOut);
		-- Instantiation des 24 modules PWM
		PWM0 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(0),
			enableReg => enableReg(0),
			pwmOut => pwmOut(0),
			writeValue => writeValue);
		PWM1 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(1),
			enableReg => enableReg(1),
			pwmOut => pwmOut(1),
			writeValue => writeValue);
		PWM2 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(2),
			enableReg => enableReg(2),
			pwmOut => pwmOut(2),
			writeValue => writeValue);
		PWM3 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(3),
			enableReg => enableReg(3),
			pwmOut => pwmOut(3),
			writeValue => writeValue);
		PWM4 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(4),
			enableReg => enableReg(4),
			pwmOut => pwmOut(4),
			writeValue => writeValue);
		PWM5 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(5),
			enableReg => enableReg(5),
			pwmOut => pwmOut(5),
			writeValue => writeValue);
		PWM6 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(6),
			enableReg => enableReg(6),
			pwmOut => pwmOut(6),
			writeValue => writeValue);
		PWM7 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(7),
			enableReg => enableReg(7),
			pwmOut => pwmOut(7),
			writeValue => writeValue);
		PWM8 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(8),
			enableReg => enableReg(8),
			pwmOut => pwmOut(8),
			writeValue => writeValue);
		PWM9 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(9),
			enableReg => enableReg(9),
			pwmOut => pwmOut(9),
			writeValue => writeValue);
		PWM10 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(10),
			enableReg => enableReg(10),
			pwmOut => pwmOut(10),
			writeValue => writeValue);
		PWM11 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(11),
			enableReg => enableReg(11),
			pwmOut => pwmOut(11),
			writeValue => writeValue);
		PWM12 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(12),
			enableReg => enableReg(12),
			pwmOut => pwmOut(12),
			writeValue => writeValue);
		PWM13 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(13),
			enableReg => enableReg(13),
			pwmOut => pwmOut(13),
			writeValue => writeValue);
		PWM14 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(14),
			enableReg => enableReg(14),
			pwmOut => pwmOut(14),
			writeValue => writeValue);
		PWM15 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(15),
			enableReg => enableReg(15),
			pwmOut => pwmOut(15),
			writeValue => writeValue);
		PWM16 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(16),
			enableReg => enableReg(16),
			pwmOut => pwmOut(16),
			writeValue => writeValue);
		PWM17 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(17),
			enableReg => enableReg(17),
			pwmOut => pwmOut(17),
			writeValue => writeValue);
		PWM18 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(18),
			enableReg => enableReg(18),
			pwmOut => pwmOut(18),
			writeValue => writeValue);
		PWM19 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(19),
			enableReg => enableReg(19),
			pwmOut => pwmOut(19),
			writeValue => writeValue);
		PWM20 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(20),
			enableReg => enableReg(20),
			pwmOut => pwmOut(20),
			writeValue => writeValue);
		PWM21 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(21),
			enableReg => enableReg(21),
			pwmOut => pwmOut(21),
			writeValue => writeValue);
		PWM22 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(22),
			enableReg => enableReg(22),
			pwmOut => pwmOut(22),
			writeValue => writeValue);
		PWM23 : PWM_MODULE
		-- Mapping des ports du module PWM
		port map (count => count,
			servosReg => servosReg(23),
			enableReg => enableReg(23),
			pwmOut => pwmOut(23),
			writeValue => writeValue);

	-- Initialisation
	resetn <= '1';

	-- Processus d'ecriture dans le registre depuis le bus µP
	registerWrite: process (clkInImx96MHz, resetn)
	begin  -- process registerWrite
		-- Reset asynchrone actif au niveau bas
		if resetn = '0' then
			-- Action eventuelle du Reset
		-- Sur le front montant de clk_in_Imx_96MHz
		elsif clkInImx96MHz'event and clkInImx96MHz = '1' then
			if CSxBI = '0' and WRxBI = '0' then
				-- Decodage d'adresse, adresse de base: 0x12000000
				case AddrxDI is
					-- 0x12000000 servo 0
					when "0000000000000" =>
						-- Sauvegarde dans le registre du servo 0
						servosReg(0) <= DataxD(11 downto 0);
					-- 0x12000002 servo 1
					when "0000000000010" =>
						-- Sauvegarde dans le registre du servo 1
						servosReg(1) <= DataxD(11 downto 0);
					-- 0x12000004 servo 2
					when "0000000000100" =>
						-- Sauvegarde dans le registre du servo 2
						servosReg(2) <= DataxD(11 downto 0);
					-- 0x12000006 servo 3
					when "0000000000110" =>
						-- Sauvegarde dans le registre du servo 3
						servosReg(3) <= DataxD(11 downto 0);
					-- 0x12000008 servo 4
					when "0000000001000" =>
						-- Sauvegarde dans le registre du servo 4
						servosReg(4) <= DataxD(11 downto 0);
					-- 0x1200000A servo 5
					when "0000000001010" =>
						-- Sauvegarde dans le registre du servo 5
						servosReg(5) <= DataxD(11 downto 0);
					-- 0x1200000C servo 6
					when "0000000001100" =>
						-- Sauvegarde dans le registre du servo 6
						servosReg(6) <= DataxD(11 downto 0);
					-- 0x1200000E servo 7
					when "0000000001110" =>
						-- Sauvegarde dans le registre du servo 7
						servosReg(7) <= DataxD(11 downto 0);
					-- 0x12000010 servo 8
					when "0000000010000" =>
						-- Sauvegarde dans le registre du servo 8
						servosReg(8) <= DataxD(11 downto 0);
					-- 0x12000012 servo 9
					when "0000000010010" =>
						-- Sauvegarde dans le registre du servo 9
						servosReg(9) <= DataxD(11 downto 0);
					-- 0x12000014 servo 10
					when "0000000010100" =>
						-- Sauvegarde dans le registre du servo 10
						servosReg(10) <= DataxD(11 downto 0);
					-- 0x12000016 servo 11
					when "0000000010110" =>
						-- Sauvegarde dans le registre du servo 11
						servosReg(11) <= DataxD(11 downto 0);
					-- 0x12000018 servo 12
					when "0000000011000" =>
						-- Sauvegarde dans le registre du servo 12
						servosReg(12) <= DataxD(11 downto 0);
					-- 0x1200001A servo 13
					when "0000000011010" =>
						-- Sauvegarde dans le registre du servo 13
						servosReg(13) <= DataxD(11 downto 0);
					-- 0x1200001C servo 14
					when "0000000011100" =>
						-- Sauvegarde dans le registre du servo 14
						servosReg(14) <= DataxD(11 downto 0);
					-- 0x1200001E servo 15
					when "0000000011110" =>
						-- Sauvegarde dans le registre du servo 15
						servosReg(15) <= DataxD(11 downto 0);
					-- 0x12000020 servo 16
					when "0000000100000" =>
						-- Sauvegarde dans le registre du servo 16
						servosReg(16) <= DataxD(11 downto 0);
					-- 0x12000022 servo 17
					when "0000000100010" =>
						-- Sauvegarde dans le registre du servo 17
						servosReg(17) <= DataxD(11 downto 0);
					-- 0x12000024 servo 18
					when "0000000100100" =>
						-- Sauvegarde dans le registre du servo 18
						servosReg(18) <= DataxD(11 downto 0);
					-- 0x12000026 servo 19
					when "0000000100110" =>
						-- Sauvegarde dans le registre du servo 19
						servosReg(19) <= DataxD(11 downto 0);
					-- 0x12000028 servo 20
					when "0000000101000" =>
						-- Sauvegarde dans le registre du servo 20
						servosReg(20) <= DataxD(11 downto 0);
					-- 0x1200002A servo 21
					when "0000000101010" =>
						-- Sauvegarde dans le registre du servo 21
						servosReg(21) <= DataxD(11 downto 0);
					-- 0x1200002C servo 22
					when "0000000101100" =>
						-- Sauvegarde dans le registre du servo 22
						servosReg(22) <= DataxD(11 downto 0);
					-- 0x1200002E servo 23
					when "0000000101110" =>
						-- Sauvegarde dans le registre du servo 23
						servosReg(23) <= DataxD(11 downto 0);
					-- 0x12000030 Registre de validation LSB
					when "0000000110000" =>
						-- Sauvegarde dans le registre de validation LSB
						enableReg(15 downto 0) <= DataxD(15 downto 0);
					-- 0x12000032 Registre de validation MSB
					when "0000000110010" =>
						-- Sauvegarde dans le registre de validation LSB
						enableReg(31 downto 16) <= DataxD(15 downto 0);
					when others => null;
				end case;
			end if;
		end if;
	end process registerWrite;

	-- Processus d'acces generique en lecture depuis le bus µP
	registerReadSyn: process (clkInImx96MHz, resetn)
		variable ReadAccessEdgeDetect : std_logic := '0';
	begin  -- process registerReadSyn
		-- Reset asynchrone actif au niveau bas
		if resetn = '0' then
			ReadAccessPulse   <= '1';
			ReadAccessEdgeDetect := '0';
		-- Sur le front montant de clk_in_Imx_96MHz
		elsif clkInImx96MHz'event and clkInImx96MHz = '1' then
			-- Si initialement ReadAccessEdgeDetect=0 et acces en lecture (RDxBI=0 et CSxBI=0)
			-- Alors ReadAccessPulse=0
			-- Au cycle d'horloge suivant si tourjours acces en lecture (RDxBI=0 et CSxBI=0)
			-- Alors ReadAccessPulse=1
			-- Et vice versa
			-- On obtient donc une transition 0->1 ou 1->0 sur ReadAccessPulse
			-- Au maximum en 2 cycle de clk_in_Imx_96MHz
			-- Lorsque l'acces en lecture est fini ReadAccessEdgeDetect revient a 0
			if (ReadAccessEdgeDetect = '0' and ((not CSxBI) and (not RDxBI)) = '1') then
				ReadAccessPulse <= '0';
			else
				ReadAccessPulse <= '1';
			end if;
			-- ReadAccessEdgeDetect = 1 si lecture (RDxBI=0 et CSxBI=0)
			ReadAccessEdgeDetect := (not CSxBI) and (not RDxBI);
		end if;
	end process registerReadSyn;

	-- Processus de lecture du registre depuis le bus µP
	registerRead : process (CSxBI, RDxBI, AddrxDI, ReadAccessPulse)
	begin -- process registerRead
		-- acces en lecture (RDxBI=0 et CSxBI=0)
		if CSxBI = '0' and RDxBI = '0' then
			case AddrxDI is
				-- 0x12000000 servo 0
				when "0000000000000" =>
					-- Envoi sur le bus de donnees µP du registre du servo 0
					DataxD <= "0000" & servosReg(0)(11 downto 0);
				-- 0x12000002 servo 1
				when "0000000000010" =>
					-- Envoi sur le bus de donnees µP du registre du servo 1
					DataxD <= "0000" & servosReg(1)(11 downto 0);
				-- 0x12000004 servo 2
				when "0000000000100" =>
					-- Envoi sur le bus de donnees µP du registre du servo 2
					DataxD <= "0000" & servosReg(2)(11 downto 0);
				-- 0x12000006 servo 3
				when "0000000000110" =>
					-- Envoi sur le bus de donnees µP du registre du servo 3
					DataxD <= "0000" & servosReg(3)(11 downto 0);
				-- 0x12000008 servo 4
				when "0000000001000" =>
					-- Envoi sur le bus de donnees µP du registre du servo 4
					DataxD <= "0000" & servosReg(4)(11 downto 0);
				-- 0x1200000A servo 5
				when "0000000001010" =>
					-- Envoi sur le bus de donnees µP du registre du servo 5
					DataxD <= "0000" & servosReg(5)(11 downto 0);
				-- 0x1200000C servo 6
				when "0000000001100" =>
					-- Envoi sur le bus de donnees µP du registre du servo 6
					DataxD <= "0000" & servosReg(6)(11 downto 0);
				-- 0x1200000E servo 7
				when "0000000001110" =>
					-- Envoi sur le bus de donnees µP du registre du servo 7
					DataxD <= "0000" & servosReg(7)(11 downto 0);
				-- 0x12000010 servo 8
				when "0000000010000" =>
					-- Envoi sur le bus de donnees µP du registre du servo 8
					DataxD <= "0000" & servosReg(8)(11 downto 0);
				-- 0x12000012 servo 9
				when "0000000010010" =>
					-- Envoi sur le bus de donnees µP du registre du servo 9
					DataxD <= "0000" & servosReg(9)(11 downto 0);
				-- 0x12000014 servo 10
				when "0000000010100" =>
					-- Envoi sur le bus de donnees µP du registre du servo 10
					DataxD <= "0000" & servosReg(10)(11 downto 0);
				-- 0x12000016 servo 11
				when "0000000010110" =>
					-- Envoi sur le bus de donnees µP du registre du servo 11
					DataxD <= "0000" & servosReg(11)(11 downto 0);
				-- 0x12000018 servo 12
				when "0000000011000" =>
					-- Envoi sur le bus de donnees µP du registre du servo 12
					DataxD <= "0000" & servosReg(12)(11 downto 0);
				-- 0x1200001A servo 13
				when "0000000011010" =>
					-- Envoi sur le bus de donnees µP du registre du servo 13
					DataxD <= "0000" & servosReg(13)(11 downto 0);
				-- 0x1200001C servo 14
				when "0000000011100" =>
					-- Envoi sur le bus de donnees µP du registre du servo 14
					DataxD <= "0000" & servosReg(14)(11 downto 0);
				-- 0x1200001E servo 15
				when "0000000011110" =>
					-- Envoi sur le bus de donnees µP du registre du servo 15
					DataxD <= "0000" & servosReg(15)(11 downto 0);
				-- 0x12000020 servo 16
				when "0000000100000" =>
					-- Envoi sur le bus de donnees µP du registre du servo 16
					DataxD <= "0000" & servosReg(16)(11 downto 0);
				-- 0x12000022 servo 17
				when "0000000100010" =>
					-- Envoi sur le bus de donnees µP du registre du servo 17
					DataxD <= "0000" & servosReg(17)(11 downto 0);
				-- 0x12000024 servo 18
				when "0000000100100" =>
					-- Envoi sur le bus de donnees µP du registre du servo 18
					DataxD <= "0000" & servosReg(18)(11 downto 0);
				-- 0x12000026 servo 19
				when "0000000100110" =>
					-- Envoi sur le bus de donnees µP du registre du servo 19
					DataxD <= "0000" & servosReg(19)(11 downto 0);
				-- 0x12000028 servo 20
				when "0000000101000" =>
					-- Envoi sur le bus de donnees µP du registre du servo 20
					DataxD <= "0000" & servosReg(20)(11 downto 0);
				-- 0x1200002A servo 21
				when "0000000101010" =>
					-- Envoi sur le bus de donnees µP du registre du servo 21
					DataxD <= "0000" & servosReg(21)(11 downto 0);
				-- 0x1200002C servo 22
				when "0000000101100" =>
					-- Envoi sur le bus de donnees µP du registre du servo 22
					DataxD <= "0000" & servosReg(22)(11 downto 0);
				-- 0x1200002E servo 23
				when "0000000101110" =>
					-- Envoi sur le bus de donnees µP du registre du servo 23
					DataxD <= "0000" & servosReg(23)(11 downto 0);
					-- 0x12000030 Registre de validation LSB
				when "0000000110000" =>
					-- Envoi sur le bus de donnees µP du registre de validation LSB
					DataxD <= enableReg(15 downto 0);
				-- 0x12000032 Registre de validation MSB
				when "0000000110010" =>
					-- Envoi sur le bus de donnees µP du registre de validation LSB
					DataxD <= enableReg(31 downto 16);
				when others		=>
					DataxD <= (others => '0');
			end case;
		else
			-- Haute impedance si acces autre qu'en lecture
			DataxD <= (others => 'Z');
		end if;
	end process registerRead;
end architecture structural;
