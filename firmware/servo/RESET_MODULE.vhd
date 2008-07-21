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
-- Create Date: 2008-05-21
-- Design Name:
-- Module Name: RESET - behavioral 
-- Project Name: SERVO
-- Target Devices: Spartan3 Armadeus
-- Tool versions: ISE 9.2.02i
-- Description: Reset Generator
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Revision 0.02 - 2008-05-11 (see SERVO_top.vhd)
-- Revision 0.03 - 2008-07-09 (see SERVO_top.vhd)
-- Additional Comments:
-- This code comes from Fabrice Mousset (syscon.vhd Wisbone_example)

-- To avoid the Warning Messages from XST:
-- WARNING:Xst:2677 - Node <resetGenerator/resetInside_4> of sequential type is unconnected in block <SERVO_top>.
-- .
-- .
-- .
-- WARNING:Xst:2677 - Node <resetGenerator/resetInside_20> of sequential type is unconnected in block <SERVO_top>.
-- FlipFlop resetGenerator/resetInside has been replicated 20 time(s)
-- With ISE
-- In ISE -> Synthesize - XST -> Properties -> Xilinx Specific Options (advanced)  -> Unckeck Register Duplication
-- In the VHDL source
-- attribute register_duplication: string;
-- attribute register_duplication of resetInside : signal is "yes";
-- See XST User Guide 8.1i: Chapter 5 p395 Register Duplication
-- The fanout of the resetCold is enought whitout duplication (Max Fanout is 500 for a Saprtan 3)...

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

---------------------------------------------------------------------------
Entity RESET_MODULE is 
---------------------------------------------------------------------------
port 
(
	-- Clock Input
	clkIn : in std_logic ;
	-- External Reset Input
	resetIn : in std_logic ;
	-- Well formed reset signal
	resetOut : out std_logic
);
end entity;

---------------------------------------------------------------------------
Architecture behavioral of RESET_MODULE is
---------------------------------------------------------------------------
signal delayCold: std_logic := '0';
signal resetCold: std_logic := '0';
signal resetReqPulse: std_logic := '0';


-- No Register Duplication for signal reset
attribute register_duplication: string;
attribute register_duplication of resetCold : signal is "no";

begin
----------------------------------------------------------------------------
--  RESET signal generator.
----------------------------------------------------------------------------
	process(clkIn,resetIn)
		variable resetInEdgeDetect : std_logic := '0';
	begin
	-- This process generate a 1->0 0->1 transition on
	-- the resetReqPulse when these a rinsing edge on resetIn
	if(rising_edge(clkIn)) then
		if (resetInEdgeDetect = '0' and resetIn ='1') then
			resetReqPulse <= '0';
		else
			resetReqPulse <= '1';
		end if;
		resetInEdgeDetect :=resetIn;
 	end if;
	end process;
	
	process(clkIn)
	begin
	-- Generating one pulse on resetCold on startup for 2 rising edge of clkIn
	-- Generating one pulse on resetCold on pulse on resetReqPulse
	if(rising_edge(clkIn)) then
		delayCold <= (not(resetReqPulse) and delayCold and not(resetCold))
			or (not(delayCold) and resetCold);
		resetCold <= (not(resetReqPulse) and not(delayCold) and not(resetCold) );
 	end if;
	end process;
	resetOut <= resetCold;
end architecture behavioral;
