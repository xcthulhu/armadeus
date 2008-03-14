---------------------------------------------------------------------------
-- Company     : ARMadeus Systems
-- Author(s)   : Fabien Marteau
-- 
-- Creation Date : 05/03/2008
-- File          : Wb_led.vhd
--
-- Abstract : drive one led with Wishbone bus
--
---------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

-----------------------------------------------------------------------
	Entity Wb_led is 
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
Architecture Wb_led_1 of Wb_led is
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

end architecture Wb_led_1;

