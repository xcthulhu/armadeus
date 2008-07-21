-------------------------------------------------------------------------------
--
--  File          :  imx_wrapper.vhd
--  Related files :  (none)
--
--  Author(s)     :  Fabrice Mousset (fabrice.mousset@laposte.net)
--  Project       :  i.MX wrapper to Wishbone bus
--
--  Creation Date :  2007/01/19
--
--  Description   :  This is the top file of the IP
-------------------------------------------------------------------------------
--  Modifications : 
-----
-- 2008/03/05
-- Fabien Marteau (fabien.marteau@armadeus.com)
-- adding comments and changing some signals names
--
-------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2, or (at your option)
-- any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
----------------------------------------------------------------------------------


library IEEE;
  use IEEE.std_logic_1164.all;
  use IEEE.numeric_std.all;

-- ----------------------------------------------------------------------------
    Entity imx_wrapper is
-- ----------------------------------------------------------------------------
    port
    (
      -- i.MX Signals
      imx_address : in    std_logic_vector(12 downto 1);
      imx_data    : inout std_logic_vector(15 downto 0);
      imx_cs_n    : in    std_logic;
      imx_oe_n    : in    std_logic;
      imx_eb3_n   : in    std_logic;

      -- Global Signals
      gls_reset : in std_logic;
      gls_clk   : in std_logic;

      -- Wishbone interface signals
      wbm_address    : out std_logic_vector(12 downto 0);  -- Address bus
      wbm_readdata   : in  std_logic_vector(15 downto 0);  -- Data bus for read access
      wbm_writedata  : out std_logic_vector(15 downto 0);  -- Data bus for write access
      wbm_strobe     : out std_logic;                      -- Data Strobe
      wbm_write      : out std_logic;                      -- Write access
		  wbm_sel        : out std_logic_vector(1 downto 0);   -- select LSB or/and MSB byte
			wbm_ack				 : in std_logic ;		                   -- acknowledge
      wbm_cycle      : out std_logic					   -- bus cycle in progress
    );
    end entity;

-- ----------------------------------------------------------------------------
    Architecture RTL of imx_wrapper is
-- ----------------------------------------------------------------------------

signal write      : std_logic;
signal read       : std_logic;
signal sel        : std_logic_vector(1 downto 0);
signal strobe     : std_logic;
signal writedata  : std_logic_vector(15 downto 0);
signal address    : std_logic_vector(12 downto 0);

begin

-- ----------------------------------------------------------------------------
--  External signals synchronization process
-- ----------------------------------------------------------------------------
process(gls_clk, gls_reset)
begin
  if(gls_reset='1') then
    write   <= '0';
    read    <= '0';
    sel     <= "00";
    strobe  <= '0';
    writedata <= (others => '0');
    address   <= (others => '0');
  elsif(rising_edge(gls_clk)) then
    strobe  <= not (imx_cs_n) and not(imx_oe_n and imx_eb3_n);
    write   <= not (imx_cs_n or imx_eb3_n);
    read    <= not (imx_cs_n or imx_oe_n);
    address <= imx_address & '0';
    writedata <= imx_data;
  end if;
end process;

wbm_address    <= address when (strobe = '1') else (others => '0');
wbm_writedata  <= writedata when (write = '1') else (others => '0');
wbm_strobe     <= strobe;
wbm_write      <= write;
wbm_sel        <= strobe & strobe;
wbm_cycle      <= strobe;

imx_data <= wbm_readdata when(read = '1') else (others => 'Z');

end architecture RTL;
