---------------------------------------------------------------------------
-- Company     : ARMadeus Systems
-- Author(s)   : Fabien Marteau
-- 
-- Creation Date : 05/03/2008
-- File          : syscon.vhd
--
-- Abstract : 
--
---------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

---------------------------------------------------------------------------
Entity syscon is 
---------------------------------------------------------------------------
port 
(
	-- external signals
	ext_clk : in std_logic ;
	ext_reset : in std_logic ;
	--internal signals
	gls_clk : out std_logic ;
	gls_reset : out std_logic 
);
end entity;


---------------------------------------------------------------------------
Architecture syscon_1 of syscon is
---------------------------------------------------------------------------

signal dly: std_logic := '0';
signal rst: std_logic := '0';

begin
----------------------------------------------------------------------------
--  RESET signal generator.
----------------------------------------------------------------------------
process(ext_clk)
begin
  if(rising_edge(ext_clk)) then
    dly <= ( not(ext_reset) and     dly  and not(rst) )
        or ( not(ext_reset) and not(dly) and     rst  );

    rst <= ( not(ext_reset) and not(dly) and not(rst) );
  end if;
end process;

gls_clk <= ext_clk;
gls_reset <= rst;
	
end architecture syscon_1;

