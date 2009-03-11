----------------------------------------------------------------------------------
-- company: armadeus project
-- engineer: Benoît Canet
-- 
-- create date:    22:04:02 04/12/2007 
-- design name: 
-- module name:    clock_divider - behavioral 
-- project name: 
-- target devices: 
-- tool versions: 
-- description: 
--
-- dependencies: 
--
-- revision: 
-- revision 0.02 - add a reset (FabM)
-- revision 0.01 - file created
-- additional comments: 
--
----------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


entity clock_divider is

    -- clock division ratio (must be a multiple of 2)
	generic(ratio   : integer := 2);
    
    port (
             reset     : in  std_logic ;
             clk_in    : in  std_logic;
             clk_out   : out std_logic);

end clock_divider;

architecture behavioral of clock_divider is

    signal state : std_logic := '0';

begin

	process (clk_in,reset)
        variable count : integer  range 0 to ratio;
    begin
        if reset = '1' then
            count := 0;
            clk_out <= '0';
        elsif rising_edge(clk_in) then
            -- count the number of clock change
            count := count + 1;
            -- if we waited long enough generate one clock change
            if (count = ratio/2) then
                state <= not state;
                count := 0;
                clk_out <= state;
            end if; 
        end if;
        
    end process;

	

end behavioral;




