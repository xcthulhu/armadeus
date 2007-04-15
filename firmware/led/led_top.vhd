----------------------------------------------------------------------------------
-- Company: Armadeus Project
-- Engineer: Benoit Canet
-- 
-- Create Date:    22:37:46 04/12/2007 
-- Design Name: 
-- Module Name:    led_top - structural 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity led_top is
    port ( -- Imx pins
           DataxD  : inout std_logic_vector(15 downto 0);
           AddrxDI : in    std_logic_vector(12 downto 0);
           WRxBI   : in    std_logic;  
           RDxBI   : in    std_logic; 
           CSxBI   : in    std_logic;

           --Our pins
           clk      : in  STD_LOGIC;
           button   : in  STD_LOGIC;
           led      : out  STD_LOGIC);
end led_top;

architecture structural of led_top is

    -- We declare the component we we'll use
    component clock_divider
        generic (ratio: integer);
        port (clk_in 	: in  STD_LOGIC;
              clk_out   : out  STD_LOGIC);    
    end component;
	
	component debouncer
		port (clk_100hz 	        : in STD_LOGIC;
              button_in             : in STD_LOGIC;
              button_debounced_out 	: out STD_LOGIC);
	end component;

	component flip_flop
        port (clk_100hz   : in  STD_LOGIC;
              switch_in_n : in  STD_LOGIC;
			  led_out     : out  STD_LOGIC);
	end component;

    -- internal signals
	signal clk_100hz            : STD_LOGIC;
    signal button_debounced_n   : STD_LOGIC;
	
	begin
    
        -- instantiate and connect the clock divider 
        -- here we use a 96MHz/100Hz=960000 ratio
		i_clock_divider_identifier : clock_divider 
			generic map(ratio   =>  960000)
			-- for simulation purpose use the following line
            -- instead of the previous
            -- generic map(ratio   =>  2)
		
            port map (clk_in    =>  clk,
                      clk_out   =>  clk_100hz);

        --instantiate and connect the debouncer
		i_debouncer_identifier : debouncer
			port map (button_in         => button,
					  clk_100hz         => clk_100hz,
					  button_debounced_out  => button_debounced_n);

        --instantiate and connect the flip flop
		i_flip_flop_identifier : flip_flop
			port map (clk_100hz =>  clk_100hz,
					  switch_in_n =>  button_debounced_n,
					  led_out   =>  led);

end structural;

