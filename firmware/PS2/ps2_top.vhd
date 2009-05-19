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
library XilinxCoreLib;
use XilinxCoreLib.all;


entity PS2_top is                       -- this is the top
  port ( DataxD  : inout std_logic_vector(15 downto 0);
         AddrxDI : in    std_logic_vector(12 downto 0);
         WRxBI   : in    std_logic;
         RDxBI   : in    std_logic;

         ClkxCI     : in    std_logic;
         CSxBI      : in    std_logic;
         PS2Clk     : inout std_logic;
         PS2Data    : inout std_logic;
         StatOutxDO : out   std_logic
         );
end PS2_top;


architecture hierarchical of PS2_top is
  component fifo_syn
    generic (
      width        :     integer;
      addrdepth    :     integer);
    port (
      ClkxC        : in  std_logic;
      din          : in  std_logic_vector(width-1 downto 0);
      rd_en        : in  std_logic;
      ResetxRB     : in  std_logic;
      wr_en        : in  std_logic;
      almost_empty : out std_logic;
      almost_full  : out std_logic;
      data_count   : out std_logic_vector(addrdepth-1 downto 0);
      dout         : out std_logic_vector(width-1 downto 0);
      empty        : out std_logic;
      full         : out std_logic;
      overflow     : out std_logic;
      valid        : out std_logic;
      underflow    : out std_logic;
      wr_ack       : out std_logic);
  end component;

  signal Recv_BRAM_Data : std_logic_vector(15 downto 0);
  signal Recv_BRAM_WEN  : std_logic;

  signal DataOutRAMxD : std_logic_vector(15 downto 0);
  signal resetn       : std_logic;
  signal reg1         : std_logic_vector(7 downto 0);
  signal reg2         : std_logic_vector(7 downto 0);

  signal dout         : std_logic_vector(7 downto 0);
  signal empty        : std_logic;
  signal full         : std_logic;
  signal overflow     : std_logic;
  signal underflow    : std_logic;
  signal valid        : std_logic;
  signal almost_empty : std_logic;
  signal almost_full  : std_logic;
  signal wr_ack       : std_logic;
  signal data_count   : std_logic_vector(3 downto 0);

  signal readaccess          : std_logic;
  signal readAccessPulse     : std_logic;
  signal readAccessPulseFiFo : std_logic;
  
begin

  resetn <= '1';


  -----------------------------------------------------------------------------
  -- Process
  -- purpose: Register Write from businterface
  -----------------------------------------------------------------------------
  PS2register_write : process (ClkxCI, resetn)
  begin  -- process PS2register_write
    if resetn = '0' then                -- asynchronous reset (active low)
      reg1       <= "10000000";
      reg2       <= (others => '0');
    elsif ClkxCI'event and ClkxCI = '1' then  -- rising clock edge
      if CSxBI = '0' and WRxBI = '0' then
        case AddrxDI is
          when "0000000000100" =>
            reg1 <= DataxD(7 downto 0);
          when "0000000000110" =>
            reg2 <= DataxD(7 downto 0);
          when others       => null;
        end case;
      end if;
    end if;
  end process PS2register_write;



  -----------------------------------------------------------------------------
  -- Process
  -- purpose: Generic Bus-Interface for read access
  -----------------------------------------------------------------------------
  PS2register_read_syn            : process (ClkxCI, resetn)
    variable ReadAccessEdgeDetect : std_logic;
  begin  -- process PS2register_read_syn
    if resetn = '0' then                -- asynchronous reset (active low)
      ReadAccessPulse   <= '1';
      ReadAccessEdgeDetect := '0';
      DataOutRAMxD      <= (others => 'Z');
    elsif ClkxCI'event and ClkxCI = '1' then  -- rising clock edge
      if (ReadAccessEdgeDetect = '0' and ((not CSxBI) and (not RDxBI)) = '1') then
        ReadAccessPulse <= '0';
      else
        ReadAccessPulse <= '1';
      end if;
      ReadAccessEdgeDetect := (not CSxBI) and (not RDxBI);
    end if;
  end process PS2register_read_syn;


  
  -----------------------------------------------------------------------------
  -- Process
  -- purpose: Register Read from businterface
  -----------------------------------------------------------------------------
  PS2register_read : process ( CSxBI, RDxBI, AddrxDI, reg1, reg2, readAccessPulse, 
                               Recv_BRAM_Data, valid, overflow, full, almost_full, empty, almost_empty, dout, data_count)
  begin  -- process PS2register_read
    readAccessPulseFiFo <= '1';
    if CSxBI = '0' and RDxBI = '0' then
      case AddrxDI is
        when "0000000000000" =>
          DataOutRAMxD <= "00" & data_count(1 downto 0) & "0000" & "00" & valid & overflow & full & almost_full & empty & almost_empty;
        when "0000000000010" =>
          DataOutRAMxD <= "00000000" & dout;
          if readAccessPulse = '0' then
            readAccessPulseFiFo <= '0';
          end if;
        when "0000000000100" =>
          DataOutRAMxD <= "00000000" & reg1;
        when "0000000000110" =>
          DataOutRAMxD <= "00000000" & reg2;

        when "0000000001000"         =>
          DataOutRAMxD <= "0001001000110100";
        when others               =>
          DataOutRAMxD <= (others => '0');
      end case;
    else
      DataOutRAMxD     <= (others => 'Z');
    end if;
  end process PS2register_read;
