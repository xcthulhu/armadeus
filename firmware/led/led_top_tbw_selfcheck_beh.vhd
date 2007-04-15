--------------------------------------------------------------------------------
-- Copyright (c) 1995-2003 Xilinx, Inc.
-- All Right Reserved.
--------------------------------------------------------------------------------
--   ____  ____ 
--  /   /\/   / 
-- /___/  \  /    Vendor: Xilinx 
-- \   \   \/     Version : 9.1i
--  \   \         Application : ISE
--  /   /         Filename : led_top_tbw_selfcheck.vhw
-- /___/   /\     Timestamp : Fri Apr 13 22:51:44 2007
-- \   \  /  \ 
--  \___\/\___\ 
--
--Command: 
--Design Name: led_top_tbw_selfcheck_beh
--Device: Xilinx
--

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
USE IEEE.STD_LOGIC_TEXTIO.ALL;
USE STD.TEXTIO.ALL;

ENTITY led_top_tbw_selfcheck_beh IS
END led_top_tbw_selfcheck_beh;

ARCHITECTURE testbench_arch OF led_top_tbw_selfcheck_beh IS
    COMPONENT led_top
        PORT (
            DataxD : InOut std_logic_vector (15 DownTo 0);
            AddrxDI : In std_logic_vector (12 DownTo 0);
            WRxBI : In std_logic;
            RDxBI : In std_logic;
            CSxBI : In std_logic;
            clk : In std_logic;
            button : In std_logic;
            led : Out std_logic
        );
    END COMPONENT;

    SIGNAL DataxD : std_logic_vector (15 DownTo 0) := "0000000000000000";
    SIGNAL AddrxDI : std_logic_vector (12 DownTo 0) := "0000000000000";
    SIGNAL WRxBI : std_logic := '0';
    SIGNAL RDxBI : std_logic := '0';
    SIGNAL CSxBI : std_logic := '0';
    SIGNAL clk : std_logic := '0';
    SIGNAL button : std_logic := '0';
    SIGNAL led : std_logic := 'U';

    SHARED VARIABLE TX_ERROR : INTEGER := 0;
    SHARED VARIABLE TX_OUT : LINE;

    constant PERIOD : time := 20 ns;
    constant DUTY_CYCLE : real := 0.5;
    constant OFFSET : time := 100 ns;

    BEGIN
        UUT : led_top
        PORT MAP (
            DataxD => DataxD,
            AddrxDI => AddrxDI,
            WRxBI => WRxBI,
            RDxBI => RDxBI,
            CSxBI => CSxBI,
            clk => clk,
            button => button,
            led => led
        );

        PROCESS    -- clock process for clk
        BEGIN
            WAIT for OFFSET;
            CLOCK_LOOP : LOOP
                clk <= '0';
                WAIT FOR (PERIOD - (PERIOD * DUTY_CYCLE));
                clk <= '1';
                WAIT FOR (PERIOD * DUTY_CYCLE);
            END LOOP CLOCK_LOOP;
        END PROCESS;

        PROCESS
            PROCEDURE CHECK_led(
                next_led : std_logic;
                TX_TIME : INTEGER
            ) IS
                VARIABLE TX_STR : String(1 to 4096);
                VARIABLE TX_LOC : LINE;
                BEGIN
                IF (led /= next_led) THEN
                    STD.TEXTIO.write(TX_LOC, string'("Error at time="));
                    STD.TEXTIO.write(TX_LOC, TX_TIME);
                    STD.TEXTIO.write(TX_LOC, string'("ns led="));
                    IEEE.STD_LOGIC_TEXTIO.write(TX_LOC, led);
                    STD.TEXTIO.write(TX_LOC, string'(", Expected = "));
                    IEEE.STD_LOGIC_TEXTIO.write(TX_LOC, next_led);
                    STD.TEXTIO.write(TX_LOC, string'(" "));
                    TX_STR(TX_LOC.all'range) := TX_LOC.all;
                    STD.TEXTIO.Deallocate(TX_LOC);
                    ASSERT (FALSE) REPORT TX_STR SEVERITY ERROR;
                    TX_ERROR := TX_ERROR + 1;
                END IF;
            END;
            BEGIN
                -- -------------  Current Time:  129ns
                WAIT FOR 129 ns;
                button <= '1';
                -- -------------------------------------
                -- -------------  Current Time:  131ns
                WAIT FOR 2 ns;
                CHECK_led('0', 131);
                -- -------------------------------------
                -- -------------  Current Time:  169ns
                WAIT FOR 38 ns;
                button <= '0';
                -- -------------------------------------
                -- -------------  Current Time:  171ns
                WAIT FOR 2 ns;
                CHECK_led('1', 171);
                -- -------------------------------------
                -- -------------  Current Time:  189ns
                WAIT FOR 18 ns;
                button <= '1';
                -- -------------------------------------
                -- -------------  Current Time:  209ns
                WAIT FOR 20 ns;
                button <= '0';
                -- -------------------------------------
                -- -------------  Current Time:  249ns
                WAIT FOR 40 ns;
                button <= '1';
                -- -------------------------------------
                -- -------------  Current Time:  269ns
                WAIT FOR 20 ns;
                button <= '0';
                -- -------------------------------------
                -- -------------  Current Time:  309ns
                WAIT FOR 40 ns;
                button <= '1';
                -- -------------------------------------
                -- -------------  Current Time:  369ns
                WAIT FOR 60 ns;
                button <= '0';
                -- -------------------------------------
                -- -------------  Current Time:  549ns
                WAIT FOR 180 ns;
                button <= '1';
                -- -------------------------------------
                -- -------------  Current Time:  669ns
                WAIT FOR 120 ns;
                button <= '0';
                -- -------------------------------------
                -- -------------  Current Time:  809ns
                WAIT FOR 140 ns;
                button <= '1';
                -- -------------------------------------
                -- -------------  Current Time:  1029ns
                WAIT FOR 220 ns;
                button <= '0';
                -- -------------------------------------
                -- -------------  Current Time:  1251ns
                WAIT FOR 222 ns;
                CHECK_led('0', 1251);
                -- -------------------------------------
                -- -------------  Current Time:  1409ns
                WAIT FOR 158 ns;
                button <= '1';
                -- -------------------------------------
                -- -------------  Current Time:  2709ns
                WAIT FOR 1300 ns;
                button <= '0';
                -- -------------------------------------
                -- -------------  Current Time:  2931ns
                WAIT FOR 222 ns;
                CHECK_led('1', 2931);
                -- -------------------------------------
                -- -------------  Current Time:  4269ns
                WAIT FOR 1338 ns;
                button <= '1';
                -- -------------------------------------
                -- -------------  Current Time:  4309ns
                WAIT FOR 40 ns;
                button <= '0';
                -- -------------------------------------
                -- -------------  Current Time:  4769ns
                WAIT FOR 460 ns;
                button <= '1';
                -- -------------------------------------
                -- -------------  Current Time:  5429ns
                WAIT FOR 660 ns;
                button <= '0';
                -- -------------------------------------
                -- -------------  Current Time:  5651ns
                WAIT FOR 222 ns;
                CHECK_led('0', 5651);
                -- -------------------------------------
                -- -------------  Current Time:  6629ns
                WAIT FOR 978 ns;
                button <= '1';
                -- -------------------------------------
                -- -------------  Current Time:  8269ns
                WAIT FOR 1640 ns;
                button <= '0';
                -- -------------------------------------
                -- -------------  Current Time:  8491ns
                WAIT FOR 222 ns;
                CHECK_led('1', 8491);
                -- -------------------------------------
                WAIT FOR 1529 ns;

                IF (TX_ERROR = 0) THEN
                    STD.TEXTIO.write(TX_OUT, string'("No errors or warnings"));
                    ASSERT (FALSE) REPORT
                      "Simulation successful (not a failure).  No problems detected."
                      SEVERITY FAILURE;
                ELSE
                    STD.TEXTIO.write(TX_OUT, TX_ERROR);
                    STD.TEXTIO.write(TX_OUT,
                        string'(" errors found in simulation"));
                    ASSERT (FALSE) REPORT "Errors found during simulation"
                         SEVERITY FAILURE;
                END IF;
            END PROCESS;

    END testbench_arch;

