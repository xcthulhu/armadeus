---------------------------------------------------------------------------
-- Company     : ARMadeus Systems
-- Author(s)   : Fabien Marteau (fabien.marteau@armadeus.com)
-- 
-- Creation Date : 05/03/2008
-- File          : Wb_top.vhd
--
-- Abstract : 
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
Entity Wb_top is 
---------------------------------------------------------------------------
port 
(
	-- imx EIM connexion
	imx_addr : in std_logic_vector( 12 downto 0);
	imx_data : inout std_logic_vector( 15 downto 0);
	imx_cs_n : in std_logic;
	imx_eb3_n : in std_logic ;
	imx_oe_n : in std_logic ;
	-- clock
	clk	: in std_logic ;
	-- fpga specific
	LED : out std_logic;
	button : in std_logic ;
	
	-- irq
	imx_irq : out std_logic

);
end entity;


---------------------------------------------------------------------------
Architecture Wb_top_1 of Wb_top is
---------------------------------------------------------------------------
	component imx_wrapper
		port (
      -- i.MX Signals
      imx_address : in    std_logic_vector(12 downto 1);
      imx_data    : inout std_logic_vector(15 downto 0);
      imx_cs_n    : in    std_logic;
      imx_oe_n    : in    std_logic;
      imx_eb3_n   : in    std_logic;

      -- Global Signals
      gls_reset : in std_logic;
      gls_clk   : in std_logic;

      -- Wishbone interface signals
      wbm_address    : out std_logic_vector(12 downto 0);  -- Address bus
      wbm_readdata   : in  std_logic_vector(15 downto 0);  -- Data bus for read access
      wbm_writedata  : out std_logic_vector(15 downto 0);  -- Data bus for write access
      wbm_strobe     : out std_logic;                      -- Data Strobe
      wbm_write      : out std_logic;                      -- Write access
		  wbm_sel        : out std_logic_vector(1 downto 0);   -- select LSB or/and MSB byte
			wbm_ack				 : in std_logic ;
      wbm_cycle      : out std_logic					   -- bus cycle in progress
    );
	end component;

	component syscon
		port( 
		-- external signals
		ext_clk : in std_logic ;
		ext_reset : in std_logic ;
		--internal signals
		gls_clk : out std_logic ;
		gls_reset : out std_logic 
		);
	end component;

	component intercon
		port (
		-- imx wishbone master connexion
					 gls_reset_m1        : out std_logic ;
					 gls_clk_m1          : out std_logic ;

					 wbm_address_m1      : in std_logic_vector( 12 downto 0);
					 wbm_readdata_m1     : out std_logic_vector( 15 downto 0);
					 wbm_writedata_m1    : in std_logic_vector( 15 downto 0);
					 wbm_strobe_m1       : in std_logic ;
					 wbm_write_m1        : in std_logic ;
					 wbm_sel_m1          : in std_logic_vector( 1 downto 0);
					 wbm_ack_m1 				 : out std_logic ;
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
--		  	 wbs_writedata_s2    : out std_logic_vector( 15 downto 0);
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
					 wbs_irq_s3					: out std_logic;

		-- syscon connection
					 gls_reset_sys       : in std_logic ;
					 gls_clk_sys         : in std_logic
				 );
	end component intercon;

	component Wb_led 
		port(
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
	end component;

	component Wb_button
		port (
				 -- global signals
					 gls_reset 				: in std_logic ;
					 gls_clk   				: in std_logic ;
--				 wbs_writedata    : in std_logic_vector( 15 downto 0);
					 wbs_readdata     : out std_logic_vector( 15 downto 0);
					 wbs_strobe       : in std_logic ;
					 wbs_write        : in std_logic ;
					 wbs_ack          : out std_logic ;

					 -- irq
					 wbs_irq_s : out std_logic ;

					 -- fpga input
					 button    : in std_logic 
	);
	end component;

	component irq_mngr
    generic
    (
      irq_count : integer := 1;
      irq_level : std_logic := '1'
    );
		port (
					 -- Global Signals
					 gls_clk   : in std_logic;
					 gls_reset : in std_logic;

					-- Wishbone interface signals
					 wbs_s1_address    : in  std_logic;                      -- Address bus
					 wbs_s1_readdata   : out std_logic_vector(15 downto 0);  -- Data bus for read access
					 wbs_s1_writedata  : in  std_logic_vector(15 downto 0);  -- Data bus for write access
					 wbs_s1_ack        : out std_logic;                      -- Access acknowledge
					 wbs_s1_strobe     : in  std_logic;                      -- Data Strobe
					 wbs_s1_write      : in  std_logic;                      -- Write access
					 -- irq from other IP
					 wbs_s1_irq        : in  std_logic_vector(irq_count-1 downto 0);

					 -- Component external signals
					 gls_irq           : out std_logic                       -- IRQ request
				 );
	end component irq_mngr;

	-- irq_mngr connection signal
	signal gls_clk_s3      : std_logic ;
	signal gls_reset_s3    : std_logic ;
	signal wbs_address_s3  : std_logic ;
	signal wbs_readdata_s3 : std_logic_vector(15 downto 0);
	signal wbs_writedata_s3: std_logic_vector( 15 downto 0);
	signal wbs_ack_s3			 : std_logic ;
	signal wbs_strobe_s3 	 : std_logic ;
	signal wbs_write_s3		 : std_logic ;
	signal wbs_irq_s3			 : std_logic;

	-- button connection signal
	signal gls_reset_s2 : std_logic ;
	signal gls_clk_s2	  : std_logic ;
	signal wbs_readdata_s2 : std_logic_vector(15 downto 0);
--signal wbs_writedata_s2: std_logic_vector( 15 downto 0);
	signal wbs_ack_s2			 : std_logic ;
	signal wbs_strobe_s2 	 : std_logic ;
	signal wbs_write_s2		 : std_logic ;

	signal wbs_irq_s2   : std_logic ;

	-- imx_wrapper connection signals
	signal gls_reset_m1 : std_logic ;
	Signal gls_clk_m1   : std_logic ;
	signal wbm_address_m1 : std_logic_vector( 12 downto 0);
	Signal wbm_readdata_m1 : std_logic_vector( 15 downto 0);
	Signal wbm_writedata_m1 : std_logic_vector( 15 downto 0);
	Signal wbm_strobe_m1 : std_logic ;
	signal wbm_write_m1 : std_logic ;
	Signal wbm_sel_m1 : std_logic_vector( 1 downto 0);
	signal wbm_ack_m1 : std_logic ;
	Signal wbm_cycle_m1 : std_logic ;

	-- led connection signals
	Signal gls_reset_s1 : std_logic ;
	Signal gls_clk_s1 : std_logic ;
	Signal wbs_writedata_s1 : std_logic_vector( 15 downto 0);
	Signal wbs_readdata_s1 : std_logic_vector( 15 downto 0);
	Signal wbs_strobe_s1 : std_logic ;
	Signal wbs_write_s1 : std_logic ;
	Signal wbs_ack_s1 : std_logic ;

	-- syscon connection signals
	Signal gls_reset_sys : std_logic ;
	Signal gls_clk_sys : std_logic ;


begin

	connect_imx_wrapper : imx_wrapper 
	port map (
			imx_address => imx_addr(12 downto 1),
			imx_data => imx_data,
			imx_cs_n =>  imx_cs_n,
			imx_oe_n => imx_oe_n,
			imx_eb3_n => imx_eb3_n,

			gls_reset => gls_reset_m1,
			gls_clk => gls_clk_m1,

			wbm_address => wbm_address_m1,
			wbm_readdata => wbm_readdata_m1,
			wbm_writedata => wbm_writedata_m1,
			wbm_strobe => wbm_strobe_m1,
			wbm_write => wbm_write_m1,
			wbm_sel => wbm_sel_m1,
			wbm_ack => wbm_ack_m1,
			wbm_cycle => wbm_cycle_m1
			 );

	connect_Wb_led : Wb_led
	port map (
			gls_reset => gls_reset_s1,
			gls_clk   => gls_clk_s1,

			wbs_writedata => wbs_writedata_s1,
			wbs_readdata => wbs_readdata_s1,
			wbs_strobe => wbs_strobe_s1,
			wbs_write => wbs_write_s1,
			wbs_ack => wbs_ack_s1,

			LED => LED 
		);

	connect_syscon : syscon
	port map (
		ext_clk => clk,
		ext_reset => '0',

		gls_clk => gls_clk_sys,
		gls_reset => gls_reset_sys
		);

	connect_intercon : intercon
	port map (
		-- imx_wrapper connexion
		gls_reset_m1 => gls_reset_m1,
		gls_clk_m1 => gls_clk_m1,
		wbm_address_m1 => wbm_address_m1,
		wbm_readdata_m1 => wbm_readdata_m1,
		wbm_writedata_m1 => wbm_writedata_m1,
		wbm_strobe_m1 => wbm_strobe_m1,
		wbm_write_m1 => wbm_write_m1,
		wbm_sel_m1 => wbm_sel_m1,
		wbm_ack_m1 => wbm_ack_m1,
		wbm_cycle_m1 => wbm_cycle_m1,
		-- led connexion
		gls_reset_s1 => gls_reset_s1,
		gls_clk_s1 => gls_clk_s1,
		wbs_writedata_s1 => wbs_writedata_s1,
		wbs_readdata_s1 => wbs_readdata_s1,
		wbs_strobe_s1 =>  wbs_strobe_s1,
		wbs_write_s1 => wbs_write_s1,
		wbs_ack_s1 => wbs_ack_s1,
		-- button connexion
		gls_reset_s2 => gls_reset_s2,
		gls_clk_s2 => gls_clk_s2,
--	wbs_writedata_s2 => wbs_writedata_s2,
		wbs_readdata_s2 => wbs_readdata_s2,
		wbs_strobe_s2 => wbs_strobe_s2,
		wbs_write_s2 => wbs_write_s2,
		wbs_ack_s2 => wbs_ack_s2,

		wbs_irq_s2 => wbs_irq_s2,
		-- irq connexion
		gls_reset_s3 => gls_reset_s3,
		gls_clk_s3 => gls_clk_s3,
		wbs_address_s3 => wbs_address_s3,
		wbs_writedata_s3 => wbs_writedata_s3,
		wbs_readdata_s3 => wbs_readdata_s3,
		wbs_strobe_s3 => wbs_strobe_s3,
		wbs_write_s3 => wbs_write_s3,
		wbs_ack_s3 => wbs_ack_s3,
		wbs_irq_s3 => wbs_irq_s3,
		-- global signals
		gls_reset_sys => gls_reset_sys,
		gls_clk_sys => gls_clk_sys
		);

	connect_button : Wb_button
	port map (
						 -- global signals
						 gls_reset     => gls_reset_s2,
						 gls_clk       => gls_clk_s2,
--					 wbs_writedata => wbs_writedata_s2,
						 wbs_readdata  => wbs_readdata_s2,
						 wbs_strobe    => wbs_strobe_s2,
						 wbs_write     => wbs_write_s2,
						 wbs_ack       => wbs_ack_s2,

						 -- irq
						 wbs_irq_s     => wbs_irq_s2,

						 -- fpga input
						 button        => button
					 );

	connect_irq_mngr : irq_mngr
	port map (
	-- Global Signals
						 gls_clk          => gls_clk_s3,
						 gls_reset        => gls_reset_s3,

	-- Wishbone interface signals
						 wbs_s1_address   => wbs_address_s3,   -- Address bus
						 wbs_s1_readdata  => wbs_readdata_s3,  -- Data bus for read access
						 wbs_s1_writedata => wbs_writedata_s3, -- Data bus for write access
						 wbs_s1_ack       => wbs_ack_s3,       -- Access acknowledge
						 wbs_s1_strobe    => wbs_strobe_s3,    -- Data Strobe
						 wbs_s1_write     => wbs_write_s3,     -- Write access
																										-- irq from other IP
						 wbs_s1_irq(0)    => wbs_irq_s3,

	-- Component external signals
						 gls_irq          => imx_irq 
					 );

end architecture Wb_top_1;

