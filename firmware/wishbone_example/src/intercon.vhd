---------------------------------------------------------------------------
-- Company     : ARMadeus Systems
-- Author(s)   : Fabien Marteau
-- 
-- Creation Date : 05/03/2008
-- File          : intercon.vhd
--
-- Abstract : connect master wishbone device with
-- slave(s) wishbone device
--
-----------------------------------------------------------------------------------
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

---------------------------------------------------------------------------
Entity intercon is 
---------------------------------------------------------------------------
	port 
	(
		-- imx wishbone master connexion
		gls_reset_m1        : out std_logic ;
		gls_clk_m1          : out std_logic ;

		wbm_address_m1      : in std_logic_vector( 12 downto 0);
		wbm_readdata_m1     : out std_logic_vector( 15 downto 0);
		wbm_writedata_m1    : in std_logic_vector( 15 downto 0);
		wbm_strobe_m1       : in std_logic ;
		wbm_write_m1        : in std_logic ;
		wbm_sel_m1          : in std_logic_vector( 1 downto 0);
		wbm_ack_m1					: out std_logic ;
		wbm_cycle_m1        : in std_logic ;

		-- led wishbone slave connexion
		gls_reset_s1        : out std_logic ;
		gls_clk_s1          : out std_logic ;
		wbs_writedata_s1    : out std_logic_vector( 15 downto 0);
		wbs_readdata_s1     : in std_logic_vector( 15 downto 0);
		wbs_strobe_s1       : out std_logic ;
		wbs_write_s1        : out std_logic ;
		wbs_ack_s1          : in std_logic ;

		-- button slave connexion
		gls_reset_s2 			  : out std_logic ;
	  gls_clk_s2					: out std_logic ;
		wbs_readdata_s2     : in std_logic_vector( 15 downto 0);
		wbs_strobe_s2       : out std_logic ;
		wbs_write_s2        : out std_logic ;
		wbs_ack_s2          : in std_logic ;

	  wbs_irq_s2 					: in std_logic ;

		-- irq slave connexion
		gls_reset_s3        : out std_logic ;
		gls_clk_s3          : out std_logic ;
		wbs_address_s3			: out std_logic ;
		wbs_writedata_s3    : out std_logic_vector( 15 downto 0);
		wbs_readdata_s3     : in std_logic_vector( 15 downto 0);
		wbs_strobe_s3       : out std_logic ;
		wbs_write_s3        : out std_logic ;
		wbs_ack_s3          : in std_logic ;
		wbs_irq_s3					: out std_logic ;

		-- syscon connection
		gls_reset_sys       : in std_logic ;
		gls_clk_sys         : in std_logic
	);
end entity;

---------------------------------------------------------------------------
Architecture intercon_1 of intercon is
---------------------------------------------------------------------------
	signal led_cs : std_logic ;
	signal button_cs : std_logic ;
	signal irq_cs : std_logic ;
	signal cs     :std_logic_vector( 2 downto 0);
begin

	-- clock and reset management
	gls_reset_s1 <= gls_reset_sys;
	gls_clk_s1 <= gls_clk_sys;

	gls_reset_s2 <= gls_reset_sys;
	gls_clk_s2 <= gls_clk_sys;

	gls_reset_s3 <= gls_reset_sys;
	gls_clk_s3 <= gls_clk_sys;

	gls_reset_m1 <= gls_reset_sys;
	gls_clk_m1 <= gls_clk_sys;

	-----------------------------
	-- master => slave connexions
	-----------------------------
	-- address decode
  irq_cs <= '1' when wbm_address_m1 = "0000000000000" or wbm_address_m1 = "0000000000010"
	  						else '0';
	led_cs <= '1' when wbm_address_m1 = "0000000000100" 
								else '0';
	button_cs <= '1' when wbm_address_m1 = "0000000001000"
							 	else '0';

	wbs_address_s3 <= wbm_address_m1(1);

	-- control signals
		-- for led (s1)
	wbs_writedata_s1 <= wbm_writedata_m1 when (wbm_write_m1 and led_cs)='1' 
								else (others => '0');
	wbs_strobe_s1 <= (wbm_strobe_m1 and led_cs);
	wbs_write_s1 <= (wbm_write_m1 and led_cs);
		-- for button(s2) read only
	wbs_strobe_s2 <= (wbm_strobe_m1 and button_cs);
	wbs_write_s2 <= '0';

		-- for irq (s3)
	wbs_writedata_s3 <= wbm_writedata_m1 when (wbm_write_m1 and irq_cs)='1'
											else (others => '0');
	wbs_strobe_s3 <= (wbm_strobe_m1 and irq_cs);
	wbs_write_s3 <= (wbm_write_m1 and irq_cs);

  -----------------------------
	-- slave => master connexions
	-----------------------------
	cs(0) <=  led_cs;
	cs(1) <=  button_cs;
	cs(2) <=  irq_cs;

	with cs select
	wbm_readdata_m1 <= wbs_readdata_s1 when "001",
										 wbs_readdata_s2 when "010",
									 	 wbs_readdata_s3 when "100",
										(others => '0')  when others;
	with cs select
	wbm_ack_m1      <= wbs_ack_s1 when "001",
										 wbs_ack_s2 when "010",
										 wbs_ack_s3 when "100",
										 '0' when others;


	-- irq management
	wbs_irq_s3 <= wbs_irq_s2;

	-- open connexion
--	wbm_sel_m1 => open;
--	wbm_cycle_m1 => open;
--	wbs_ack_s1 => open;

end architecture intercon_1;

