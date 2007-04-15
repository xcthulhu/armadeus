--------------------------------------------------------------------------------
-- Copyright (c) 1995-2003 Xilinx, Inc.
-- All Right Reserved.
--------------------------------------------------------------------------------
--   ____  ____ 
--  /   /\/   / 
-- /___/  \  /    Vendor: Xilinx 
-- \   \   \/     Version : 9.1i
--  \   \         Application : ISE
--  /   /         Filename : clock_divider_tbw.vhw
-- /___/   /\     Timestamp : Fri Apr 13 23:07:43 2007
-- \   \  /  \ 
--  \___\/\___\ 
--
--Command: 
--Design Name: clock_divider_tbw
--Device: Xilinx
--

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
USE IEEE.STD_LOGIC_TEXTIO.ALL;
USE STD.TEXTIO.ALL;

ENTITY clock_divider_tbw IS
END clock_divider_tbw;

ARCHITECTURE testbench_arch OF clock_divider_tbw IS
    COMPONENT clock_divider
        PORT (
            clk_in : In std_logic;
            clk_out : Out std_logic
        );
    END COMPONENT;

    SIGNAL clk_in : std_logic := '0';
    SIGNAL clk_out : std_logic := '0';

    constant PERIOD : time := 20 ns;
    constant DUTY_CYCLE : real := 0.5;
    constant OFFSET : time := 100 ns;

    BEGIN
        UUT : clock_divider
        PORT MAP (
            clk_in => clk_in,
            clk_out => clk_out
        );

        PROCESS    -- clock process for clk_in
        BEGIN
            WAIT for OFFSET;
            CLOCK_LOOP : LOOP
                clk_in <= '0';
                WAIT FOR (PERIOD - (PERIOD * DUTY_CYCLE));
                clk_in <= '1';
                WAIT FOR (PERIOD * DUTY_CYCLE);
            END LOOP CLOCK_LOOP;
        END PROCESS;

        PROCESS
            BEGIN
                WAIT FOR 1020 ns;

            END PROCESS;

    END testbench_arch;