--
  DataxD               <= DataOutRAMxD;


  -----------------------------------------------------------------------------
  -- FIFO Instantiation
  -----------------------------------------------------------------------------
  ps2_read_fifo : fifo_syn
    generic map (
      width        => 8,
      addrdepth    => 4)
    port map (
      ClkxC        => ClkxCI,
      din          => Recv_BRAM_Data(7 downto 0),
      rd_en        => ReadAccessPulseFiFo,  --rd_en,
      ResetxRB     => resetn,
      wr_en        => Recv_BRAM_WEN,
      almost_empty => almost_empty,
      almost_full  => almost_full,
      data_count   => data_count,
      dout         => dout,
      empty        => empty,
      full         => full,
      overflow     => overflow,
      valid        => valid,
      underflow    => underflow,
      wr_ack       => wr_ack);


  -----------------------------------------------------------------------------
  -- Process
  -- purpose: PS2 receiver
  -----------------------------------------------------------------------------
  Recv_FSM                 : process (ClkxCI, resetn)
    variable bitcounter    : integer                       := 0;
    variable edge_detect_v : std_logic_vector(2 downto 0)  := "000";
    variable recv_buffer   : std_logic_vector(11 downto 0) := (others => '0');
  begin  -- process Recv_FSM     

    if resetn = '0' then                -- asynchronous reset (active low)
      bitcounter     := 0;
      edge_detect_v  := "000";
      Recv_BRAM_WEN    <= '1';
    elsif ClkxCI'event and ClkxCI = '1' then  -- rising clock edge
      Recv_BRAM_WEN    <= '1';
      if edge_detect_v = "110" and PS2Clk = '0' then  ---and PS2Data = '0' then  -- = "110"
        recv_buffer  := PS2Data & recv_buffer(11 downto 1);
        bitCounter   := bitCounter + 1;
      elsif bitcounter = 11 then
        Recv_BRAM_WEN  <= '0';
        Recv_BRAM_Data <= "00000" & recv_buffer(1) & recv_buffer(10) & recv_buffer(11) & recv_buffer(9 downto 2);
        if wr_ack = '1' then
          bitCounter := bitcounter + 1;
        end if;
      elsif bitcounter = 12 then
        bitcounter   := 0;
      end if;
      edge_detect_v  := edge_detect_v(1 downto 0) & PS2Clk;

    end if;

  end process Recv_FSM;

  StatOutxDO <= wr_ack;
end hierarchical;
