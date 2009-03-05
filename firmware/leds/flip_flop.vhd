----------------------------------------------------------------------------------
-- Company: Armadeus Project
-- Engineer: Benoit Canet
-- 
-- Create Date:    21:21:27 04/12/2007 
-- Design Name: 
-- Module Name:    flip_flop - Behavioral 
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


entity flip_flop is
    port (clk_100hz    : in  STD_LOGIC;
          switch_in_n  : in  STD_LOGIC;
          led_out      : out  STD_LOGIC);
end flip_flop;


architecture behavioral of flip_flop is

begin

    process(clk_100hz)
        variable state      : STD_LOGIC := '0';
        variable latch      : STD_LOGIC := '0';        
    begin
        if clk_100hz'event and (clk_100hz = '1') then
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

