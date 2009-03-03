---------------------------------------------------------------------------
-- Company     : Automaticaly generated by POD
-- Author(s)   : 
-- 
-- Creation Date : 2009-03-03
-- File          : Top_wishbone_example.vhd
--
-- Abstract : 
-- insert a description here
--
---------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity top_wishbone_example is

    port
    (
    imx9328_wb16_wrapper00_imx_data : inout std_logic_vector(15 downto 0);
    imx9328_wb16_wrapper00_imx_address : in std_logic_vector(11 downto 0);
    rstgen_syscon00_ext_clk : in std_logic;
    imx9328_wb16_wrapper00_imx_cs_n : in std_logic;
    imx9328_wb16_wrapper00_imx_eb3_n : in std_logic;
    imx9328_wb16_wrapper00_imx_oe_n : in std_logic;
    irq_mngr00_gls_irq : out std_logic;
    led0_led : out std_logic;
    button0_button : in std_logic
    );
end entity top_wishbone_example;

architecture top_wishbone_example_1 of top_wishbone_example is
    -------------------------
    -- declare components  --
    -------------------------


    component rstgen_syscon
        port (
            -- candr
            gls_clk  : out std_logic;
            gls_reset  : out std_logic;
            -- clk_ext
            ext_clk  : in std_logic
        );
    end component;

    component led
        generic(
            id : natural := 2;
            wb_size : natural := 16
        );
        port (
            -- int_led
            led  : out std_logic;
            -- candr
            gls_reset  : in std_logic;
            gls_clk  : in std_logic;
            -- swb16
            wbs_add  : in std_logic;
            wbs_writedata  : in std_logic_vector(15 downto 0);
            wbs_readdata  : out std_logic_vector(15 downto 0);
            wbs_strobe  : in std_logic;
            wbs_cycle  : in std_logic;
            wbs_write  : in std_logic;
            wbs_ack  : out std_logic
        );
    end component;

    component button
        generic(
            id : natural := 3
        );
        port (
            -- int_button
            button  : in std_logic;
            irq  : out std_logic;
            -- candr
            gls_reset  : in std_logic;
            gls_clk  : in std_logic;
            -- swb16
            wbs_add  : in std_logic;
            wbs_readdata  : out std_logic_vector(15 downto 0);
            wbs_strobe  : in std_logic;
            wbs_write  : in std_logic;
            wbs_ack  : out std_logic;
            wbs_cycle  : in std_logic
        );
    end component;

    component imx9328_wb16_wrapper
        port (
            -- eim
            imx_address  : in std_logic_vector(11 downto 0);
            imx_data  : inout std_logic_vector(15 downto 0);
            imx_cs_n  : in std_logic;
            imx_oe_n  : in std_logic;
            imx_eb3_n  : in std_logic;
            -- candr
            gls_reset  : in std_logic;
            gls_clk  : in std_logic;
            -- mwb16
            wbm_address  : out std_logic_vector(12 downto 0);
            wbm_readdata  : in std_logic_vector(15 downto 0);
            wbm_writedata  : out std_logic_vector(15 downto 0);
            wbm_strobe  : out std_logic;
            wbm_write  : out std_logic;
            wbm_ack  : in std_logic;
            wbm_cycle  : out std_logic
        );
    end component;

    component irq_mngr
        generic(
            id : natural := 1;
            irq_level : std_logic := '1';
            irq_count : integer := 1
        );
        port (
            -- candr
            gls_clk  : in std_logic;
            gls_reset  : in std_logic;
            -- swb16
            wbs_s1_address  : in std_logic_vector(1 downto 0);
            wbs_s1_readdata  : out std_logic_vector(15 downto 0);
            wbs_s1_writedata  : in std_logic_vector(15 downto 0);
            wbs_s1_ack  : out std_logic;
            wbs_s1_strobe  : in std_logic;
            wbs_s1_cycle  : in std_logic;
            wbs_s1_write  : in std_logic;
            -- irq
            irqport  : in std_logic_vector(0 downto 0);
            -- ext_irq
            gls_irq  : out std_logic
        );
    end component;

    component imx9328_wb16_wrapper00_mwb16
        port (
            -- irq_mngr00_swb16
            irq_mngr00_wbs_s1_address  : out std_logic_vector(1 downto 0);
            irq_mngr00_wbs_s1_readdata  : in std_logic_vector(15 downto 0);
            irq_mngr00_wbs_s1_writedata  : out std_logic_vector(15 downto 0);
            irq_mngr00_wbs_s1_ack  : in std_logic;
            irq_mngr00_wbs_s1_strobe  : out std_logic;
            irq_mngr00_wbs_s1_cycle  : out std_logic;
            irq_mngr00_wbs_s1_write  : out std_logic;
            -- irq_mngr00_candr
            irq_mngr00_gls_clk  : out std_logic;
            irq_mngr00_gls_reset  : out std_logic;
            -- led0_swb16
            led0_wbs_add  : out std_logic;
            led0_wbs_writedata  : out std_logic_vector(15 downto 0);
            led0_wbs_readdata  : in std_logic_vector(15 downto 0);
            led0_wbs_strobe  : out std_logic;
            led0_wbs_cycle  : out std_logic;
            led0_wbs_write  : out std_logic;
            led0_wbs_ack  : in std_logic;
            -- led0_candr
            led0_gls_reset  : out std_logic;
            led0_gls_clk  : out std_logic;
            -- button0_swb16
            button0_wbs_add  : out std_logic;
            button0_wbs_readdata  : in std_logic_vector(15 downto 0);
            button0_wbs_strobe  : out std_logic;
            button0_wbs_write  : out std_logic;
            button0_wbs_ack  : in std_logic;
            button0_wbs_cycle  : out std_logic;
            -- button0_candr
            button0_gls_reset  : out std_logic;
            button0_gls_clk  : out std_logic;
            -- imx9328_wb16_wrapper00_mwb16
            imx9328_wb16_wrapper00_wbm_address  : in std_logic_vector(12 downto 0);
            imx9328_wb16_wrapper00_wbm_readdata  : out std_logic_vector(15 downto 0);
            imx9328_wb16_wrapper00_wbm_writedata  : in std_logic_vector(15 downto 0);
            imx9328_wb16_wrapper00_wbm_strobe  : in std_logic;
            imx9328_wb16_wrapper00_wbm_write  : in std_logic;
            imx9328_wb16_wrapper00_wbm_ack  : out std_logic;
            imx9328_wb16_wrapper00_wbm_cycle  : in std_logic;
            -- imx9328_wb16_wrapper00_candr
            imx9328_wb16_wrapper00_gls_reset  : out std_logic;
            imx9328_wb16_wrapper00_gls_clk  : out std_logic;
            -- rstgen_syscon00_imx9328_wb16_wrapper00
            rstgen_syscon00_gls_clk  : in std_logic;
            rstgen_syscon00_gls_reset  : in std_logic
        );
    end component;
    -------------------------
    -- Signals declaration
    -------------------------

    -- imx9328_wb16_wrapper00
    -- eim
    -- candr
    signal imx9328_wb16_wrapper00_gls_reset :  std_logic;
    signal imx9328_wb16_wrapper00_gls_clk :  std_logic;
    -- mwb16
    signal imx9328_wb16_wrapper00_wbm_address :  std_logic_vector(12 downto 0);
    signal imx9328_wb16_wrapper00_wbm_readdata :  std_logic_vector(15 downto 0);
    signal imx9328_wb16_wrapper00_wbm_writedata :  std_logic_vector(15 downto 0);
    signal imx9328_wb16_wrapper00_wbm_strobe :  std_logic;
    signal imx9328_wb16_wrapper00_wbm_write :  std_logic;
    signal imx9328_wb16_wrapper00_wbm_ack :  std_logic;
    signal imx9328_wb16_wrapper00_wbm_cycle :  std_logic;

    -- rstgen_syscon00
    -- candr
    signal rstgen_syscon00_gls_clk :  std_logic;
    signal rstgen_syscon00_gls_reset :  std_logic;
    -- clk_ext

    -- irq_mngr00
    -- candr
    signal irq_mngr00_gls_clk :  std_logic;
    signal irq_mngr00_gls_reset :  std_logic;
    -- swb16
    signal irq_mngr00_wbs_s1_address :  std_logic_vector(1 downto 0);
    signal irq_mngr00_wbs_s1_readdata :  std_logic_vector(15 downto 0);
    signal irq_mngr00_wbs_s1_writedata :  std_logic_vector(15 downto 0);
    signal irq_mngr00_wbs_s1_ack :  std_logic;
    signal irq_mngr00_wbs_s1_strobe :  std_logic;
    signal irq_mngr00_wbs_s1_cycle :  std_logic;
    signal irq_mngr00_wbs_s1_write :  std_logic;
    -- irq
    signal irq_mngr00_irqport :  std_logic_vector(0 downto 0);
    -- ext_irq

    -- led0
    -- int_led
    -- candr
    signal led0_gls_reset :  std_logic;
    signal led0_gls_clk :  std_logic;
    -- swb16
    signal led0_wbs_add :  std_logic;
    signal led0_wbs_writedata :  std_logic_vector(15 downto 0);
    signal led0_wbs_readdata :  std_logic_vector(15 downto 0);
    signal led0_wbs_strobe :  std_logic;
    signal led0_wbs_cycle :  std_logic;
    signal led0_wbs_write :  std_logic;
    signal led0_wbs_ack :  std_logic;

    -- button0
    -- int_button
    signal button0_irq :  std_logic;
    -- candr
    signal button0_gls_reset :  std_logic;
    signal button0_gls_clk :  std_logic;
    -- swb16
    signal button0_wbs_add :  std_logic;
    signal button0_wbs_readdata :  std_logic_vector(15 downto 0);
    signal button0_wbs_strobe :  std_logic;
    signal button0_wbs_write :  std_logic;
    signal button0_wbs_ack :  std_logic;
    signal button0_wbs_cycle :  std_logic;

    -- imx9328_wb16_wrapper00_mwb16_intercon
    -- irq_mngr00_swb16
    signal imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_address :  std_logic_vector(1 downto 0);
    signal imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_readdata :  std_logic_vector(15 downto 0);
    signal imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_writedata :  std_logic_vector(15 downto 0);
    signal imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_ack :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_strobe :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_cycle :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_write :  std_logic;
    -- irq_mngr00_candr
    signal imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_gls_clk :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_gls_reset :  std_logic;
    -- led0_swb16
    signal imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_add :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_writedata :  std_logic_vector(15 downto 0);
    signal imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_readdata :  std_logic_vector(15 downto 0);
    signal imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_strobe :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_cycle :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_write :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_ack :  std_logic;
    -- led0_candr
    signal imx9328_wb16_wrapper00_mwb16_intercon_led0_gls_reset :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_led0_gls_clk :  std_logic;
    -- button0_swb16
    signal imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_add :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_readdata :  std_logic_vector(15 downto 0);
    signal imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_strobe :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_write :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_ack :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_cycle :  std_logic;
    -- button0_candr
    signal imx9328_wb16_wrapper00_mwb16_intercon_button0_gls_reset :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_button0_gls_clk :  std_logic;
    -- imx9328_wb16_wrapper00_mwb16
    signal imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_address :  std_logic_vector(12 downto 0);
    signal imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_readdata :  std_logic_vector(15 downto 0);
    signal imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_writedata :  std_logic_vector(15 downto 0);
    signal imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_strobe :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_write :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_ack :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_cycle :  std_logic;
    -- imx9328_wb16_wrapper00_candr
    signal imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_gls_reset :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_gls_clk :  std_logic;
    -- rstgen_syscon00_imx9328_wb16_wrapper00
    signal imx9328_wb16_wrapper00_mwb16_intercon_rstgen_syscon00_gls_clk :  std_logic;
    signal imx9328_wb16_wrapper00_mwb16_intercon_rstgen_syscon00_gls_reset :  std_logic;

    -- void pins

