---------------------------------------------------------------------------
-- Company     : ARMadeus Systems
-- Author(s)   : Fabien Marteau
-- 
-- Creation Date : 05/03/2008
-- File          : wb_led.vhd
--
-- Abstract : drive one led with Wishbone bus
--
------------------------------------------------------------------------------------
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

-----------------------------------------------------------------------
	Entity wb_led is 
-----------------------------------------------------------------------
    port 
    (
		-- Syscon signals
		gls_reset : in std_logic ;
		gls_clk	  : in std_logic ;
		-- Wishbone signals
		wbs_writedata : in std_logic_vector( 15 downto 0);
		wbs_readdata  : out std_logic_vector( 15 downto 0);
		wbs_strobe    : in std_logic ;
		wbs_write	  : in std_logic ;
		wbs_ack	      : out std_logic;
		-- out signals
		LED : out std_logic 
    );
end entity;


-----------------------------------------------------------------------
Architecture wb_led_1 of wb_led is
-----------------------------------------------------------------------
	signal reg : std_logic_vector( 15 downto 0);
begin

    -- connect led
    LED <= reg(0);
    
    -- manage register
    reg_bloc : process(gls_clk,gls_reset)
    begin
    	if gls_reset = '1' then 
    		reg <= (others => '0');
    	elsif rising_edge(gls_clk) then
    		if ((wbs_strobe and wbs_write) = '1' ) then
    			reg <= wbs_writedata;
    		else
    			reg <= reg;
    		end if;
    	end if;
    
    end process reg_bloc;
    
    wbs_ack <= wbs_strobe;
    wbs_readdata <= reg;

end architecture wb_led_1;

