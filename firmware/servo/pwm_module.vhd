----------------------------------------------------------------------------------
-- Company: Armadeus Project
-- Engineer: Yvan ROCH
-- 
-- Create Date: 2007-12-05
-- Design Name:
-- Module Name: PWM_MODULE - Behavioral 
-- Project Name: SERVO
-- Target Devices: Spartan3 Armadeus
-- Tool versions: ISE 9.2.02i
-- Description: PWM signal generator
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments:
--
------------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity PWM_MODULE is
	Port ( 
		-- Compteur externe de 16 bits cadencer a 4MHz: un cycle complet du compteur
		-- dure 16.384 ms, c'est la durée entre 2 implusions
		count : in std_logic_vector(15 downto 0);
		-- signal servosReg(X) copie dans pwmValue
		servosReg : in std_logic_vector( 11 downto 0);
		-- signal enableReg(X) copie dans enable
		enableReg: in std_logic;
		-- sortie pwm
		pwmOut : out std_logic;
		-- le signal writeValue valide l'ecriture de servosReg dans pwmValue et enableReg dans
		-- enable a un instant ou la sortie PWM est a zero.
		-- Ainsi la generation de l'impulsion n'est pas perturbee par le changement
		-- de valeur de servosReg et enableReg
		writeValue : in std_logic);
end PWM_MODULE;

architecture Behavioral of PWM_MODULE is
	-- Valeur interne pwmValue : registre de 12 bits.
	signal pwmValue : std_logic_vector(11 downto 0) := "000000000000";
	-- Validation de la sortie PWM
	signal enable: std_logic := '0';
begin

	-- Ecriture dans les registres du module
	process(writeValue)
	begin
		if (writeValue'event and writeValue='1') then
		-- Sur le front montant de writeValue memorisation
		-- de servosReg dans pwmValue et enableReg dans enable
			pwmValue <= servosReg;
			enable <= enableReg;
		end if;
	end process;

	-- Generation du signal PWM
	pwmOut <= '0' when (enable = '0') else -- sortie a 0 si servo devalide
		  	  -- Generation de l'impulsion de 0.5 ms commune a toute les valeurs
			  -- 1111100000110000b=63536
			  -- 65536-63536=2000 2000 cycle a 4MHz=0.5ms
			  -- Cela corresponde au debut de generation de l'impulsion
			'1' when (count > "1111100000110000") else -- Count of 63536
			  -- Multiplication de pwmValue par 2 (decalage a gauche de 1)
			  -- pour pwmValue=0 position extreme avec une impulsion de 0+0.5=0.5ms
			  -- pour pwmValue=4000 position extreme avec une impulsion de 2+0.5=2.5ms
			  -- pwmValue= temps_impulsion(ms).2.10^6
			  -- Quand le compteur est depasse: fin de l'implusion
			'0' when (count > ("000" & pwmValue(11 downto 0) & "0")) else
			  -- le compteur vaut entre 0 et pwmValue: generation de l'impulsion
			  '1';
end Behavioral;
