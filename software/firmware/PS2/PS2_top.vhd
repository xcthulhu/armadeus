--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:    23:04:42 03/13/06
-- Design Name:    
-- Module Name:    PS2_top - Behavioral
-- Project Name:   
-- Target Device:  
-- Tool versions:  
-- Description:
--
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
--------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.STD_LOGIC_ARITH.all;
use IEEE.STD_LOGIC_UNSIGNED.all;
library UNISIM;
use UNISIM.VComponents.all;


entity PS2_top is                       -- this is the top
  port ( DataxD  : inout std_logic_vector(15 downto 0);
         AddrxDI : in    std_logic_vector(9 downto 0);
         WRxBI   : in    std_logic;
         RDxBI   : in    std_logic;

         ClkxCI     : in    std_logic;
         CSxBI      : in    std_logic;
         PS2Clk     : inout std_logic;
         PS2Data    : inout std_logic;
         StatOutxDO : out   std_logic
--  --ResetxRB         : in std_logic
         );
end PS2_top;


architecture hierarchical of PS2_top is
-- signal Recv_Ena : std_logic;
-- signal Recv_Sample : std_logic;
  signal Recv_BRAM_Addr : std_logic_vector(9 downto 0);
  signal Recv_BRAM_Data : std_logic_vector(15 downto 0);
  signal Recv_BRAM_WEN  : std_logic;

  signal DataOutRAMxD : std_logic_vector(15 downto 0);
  signal CSBRAM       : std_logic := '0';
  signal WRBRAM       : std_logic := '0';
  signal CLKBRAM      : std_logic := '0';

begin
--
--
--  -- RAMB16_S18: Virtex-II/II-Pro, Spartan-3 1k x 16 + 2 Parity bits Single-Port RAM
--  -- Xilinx  HDL Language Template version 6.3.1i
--
  RAMB16_D18_18_inst : RAMB16_S18_S18
    generic map (
      INIT_A  => X"00000",              --  Value of output RAM registers at startup
      SRVAL_A => X"00000",              --  Ouput value upon SSR assertion
      INIT_00 => X"1100001100000000000000000000000000000000000000000000000010010110"
      )
    port map (
      DOA     => DataOutRAMxD,          -- 16-bit Data Output
      DOPA    => open,                  -- 2-bit parity Output
      ADDRA   => AddrxDI,               -- 10-bit Address Input
      CLKA    => CLKBRAM,               -- Clock
      DIA     => DataxD,                -- 16-bit Data Input
      DIPA    => "00",                  -- 2-bit parity Input
      ENA     => CSBRAM,                -- RAM Enable Input
      SSRA    => '0',                   -- Synchronous Set/Reset Input
      WEA     => WRBRAM,                -- Write Enable Input

      DOB   => open,                    -- 16-bit Data Output
      DOPB  => open,                    -- 2-bit parity Output
      ADDRB => Recv_BRAM_Addr,          -- 10-bit Address Input
      CLKB  => CLKBRAM,                 -- Clock
      DIB   => Recv_BRAM_Data,          -- 16-bit Data Input
      DIPB  => "00",                    -- 2-bit parity Input
      ENB   => '1',                     -- RAM Enable Input
      SSRB  => '0',                     -- Synchronous Set/Reset Input
      WEB   => Recv_BRAM_WEN            -- Write Enable Input

      );

  -- End of RAMB16_18_18_inst instantiation


  OutputEnable : process (DataOutRAMxD, CSxBI, RDxBI)
  begin
    if (CSxBI = '0' and RDxBI = '0') then  -- fix me
      DataxD <= DataOutRAMxD;
    else
      DataxD <= (others => 'Z');           --
    end if;
  end process OutputEnable;

  CSBRAM  <= not CSxBI;
  WRBRAM  <= not WRxBI;
  CLKBRAM <= ClkxCI;


  -- purpose: checks start and stop of reception
  -- type   : sequential
  -- inputs : ClkxCI, CSxBI
  -- outputs: 
  Recv_FSM                 : process (ClkxCI, CSxBI)
    variable bitcounter    : integer                       := 0;
    variable wordcounter   : std_logic_vector(9 downto 0)  := (others => '0');
    variable edge_detect_v : std_logic_vector(2 downto 0)  := "000";
    variable recv_buffer   : std_logic_vector(11 downto 0) := (others => '0');
  begin  -- process Recv_FSM     

    if CSxBI = '0' then                 -- asynchronous reset (active low)
      bitcounter    := 0;
      wordcounter   := (others    => '0');
      edge_detect_v := "000";
      Recv_BRAM_WEN    <= '0';
    elsif ClkxCI'event and ClkxCI = '1' then  -- rising clock edge
      Recv_BRAM_WEN    <= '0';
      Recv_BRAM_Data   <= (others => '0');
      StatOutxDO       <= '0';
      if edge_detect_v = "110" and PS2Clk = '0' then  ---and PS2Data = '0' then  -- = "110"
        recv_buffer := PS2Data & recv_buffer(11 downto 1);
        bitCounter  := bitCounter + 1;
        StatOutxDO     <= '1';
      elsif bitcounter = 11 then
        Recv_BRAM_WEN  <= '1';
        bitCounter  := bitcounter + 1;
        Recv_BRAM_Data <= "00000" & recv_buffer(1) & recv_buffer(10) & recv_buffer(11) & recv_buffer(9 downto 2);
      elsif bitcounter = 12 then
        bitcounter  := 0;
        wordCounter := wordCounter + 1;
      end if;
      edge_detect_v := edge_detect_v(1 downto 0) & PS2Clk;

    end if;
    Recv_BRAM_Addr <= wordCounter;

  end process Recv_FSM;

end hierarchical;
