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
use work.apf_test_pkg.ALL;

----------------------------------------------------------------------------------
Entity led_top_tb is 
----------------------------------------------------------------------------------
	end entity led_top_tb;


----------------------------------------------------------------------------------
Architecture RTL of led_top_tb is
	----------------------------------------------------------------------------------

	CONSTANT HALF_PERIODE : time := 5 ns;  -- Half clock period

	signal imx_data       : std_logic_vector(15 downto 0);
	signal imx_data0_test : std_logic ;
	signal imx_addr1_test : std_logic ;
	signal imx_addr      : std_logic_vector (12 downto 1);
	signal imx_cs_n        : std_logic ;
	signal imx_eb3_n        : std_logic ;
	signal imx_oe_n        : std_logic ;
	signal clk       : std_logic ;
	signal LED          : std_logic ;
	--	signal Rst_n      : std_logic ;

	component bus_led_top
		port (
	 imx_data       : inout std_logic_vector(15 downto 0);
	 imx_data0_test : out std_logic ;
	 imx_addr1_test : out std_logic ;
	 imx_addr      : in std_logic_vector(12 downto 1);
	 imx_cs_n        : in std_logic ;
	 imx_eb3_n        : in std_logic ;
	 imx_oe_n        : in std_logic ;
	 clk       : in std_logic ;
	 LED          : out std_logic );
	end component bus_led_top;

begin
	-- Test stimulus
	stimulus : process
		variable value : std_logic_vector (15 downto 0);
	begin
		imx_oe_n <= '1';
		imx_eb3_n <= '1';
		imx_cs_n <= '1';
		imx_addr <= (others => 'Z');
		imx_data  <= (others => 'Z');

		wait for HALF_PERIODE*20;

		for i in 0 to 600 loop
			imx_write(x"0000",conv_std_logic_vector(i,16),
						clk,imx_cs_n,imx_oe_n,
						imx_eb3_n,imx_addr(12 downto 1),imx_data);
			imx_read(x"0000",value,
						clk,imx_cs_n,imx_oe_n,
						imx_eb3_n,imx_addr(12 downto 1),imx_data);
			assert value = conv_std_logic_vector(i,16) report "error reading imx_data" severity error;
		end loop;

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

	connect_bus_led : bus_led_top
	port map(
				imx_data       => imx_data,
				imx_data0_test => imx_data0_test,
				imx_addr1_test => imx_addr1_test,
				imx_addr      => imx_addr,
				imx_cs_n        => imx_cs_n,
				imx_eb3_n        => imx_eb3_n,
				imx_oe_n        => imx_oe_n,
				clk       => clk,
				LED          => LED);

--		Rst_n   => Rst_n);

end architecture RTL;

