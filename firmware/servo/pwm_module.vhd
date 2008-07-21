--------------------------------------------------------------------------------
-- Copyright (C) 2008  Armadeus Project

-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.


-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.

-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------
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
-- Revision 0.02 - 2008-05-11 (see SERVO_top.vhd)
-- Revision 0.03 - 2008-07-09 (see SERVO_top.vhd)
-- Additional Comments:
-- PWM module: This module generates the PWM output and save the servosReg and
-- enableReg signals in the internal module register at the right time.
--------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity PWM_MODULE is
	Port ( 
		-- External 96 MHz clock
		clkIn : in std_logic;
		-- External 4MHz, 16 bits, external counter
		-- One counter cycle has a duration of 16.384 ms
		-- This is the delay between 2 PWM pulses
		count : in std_logic_vector(15 downto 0);

		-- signal servosReg(X) to be copied in pwmValue
		servosReg : in std_logic_vector( 11 downto 0);

		-- signal enableReg(X) to be copied in enable
		enableReg: in std_logic;

		-- PWM output
		pwmOut : out std_logic);
end PWM_MODULE;

architecture Behavioral of PWM_MODULE is
	-- Internal value pwmValue : 12 bits register
	signal pwmValue : std_logic_vector(11 downto 0) := "000000000000";
	-- PWM output validation
	signal enable: std_logic := '0';
begin

	-- Writing operation in the module registers
	-- 
	-- Revision 0.02: removing the writeValue signal.
	-- The internal registers must updated only when the PWM ouput is 0
	-- for avoiding the servo jittering.
	-- The PWM ouput is 0 when count < 1111100000110000b (start of the common 
	-- 0.5 ms PWM pulse for all servos)and
	-- count > 0001111111111110b (max 12bits pwmValue left shifted by 1 bit)
	-- Arbitrary the internal registers will be updated when count is 0x8000
	process(clkIn)
	begin
		-- On the rising edge of count
		if (rising_edge(clkIn)) then
			-- count is 0x8000 update of the internal registers
			if (count = "1000000000000000") then
				pwmValue <= servosReg;
				enable <= enableReg;
			end if;
		end if;
	end process;

	-- PWM signal generation 
	-- 
	pwmOut <= 
			-- PWM output to 0 if servo disable
			'0' when (enable = '0') else

			-- Common to all values 0.5ms pulse generation
			-- 1111100000110000b=63536
			-- 65536-63536=2000: 2000 cycles at 4MHz=0.5ms
			-- This is the start of the pulse generation
			'1' when (count > "1111100000110000") else -- Count of 63536

			-- Multiplication by 2 of pwmValue (left shift by 1 bit)
			-- for pwmValue=0 extreme anti-clockwise position with pulse duration of 0+0.5=0.5ms
			-- pour pwmValue=4000 extreme clockwise position with pulse duration of 2+0.5=2.5ms
			-- pwmValue= (total_impulsion_time(ms)-0.5ms).2.10^6
			-- When count is greater : end of pulse
			'0' when (count > ("000" & pwmValue(11 downto 0) & "0")) else
			
			-- the count value is between 0 and pwmValue: PWM pulse generation
			'1';
end Behavioral;