begin
    -------------------------
    -- declare instances
    -------------------------

    imx9328_wb16_wrapper00 : imx9328_wb16_wrapper
    port map (
            -- eim
            imx_address => imx9328_wb16_wrapper00_imx_address,
            imx_data => imx9328_wb16_wrapper00_imx_data,
            imx_cs_n => imx9328_wb16_wrapper00_imx_cs_n,
            imx_oe_n => imx9328_wb16_wrapper00_imx_oe_n,
            imx_eb3_n => imx9328_wb16_wrapper00_imx_eb3_n,
            -- candr
            gls_reset => imx9328_wb16_wrapper00_gls_reset,
            gls_clk => imx9328_wb16_wrapper00_gls_clk,
            -- mwb16
            wbm_address => imx9328_wb16_wrapper00_wbm_address,
            wbm_readdata => imx9328_wb16_wrapper00_wbm_readdata,
            wbm_writedata => imx9328_wb16_wrapper00_wbm_writedata,
            wbm_strobe => imx9328_wb16_wrapper00_wbm_strobe,
            wbm_write => imx9328_wb16_wrapper00_wbm_write,
            wbm_ack => imx9328_wb16_wrapper00_wbm_ack,
            wbm_cycle => imx9328_wb16_wrapper00_wbm_cycle
            );

    rstgen_syscon00 : rstgen_syscon
    port map (
            -- candr
            gls_clk => rstgen_syscon00_gls_clk,
            gls_reset => rstgen_syscon00_gls_reset,
            -- clk_ext
            ext_clk => rstgen_syscon00_ext_clk
            );

    irq_mngr00 : irq_mngr
    generic map (
            id => 1,
            irq_level => '1',
            irq_count => 1
        )
    port map (
            -- candr
            gls_clk => irq_mngr00_gls_clk,
            gls_reset => irq_mngr00_gls_reset,
            -- swb16
            wbs_s1_address => irq_mngr00_wbs_s1_address,
            wbs_s1_readdata => irq_mngr00_wbs_s1_readdata,
            wbs_s1_writedata => irq_mngr00_wbs_s1_writedata,
            wbs_s1_ack => irq_mngr00_wbs_s1_ack,
            wbs_s1_strobe => irq_mngr00_wbs_s1_strobe,
            wbs_s1_cycle => irq_mngr00_wbs_s1_cycle,
            wbs_s1_write => irq_mngr00_wbs_s1_write,
            -- irq
            irqport => irq_mngr00_irqport,
            -- ext_irq
            gls_irq => irq_mngr00_gls_irq
            );

    led0 : led
    generic map (
            id => 2,
            wb_size => 16
        )
    port map (
            -- int_led
            led => led0_led,
            -- candr
            gls_reset => led0_gls_reset,
            gls_clk => led0_gls_clk,
            -- swb16
            wbs_add => led0_wbs_add,
            wbs_writedata => led0_wbs_writedata,
            wbs_readdata => led0_wbs_readdata,
            wbs_strobe => led0_wbs_strobe,
            wbs_cycle => led0_wbs_cycle,
            wbs_write => led0_wbs_write,
            wbs_ack => led0_wbs_ack
            );

    button0 : button
    generic map (
            id => 3
        )
    port map (
            -- int_button
            button => button0_button,
            irq => button0_irq,
            -- candr
            gls_reset => button0_gls_reset,
            gls_clk => button0_gls_clk,
            -- swb16
            wbs_add => button0_wbs_add,
            wbs_readdata => button0_wbs_readdata,
            wbs_strobe => button0_wbs_strobe,
            wbs_write => button0_wbs_write,
            wbs_ack => button0_wbs_ack,
            wbs_cycle => button0_wbs_cycle
            );

    imx9328_wb16_wrapper00_mwb16_intercon : imx9328_wb16_wrapper00_mwb16
    port map (
            -- irq_mngr00_swb16
            irq_mngr00_wbs_s1_address => imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_address,
            irq_mngr00_wbs_s1_readdata => imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_readdata,
            irq_mngr00_wbs_s1_writedata => imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_writedata,
            irq_mngr00_wbs_s1_ack => imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_ack,
            irq_mngr00_wbs_s1_strobe => imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_strobe,
            irq_mngr00_wbs_s1_cycle => imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_cycle,
            irq_mngr00_wbs_s1_write => imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_write,
            -- irq_mngr00_candr
            irq_mngr00_gls_clk => imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_gls_clk,
            irq_mngr00_gls_reset => imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_gls_reset,
            -- led0_swb16
            led0_wbs_add => imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_add,
            led0_wbs_writedata => imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_writedata,
            led0_wbs_readdata => imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_readdata,
            led0_wbs_strobe => imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_strobe,
            led0_wbs_cycle => imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_cycle,
            led0_wbs_write => imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_write,
            led0_wbs_ack => imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_ack,
            -- led0_candr
            led0_gls_reset => imx9328_wb16_wrapper00_mwb16_intercon_led0_gls_reset,
            led0_gls_clk => imx9328_wb16_wrapper00_mwb16_intercon_led0_gls_clk,
            -- button0_swb16
            button0_wbs_add => imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_add,
            button0_wbs_readdata => imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_readdata,
            button0_wbs_strobe => imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_strobe,
            button0_wbs_write => imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_write,
            button0_wbs_ack => imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_ack,
            button0_wbs_cycle => imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_cycle,
            -- button0_candr
            button0_gls_reset => imx9328_wb16_wrapper00_mwb16_intercon_button0_gls_reset,
            button0_gls_clk => imx9328_wb16_wrapper00_mwb16_intercon_button0_gls_clk,
            -- imx9328_wb16_wrapper00_mwb16
            imx9328_wb16_wrapper00_wbm_address => imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_address,
            imx9328_wb16_wrapper00_wbm_readdata => imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_readdata,
            imx9328_wb16_wrapper00_wbm_writedata => imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_writedata,
            imx9328_wb16_wrapper00_wbm_strobe => imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_strobe,
            imx9328_wb16_wrapper00_wbm_write => imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_write,
            imx9328_wb16_wrapper00_wbm_ack => imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_ack,
            imx9328_wb16_wrapper00_wbm_cycle => imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_cycle,
            -- imx9328_wb16_wrapper00_candr
            imx9328_wb16_wrapper00_gls_reset => imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_gls_reset,
            imx9328_wb16_wrapper00_gls_clk => imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_gls_clk,
            -- rstgen_syscon00_imx9328_wb16_wrapper00
            rstgen_syscon00_gls_clk => imx9328_wb16_wrapper00_mwb16_intercon_rstgen_syscon00_gls_clk,
            rstgen_syscon00_gls_reset => imx9328_wb16_wrapper00_mwb16_intercon_rstgen_syscon00_gls_reset
            );

    ---------------------------
    -- instances connections --
    ---------------------------

    -- connect imx9328_wb16_wrapper00
        -- eim
        -- candr
        imx9328_wb16_wrapper00_gls_reset <= imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_gls_reset;
        imx9328_wb16_wrapper00_gls_clk <= imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_gls_clk;
        -- mwb16
        imx9328_wb16_wrapper00_wbm_readdata <= imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_readdata;
        imx9328_wb16_wrapper00_wbm_ack <= imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_ack;

    -- connect rstgen_syscon00
        -- candr
        -- clk_ext

    -- connect irq_mngr00
        -- candr
        irq_mngr00_gls_clk <= imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_gls_clk;
        irq_mngr00_gls_reset <= imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_gls_reset;
        -- swb16
        irq_mngr00_wbs_s1_address <= imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_address;
        irq_mngr00_wbs_s1_writedata <= imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_writedata;
        irq_mngr00_wbs_s1_strobe <= imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_strobe;
        irq_mngr00_wbs_s1_cycle <= imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_cycle;
        irq_mngr00_wbs_s1_write <= imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_write;
        -- irq
        irq_mngr00_irqport(0) <= button0_irq;
        -- ext_irq

    -- connect led0
        -- int_led
        -- candr
        led0_gls_reset <= imx9328_wb16_wrapper00_mwb16_intercon_led0_gls_reset;
        led0_gls_clk <= imx9328_wb16_wrapper00_mwb16_intercon_led0_gls_clk;
        -- swb16
        led0_wbs_add <= imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_add;
        led0_wbs_writedata <= imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_writedata;
        led0_wbs_strobe <= imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_strobe;
        led0_wbs_cycle <= imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_cycle;
        led0_wbs_write <= imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_write;

    -- connect button0
        -- int_button
        -- candr
        button0_gls_reset <= imx9328_wb16_wrapper00_mwb16_intercon_button0_gls_reset;
        button0_gls_clk <= imx9328_wb16_wrapper00_mwb16_intercon_button0_gls_clk;
        -- swb16
        button0_wbs_add <= imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_add;
        button0_wbs_strobe <= imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_strobe;
        button0_wbs_write <= imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_write;
        button0_wbs_cycle <= imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_cycle;

    -- connect imx9328_wb16_wrapper00_mwb16_intercon
        -- irq_mngr00_swb16
        imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_readdata <= irq_mngr00_wbs_s1_readdata;
        imx9328_wb16_wrapper00_mwb16_intercon_irq_mngr00_wbs_s1_ack <= irq_mngr00_wbs_s1_ack;
        -- irq_mngr00_candr
        -- led0_swb16
        imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_readdata <= led0_wbs_readdata;
        imx9328_wb16_wrapper00_mwb16_intercon_led0_wbs_ack <= led0_wbs_ack;
        -- led0_candr
        -- button0_swb16
        imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_readdata <= button0_wbs_readdata;
        imx9328_wb16_wrapper00_mwb16_intercon_button0_wbs_ack <= button0_wbs_ack;
        -- button0_candr
        -- imx9328_wb16_wrapper00_mwb16
        imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_address <= imx9328_wb16_wrapper00_wbm_address;
        imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_writedata <= imx9328_wb16_wrapper00_wbm_writedata;
        imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_strobe <= imx9328_wb16_wrapper00_wbm_strobe;
        imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_write <= imx9328_wb16_wrapper00_wbm_write;
        imx9328_wb16_wrapper00_mwb16_intercon_imx9328_wb16_wrapper00_wbm_cycle <= imx9328_wb16_wrapper00_wbm_cycle;
        -- imx9328_wb16_wrapper00_candr
        -- rstgen_syscon00_imx9328_wb16_wrapper00
        imx9328_wb16_wrapper00_mwb16_intercon_rstgen_syscon00_gls_clk <= rstgen_syscon00_gls_clk;
        imx9328_wb16_wrapper00_mwb16_intercon_rstgen_syscon00_gls_reset <= rstgen_syscon00_gls_reset;

end architecture top_wishbone_example_1;
