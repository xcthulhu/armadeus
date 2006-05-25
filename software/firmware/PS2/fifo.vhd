library ieee;
use ieee.std_logic_1164.all;
use IEEE.STD_LOGIC_ARITH.all;
use IEEE.STD_LOGIC_UNSIGNED.all;

entity fifo_syn is
  generic (
    width        :     integer := 8;
    addrdepth    :     integer := 4);   -- Depth of the FIFO: 2^addrdepth
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
end fifo_syn;

architecture fifo_syn_a of fifo_syn is
  subtype wrdtype is std_logic_vector(width-1 downto 0);
  type regtype is array (0 to 15) of wrdtype;  --((2 ** addrdepth)-1) of wrdtype;
  signal reg : regtype;

  signal RdCntr : std_logic_vector(3 downto 0);
  signal WrCntr : std_logic_vector(3 downto 0);

  signal RW      : std_logic_vector(1 downto 0);
  signal fullxB  : std_logic;
  signal emptyxB : std_logic;
begin
  RW <= rd_en & wr_en;

  seq : process (ClkxC, ResetxRB)
  begin  -- process seq
    if ResetxRB = '0' then              -- asynchronous reset (active low)
      RdCntr                        <= (others => '0');
      WrCntr                        <= (others => '0');
      emptyxB                       <= '0';
      fullxB                        <= '1';
      for j in 0 to 15 loop
        reg(j)                      <= (others => '0');
      end loop;  -- j
      wr_ack                        <= '0';
    elsif ClkxC'event and ClkxC = '1' then  -- rising clock edge
      wr_ack                        <= '0';
      case RW is
        when "00"                              =>  -- read and write at the same time
          RdCntr                    <= RdCntr + 1;
          WrCntr                    <= WrCntr + 1;
          wr_ack                    <= '1';
          reg(conv_integer(WrCntr)) <= din;

        when "01" =>                    -- only read
          if (emptyxB = '1') then       -- not empty
            if ((RdCntr +1) = WrCntr) then
              emptyxB <= '0';
            end if;
            RdCntr    <= RdCntr + 1;
          end if;
          fullxB      <= '1';

        when "10" =>                    -- only write
          emptyxB                     <= '1';
          if fullxB = '1' then          -- not full
            reg(conv_integer(WrCntr)) <= din;
            if WrCntr+1 = RdCntr then
              fullxB                  <= '0';
            end if;
            WrCntr                    <= WrCntr +1;

          end if;

        when others => 
          wr_ack <= '1';
      end case;
    end if;
  end process seq;

  dout         <= reg(conv_integer(RdCntr));
  full         <= not fullxB;
  empty        <= not emptyxB;
  -- for future use ... some more status bits
  almost_full  <= '0';
  almost_empty <= '0';
  valid        <= '0';
  overflow     <= '0';
  underflow    <= '0';

  data_count <= WrCntr;

end fifo_syn_a;

