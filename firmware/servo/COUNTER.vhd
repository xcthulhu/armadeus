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
-- Engineer: Yvan Roch
-- 
-- Create Date: 2007-12-03
-- Design Name: 
-- Module Name: COUNTER - behavioral 
-- Project Name: 
-- Target Devices: Spartan3 Armadeus
-- Tool versions: ISE 9.2.02i
-- Description: Servo Contoller
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Revision 0.02 - 2008-05-11 (see SERVO_top.vhd)
-- Revision 0.03 - 2008-07-09 (see SERVO_top.vhd)
-- Additional Comments:
-- This module is a 16bits counter. 
-- It maintains a 16 bits counter at 4 MHz used by all the PWM modules.
----------------------------------------------------------------------------------
library IEEE;
-- IEEE.STD_LOGIC_UNSIGNED.ALL and IEEE.STD_LOGIC_ARITH.ALL are not used because
-- they may have some incompability with some synthesizer (SYNOPSYS library)
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity COUNTER_MODULE is
	-- Clock division ratio
	-- Default value of 24 for counter output at 4MHz
	generic(ratio	: integer := 24);

	port (clkIn	: in STD_LOGIC; -- 96 MHz clock input from MC9328MXL
		counterOut16Bits	: out std_logic_vector(15 downto 0) -- 16 bits counter used by all the PWM modules
	);
end entity COUNTER_MODULE;

architecture behavioral of COUNTER_MODULE is
	signal counter : std_logic_vector(15 downto 0) := X"0000"; -- Internal 16 bits counter register

begin
	-- 4MHz counter
	process (clkIn)
		variable count : integer  range 0 to ratio;

	begin
		-- On the rising edge of clkIn
		if (rising_edge(clkIn)) then
			-- Count the 96MHz signal
			count := count + 1;
			-- 16 bits 96/ratio MHz counter management
			-- For ratio=24, counter is incremented at 4MHz rate
			if (count = ratio) then
				count := 0;
				counter <= std_logic_vector(unsigned(counter) + 1);
				counterOut16Bits <= counter;				
			end if; 
		end if;
	end process;
end architecture behavioral;
