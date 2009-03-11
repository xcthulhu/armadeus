----------------------------------------------------------------------------------
-- company: armadeus project
-- engineer: Benoît Canet
-- 
-- create date:    22:37:46 04/12/2007 
-- design name: 
-- module name:    led_top - structural 
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
--
----------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity led_top is
    port ( 
             clk      : in  std_logic;
             button   : in  std_logic;
             led      : out  std_logic);
end led_top;

architecture structural of led_top is

    -- we declare the component we we'll use
    component clock_divider
        generic (ratio: integer);
        port (reset     : in std_logic ;
              clk_in 	: in  std_logic;
              clk_out   : out  std_logic);    
    end component;

    component debouncer
        port (clk : in std_logic ;
              reset : in std_logic ;
              clk_ratio 	        : in std_logic;
              button_in             : in std_logic;
              button_debounced_out 	: out std_logic);
    end component;

    component flip_flop
        port (
            clk   : in  std_logic;
            reset : in std_logic ;
            switch_in_n : in  std_logic;
            led_out     : out  std_logic
        );
    end component;

    component reset_gen
        generic (
                    invert_reset : std_logic := '0' -- 0 : not invert, 1 invert
                );
        port (
                 -- external signals
                 clk   : in std_logic ;
                 --internal signals
                 reset : out std_logic 
             );
    end component reset_gen;

    -- internal signals
    signal clk_ratio            : std_logic;
    signal reset                : std_logic;
    signal button_debounced_n   : std_logic;

begin

    -- instantiate and connect the clock divider 
    -- here we use a 96mhz/100hz=960000 ratio
    i_clock_divider_identifier : clock_divider 
        generic map(ratio   =>  960000)
            -- for simulation purpose use the following line
            -- instead of the previous
            -- generic map(ratio   =>  2)

        port map (reset     =>  reset,
                  clk_in    =>  clk,
                  clk_out   =>  clk_ratio);

    --instantiate and connect the debouncer
    i_debouncer_identifier : debouncer
        port map (  clk            => clk,
                    reset          => reset,
                    clk_ratio         => clk_ratio,
                    button_in         => button,
                    button_debounced_out  => button_debounced_n);

    --instantiate and connect the flip flop
    i_flip_flop_identifier : flip_flop
        port map (clk => clk,
                  reset => reset,
                  switch_in_n =>  button_debounced_n,
                  led_out   =>  led);

    i_reset_gen : reset_gen
        generic map (invert_reset => '0')
        port map (
                clk => clk,
                reset => reset
        );

end structural;

