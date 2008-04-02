----------------------------------------------------------------------------------
-- Company: ARMadeus Systems
-- Engineer: Fabien Marteau <fabien.marteau@armadeus.com>
-- 
-- Create Date: 2008
-- Design Name:
-- Module Name:
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2, or (at your option)
-- any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;



entity bus_led_top
is
	port (
			 imx_data         : inout std_logic_vector(15 downto 0);
			 imx_data0_test   : out std_logic;
			 imx_addr1_test   : out std_logic ;
			 imx_addr         : in std_logic_vector(12 downto 1);
			 imx_cs_n         : in std_logic ;
			 imx_eb3_n        : in std_logic ;
			 imx_oe_n         : in std_logic ;
			 clk              : in std_logic ;
			 LED              : out std_logic);

end entity bus_led_top ; 

architecture hierarchical
	of bus_led_top  is
		component bus_led
			port(
					imx_data    : inout std_logic_vector(15 downto 0);
					imx_addr    : in	std_logic_vector(12 downto 1);
					imx_cs_n    : in	std_logic ;
					imx_eb3_n   : in	std_logic ;
					imx_oe_n    : in 	std_logic ;
					clk         : in 	std_logic ;
					LED         : out	std_logic ;
					Rst_n       : in	std_logic);
		end component;

		-- Asynchrone reset active low
		signal Rst_n : std_logic ;
	begin

		Rst_n <= '1';
		imx_data0_test <= imx_data(0);
		imx_addr1_test <= imx_addr(1);

		connect_bus_led : bus_led
		port map(
					imx_data    => imx_data,
					imx_addr    => imx_addr,
					imx_cs_n    => imx_cs_n,
					imx_eb3_n   => imx_eb3_n,
					imx_oe_n    => imx_oe_n,
					clk         => clk,
					LED         => LED,
					Rst_n       => Rst_n);

end architecture ;

