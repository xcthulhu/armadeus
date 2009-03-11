----------------------------------------------------------------------------------
-- company: armadeus project
-- engineer: Benoît Canet
-- 
-- create date:    21:21:27 04/12/2007 
-- design name: 
-- module name:    flip_flop - behavioral 
-- project name: 
-- target devices: 
-- tool versions: 
-- description: 
--
-- dependencies: 
--
-- revision: 
-- revision 0.02 - Synchronize with high speed clock (FabM)
-- revision 0.01 - file created
-- additional comments: 
--
----------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity flip_flop is
    port (
        clk  : in  std_logic;
        reset : in std_logic ;
        switch_in_n  : in  std_logic;
        led_out      : out  std_logic
    );
end flip_flop;


architecture behavioral of flip_flop is

begin

    led_output : process(clk,reset)
        variable state      : std_logic := '0';
        variable latch      : std_logic := '0';        
    begin
        if reset = '1' then
            led_out <= '0';
            latch := '0';
            state := '0';
        elsif rising_edge(clk) then
            -- switch_in_n is active when down 
            --( the switch is connected via a pullup)
            if (switch_in_n = '0') and (latch /= switch_in_n) then 
                state := not state;
            end if;
            latch := switch_in_n;
            led_out <= state;
        end if;	
        
    end process;
     
end behavioral;

