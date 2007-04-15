----------------------------------------------------------------------------------
-- Company: Armadeus Project
-- Engineer: Benoit Canet
-- 
-- Create Date:    22:04:02 04/12/2007 
-- Design Name: 
-- Module Name:    clock_divider - behavioral 
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


entity clock_divider is

    -- clock division ratio (must be a multiple of 2)
	generic(ratio   : integer := 2);
    
    port (clk_in    : in  STD_LOGIC;
          clk_out   : out  STD_LOGIC);

end clock_divider;

architecture behavioral of clock_divider is

    signal state : STD_LOGIC := '0';

begin

	process (clk_in)

        variable count : integer  range 0 to ratio;
	
    begin

        if (clk_in'event) and (clk_in = '1') then

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




