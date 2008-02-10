----------------------------------------------------------------------------------
-- Company: Armadeus Project
-- Engineer: Yvan Roch
-- 
-- Create Date: 2007-12-03
-- Design Name: 
-- Module Name: CLK_GENERATOR - behavioral 
-- Project Name: 
-- Target Devices: Spartan3 Armadeus
-- Tool versions: ISE 9.2.02i
-- Description: Servo Contoller
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments:
-- Ce module est un diviseur d'horloge, il fourni un signal d'horloge a 4MHz
-- et maintient le compteur 16 bits utilise par tous les modules PWM
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity CLK_GENERATOR is
	port (clkIn    : in STD_LOGIC; -- Entree de l'horloge a 96 MHz provenant du MC9328MXL
		counterOut16Bits	: out std_logic_vector(15 downto 0); -- Compteur 16 bits utilise par tous les modules PWM
		writeValue : out STD_LOGIC); -- Validation de la mise a jour des registres internes des modules PWM
end entity CLK_GENERATOR;

architecture behavioral of CLK_GENERATOR is
	signal clkInternalOut : STD_LOGIC := '0'; -- Registre interne du generateur 4MHz
	signal counter : std_logic_vector(15 downto 0) := X"0000"; -- Registre interne du compteur

begin
	-- generation du signal 4MHz
	process (clkIn)
		variable countMax : integer; -- Valeur numerique du countMax
		variable count : integer;
	
	begin
		-- Sur front montant de clkIn
		if (clkIn'event) and (clkIn = '1') then
			-- Compte le nombre de cycle d'horloge
			count := count + 1;
			-- Pour une frequence de sortie de 4MHz
			countMax := 24;
			-- Si countMax/2 est atteint
			-- La frequence de sortie vaut: 1/((1/96000000)*countMax)
			if (count = countMax/2) then
				clkInternalOut <= not clkInternalOut;
				count := 0;
			end if; 
		end if;
    end process;
	 
	 -- gestion du compteur 16 bits
	 process (clkInternalOut)

	 begin
		-- Sur front montant de clkInternalOut
		if (clkInternalOut'event and clkInternalOut = '1') then
			counter <= counter + 1;
			-- Sortie de la valeur du compteur
			counterOut16Bits <= counter;
			-- Generation du signal writeValue
			if (counter > "1111100000110000") then
				-- Debut de la generation de l'impulsion PWM (partie constante de 0.5ms)
				-- a la valeur 63536 du counter
				-- => Mise a jour des registres interdite
				writeValue <= '0';
			elsif (counter < "0001111111111110") then
				-- Generation impulsions PWM en cours
				-- valeur maximale du compteur 12 bits 
				-- => Mise a jour des registres interdite
				writeValue <= '0';
			else
				-- Dans les autre cas, les sorties PWM sont a 0 => Mise jour des registres possible
				writeValue <= '1';
			end if;
		end if;
	 end process;
end architecture behavioral;
