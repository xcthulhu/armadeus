----------------------------------------------------------------------------------
-- company: armadeus project
-- engineer: Benoît Canet
-- 
-- create date:    21:48:05 04/12/2007 
-- design name: 
-- module name:    debouncer - behavioral 
-- project name: 
-- target devices: 
-- tool versions: 
-- description: 
--
-- dependencies: 
--
-- revision: 
-- revision 0.02 - Make it synchrone with reset (FabM)
-- revision 0.01 - file created
-- additional comments: 
-- original code comes from dennis smith
-- http://homepage.mac.com/dgsmith1/handouts/ee452/debounce.vhd.txt
----------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


entity debouncer is
    port ( clk            : in  std_logic;
           reset          : in std_logic ;
           clk_ratio      : in std_logic ;
           button_in            : in  std_logic;
           button_debounced_out : out  std_logic);
end debouncer;

architecture behavioral of debouncer is

    signal shift_pb : std_logic_vector(3 downto 0) := "0000";
    signal state    : std_logic := '1';

begin
    
    process(clk,reset)
        variable clk_ratio_old : std_logic := '0';
    begin
        if reset = '1' then
            state <= '0';
            clk_ratio_old := '0';
        elsif rising_edge(clk) then
            if clk_ratio = '0' and clk_ratio /= clk_ratio_old then
                -- shift the register
                shift_pb(2 downto 0)    <= shift_pb(3 downto 1);
                -- input the new sample in the register
                shift_pb(3)             <= button_in;
                -- change output if needed
                case shift_pb is
                    when "0000" => state <= '0';
                    when "1111" => state <= '1';
                    when others => state <= state;
                end case;
            end if;
            clk_ratio_old := clk_ratio;
        end if;
	end process;

    button_debounced_out <= state;

end behavioral;

