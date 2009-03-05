----------------------------------------------------------------------------------
-- Company: Armadeus Project
-- Engineer: Benoit Canet
-- 
-- Create Date:    21:48:05 04/12/2007 
-- Design Name: 
-- Module Name:    debouncer - behavioral 
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
-- original code comes from Dennis Smith
-- http://homepage.mac.com/dgsmith1/HANDOUTS/EE452/debounce.vhd.txt
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


entity debouncer is
    port ( clk_100hz            : in  STD_LOGIC;
           button_in            : in  STD_LOGIC;
           button_debounced_out : out  STD_LOGIC);
end debouncer;

architecture behavioral of debouncer is

    signal shift_pb : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0000";
    signal state    : STD_LOGIC := '1';

begin
    
    process(clk_100hz)
    begin
        if (clk_100hz'event) and (clk_100hz = '1')
        then
            -- shift the register
            shift_pb(2 DOWNTO 0)    <= shift_pb(3 DOWNTO 1);
            -- input the new sample in the register
            shift_pb(3)             <= button_in;
            -- change output if needed
            case shift_pb is
                when "0000" =>
                    state <= '0';
                when "1111" =>
                    state <= '1';
                when others =>
                    state <= state;
                end case;
        end if;
                
	end process;

    button_debounced_out <= state;

end behavioral;

