----------------------------------------------------------------------------------
-- Company     : ARMadeus Systems
-- Author(s)   : Fabien Marteau
-- 
-- Creation Date : 14/02/2008
-- File          : led_top_tb.vhd
--
-- Abstract : Testbench for simple design that switch a led with the imx
--
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;
use ieee.std_logic_arith.all;

--library SIMPRIM;
--use SIMPRIM.VCOMPONENTS.ALL;
--use SIMPRIM.VPACKAGE.ALL;

use work.apf_test_pkg.ALL;
----------------------------------------------------------------------------------
Entity Wb_top_tb is 
----------------------------------------------------------------------------------
	end entity Wb_top_tb;


----------------------------------------------------------------------------------
Architecture RTL of Wb_top_tb is
	----------------------------------------------------------------------------------

	CONSTANT HALF_PERIODE : time := 5 ns;  -- Half clock period
  constant IRQ_PEND : std_logic_vector( 15 downto 0) := x"0002";
  constant IRQ_MASK : std_logic_vector( 15 downto 0) := x"0000";
  constant LED_REG  : std_logic_vector( 15 downto 0) := x"0004";
  constant BUTTON_REG : std_logic_vector( 15 downto 0) := x"0008";

	signal imx_data          : std_logic_vector(15 downto 0);
	signal imx_addr          : std_logic_vector (12 downto 0);
	signal imx_cs_n          : std_logic ;
	signal imx_eb3_n         : std_logic ;
	signal imx_oe_n          : std_logic ;
	signal clk               : std_logic ;
	signal LED              : std_logic ;
	signal imx_irq 					: std_logic ;
	signal button						: std_logic ;

	component Wb_top
		port (
					 -- imx EIM connexion
					 imx_addr  : in std_logic_vector( 12 downto 0);
					 imx_data  : inout std_logic_vector( 15 downto 0);
					 imx_cs_n  : in std_logic;
					 imx_eb3_n : in std_logic ;
					 imx_oe_n  : in std_logic ;
					 -- clock
					 clk       : in std_logic ;
					 -- fpga specific
					 LED       : out std_logic;
					 button    : in std_logic ;
					 -- irq
					 imx_irq   : out std_logic
				 );
	end component Wb_top;
begin

	connect_Wb_led : Wb_top
	port map(imx_data  => imx_data,
					 imx_addr  => imx_addr,
					 imx_cs_n  => imx_cs_n,
					 imx_eb3_n => imx_eb3_n,
					 imx_oe_n  => imx_oe_n,
					 clk       => clk,
					 LED       => LED,
					 button    => button,
					 imx_irq   => imx_irq);

	-- test read/write
	stimulus : process
		variable value : std_logic_vector (15 downto 0);
	begin
		imx_oe_n <= '1';
		imx_eb3_n <= '1';
		imx_cs_n <= '1';
		imx_addr <= (others => 'Z');
		imx_data  <= (others => 'Z');
		button <= '0';

		wait for HALF_PERIODE*10;

		-- reset irq_pend
		imx_write(IRQ_PEND,x"0000",
		clk,imx_cs_n,imx_oe_n,
		imx_eb3_n,imx_addr(12 downto 0),imx_data,
    4);
		-- write on irq_mask
		imx_write(IRQ_MASK,x"ffff",
		clk,imx_cs_n,imx_oe_n,
		imx_eb3_n,imx_addr(12 downto 0),imx_data,
    4);
		-- read led value
		imx_read(LED_REG,value,
		clk,imx_cs_n,imx_oe_n,
		imx_eb3_n,imx_addr(12 downto 0),imx_data,
    4);
		assert value(0) = '0' report "Wrong led value" severity error;

		-- push button
		button <= '1';
		wait for HALF_PERIODE*20;
		assert imx_irq = '1' report "IRQ not raised" severity error;
		-- read button value
		imx_read(BUTTON_REG,value,
		clk,imx_cs_n,imx_oe_n,
		imx_eb3_n,imx_addr(12 downto 0),imx_data,
    4);
		assert value(0) = '1' report  "Failed to read button" severity error;

		-- Read irq pending
		imx_read(IRQ_PEND,value,
		clk,imx_cs_n,imx_oe_n,
		imx_eb3_n,imx_addr(12 downto 0),imx_data,
    4);
		assert value(0) = '1' report "IRQ pending error" severity error;
		-- Acknowledge irq
		imx_write(IRQ_PEND,x"ffff",
		clk,imx_cs_n,imx_oe_n,
		imx_eb3_n,imx_addr(12 downto 0),imx_data,
    4);
		imx_read(IRQ_PEND,value,
		clk,imx_cs_n,imx_oe_n,
		imx_eb3_n,imx_addr(12 downto 0),imx_data,
    4);
		assert value(0) = '0' report "Acknowledge error" severity error;

		-- release button
		button <= '0';
		wait for HALF_PERIODE*20;
		-- Acknowledge irq
		imx_write(IRQ_PEND,x"ffff",
		clk,imx_cs_n,imx_oe_n,
		imx_eb3_n,imx_addr(12 downto 0),imx_data,
    4);
		imx_read(IRQ_PEND,value,
		clk,imx_cs_n,imx_oe_n,
		imx_eb3_n,imx_addr(12 downto 0),imx_data,
    4);
		--read button value
		imx_read(BUTTON_REG,value,
		clk,imx_cs_n,imx_oe_n,
		imx_eb3_n,imx_addr(12 downto 0),imx_data,
    4);
		assert value(0) = '0' report "Low read button error" severity error;
	

		

		wait for HALF_PERIODE*10;
		assert false report "End of test" severity error;
	end process stimulus;		

			-- 100MHz Clock
	Clockp : process
	begin
		clk <= '1';
		wait for HALF_PERIODE;
		clk <= '0';
		wait for HALF_PERIODE;
	end process Clockp;

end architecture RTL;
