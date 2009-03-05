---------------------------------------------------------------------------
-- Company     : ARMades Systems
-- Author(s)   : Fabien Marteau <fabien.marteau@armadeus.com>
-- 
-- Creation Date : 05/03/2009
-- File          : led_blink_tb.vhd
--
-- Abstract : 
--
---------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

---------------------------------------------------------------------------
Entity led_blink_tb is 
---------------------------------------------------------------------------
end entity;


---------------------------------------------------------------------------
Architecture led_blink_tb_1 of led_blink_tb is
---------------------------------------------------------------------------

    component Clk_div_led
        port (
                 Clk         : in  std_logic;
                 led_cathode : out std_logic;
                 led_anode   : out std_logic
             );
    end component Clk_div_led;


begin

    assert false report "TODO: testbench" severity error;

end architecture led_blink_tb_1;

