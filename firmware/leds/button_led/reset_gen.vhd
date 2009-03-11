---------------------------------------------------------------------------
-- Company     : ARMadeus Systems
-- Author(s)   : Fabien Marteau
-- 
-- Creation Date : 05/03/2008
-- File          : reset_gen.vhd
--
-- Abstract : 
--
---------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

---------------------------------------------------------------------------
Entity reset_gen is 
---------------------------------------------------------------------------
generic(
    invert_reset : std_logic := '0' -- 0 : not invert, 1 invert
);
port 
(
	-- external signals
	clk : in std_logic ;
	--internal signals
	reset : out std_logic 
);
end entity;


---------------------------------------------------------------------------
Architecture reset_gen_1 of reset_gen is
---------------------------------------------------------------------------

	signal dly: std_logic := '0';
	signal rst: std_logic := '0';
	signal reset_tmp : std_logic;

begin

	reset_tmp <= '0';
    ----------------------------------------------------------------------------
    --  RESET signal generator.
    ----------------------------------------------------------------------------
    process(clk)
    begin
      if(rising_edge(clk)) then
        dly <= ( not(reset_tmp) and     dly  and not(rst) )
            or ( not(reset_tmp) and not(dly) and     rst  );
        rst <= ( not(reset_tmp) and not(dly) and not(rst) );
      end if;
    end process;
    reset <= rst xor invert_reset ;
	
end architecture reset_gen_1;

