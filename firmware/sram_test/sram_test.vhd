--------------------------------------------------------------------------------
-- Company: Armadeus Systems
-- Engineer: NC
--
-- Create Date:    23:04:42 08/26/07
-- Design Name:    DEVFULL SRAM TEST
-- Module Name:    SRAM_test - Behavioral
-- Project Name:   
-- Target Device:  
-- Tool versions:  
-- Description: Used to access the DevFull SRAM (CY7C1021) from UBOOT
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
--use IEEE.STD_LOGIC_ARITH.all;
--use IEEE.STD_LOGIC_UNSIGNED.all;
--library UNISIM;
--use UNISIM.VComponents.all;
--library XilinxCoreLib;
--use XilinxCoreLib.all;


entity sram_test is                       -- this is the top
  port ( DataxD    : inout std_logic_vector(15 downto 0);
         AddrxDI   : in  std_logic_vector(12 downto 0);
         WRxBI     : in  std_logic;
         RDxBI     : in  std_logic;

         ClkxCI    : in  std_logic;  --imx bus clock
         CSxBI     : in  std_logic;  -- fpga CS
			
			SRAM_Addr : out std_logic_vector(15 downto 0);  -- CY7C1021
			SRAM_Data : inout  std_logic_vector(15 downto 0);
			SRAM_WEn  : out std_logic;
			SRAM_OEn  : out std_logic;
			SRAM_CEn  : out std_logic;
			SRAM_BLEn : out std_logic;
			SRAM_BHEn : out std_logic
         );
end sram_test;

architecture hierarchical of sram_test is

  type readAccessType is ('0','1','2');
  subtype  reg is std_logic_vector (12 downto 0);
  
  constant SRAM_READ 		: readAccessType := '0';
  constant INTERNAL_READ 	: readAccessType := '1';
  constant OTHER_READ 		: readAccessType := '2';
  constant DATA_REGISTER   : reg        	  := b"0000000000010"; 
  constant ADDRESS_REGISTER: reg        	  := b"0000000000000";

  signal resetn       		: std_logic;
  signal DataxD_out 			: std_logic_vector(15 downto 0);
  signal regSRAM_Data_out 	: std_logic_vector(15 downto 0);
  signal SRAM_Addr_out 		: std_logic_vector(15 downto 0);
  signal regSRAM_WEn_out 	: std_logic;
  signal regSRAM_OEn_out 	: std_logic;
  signal readAccess 			: readAccessType;
  
begin

  resetn <= '1';

  -----------------------------------------------------------------------------
  -- Process
  -- purpose: Register Write from businterface
  -----------------------------------------------------------------------------
  TestRegister_write : process (CSxBI, ClkxCI, WRxBI, AddrxDI, DataxD )
  begin  -- process TestRegister_write
    if resetn = '0' then                -- asynchronous reset (active low)
	   regSRAM_Data_out <= (others => 'Z');
    elsif ClkxCI'event and ClkxCI = '1' then  -- rising clock edge
	   if CSxBI = '0' and WRxBI = '0' then
        case AddrxDI is
          when ADDRESS_REGISTER => 
			   SRAM_Addr_out <= DataxD(15 downto 0);
  	   	   regSRAM_WEn_out <= '1';
          when DATA_REGISTER => 
            regSRAM_Data_out <= DataxD(15 downto 0);
				regSRAM_WEn_out <= '0';
			 when others =>
            regSRAM_Data_out <= (others => '0');
 	   	   regSRAM_WEn_out <= '1';
		  end case;
		else
		  regSRAM_Data_out <= (others => 'Z');
		  regSRAM_WEn_out <= '1';
		end if;
    end if;
  end process TestRegister_write;

  
SRAM_read_process : process (ClkxCI, readAccess,  SRAM_Data )  
begin  -- process TestRegister_write
	  if ClkxCI'event and ClkxCI = '1' then
	    if readAccess = SRAM_READ  then
				DataxD_out <= SRAM_Data(15 downto 0);
		 elsif readAccess = INTERNAL_READ then
				DataxD_out    <= (others => '0');
		 else -- other 
	      DataxD_out    <= (others => 'Z');
		 end if;
	  end if;
end process SRAM_read_process;  
  
  TestRegister_read : process (CSxBI, RDxBI, AddrxDI, DataxD, ClkxCI )
  begin  -- process TestRegister_write

      if CSxBI = '0' and RDxBI = '0' then
        case AddrxDI is
          when DATA_REGISTER => -- data register
   		     regSRAM_OEn_out <= '0';
				  readAccess <= SRAM_READ; -- SRAM read
			 when others =>
				 readAccess <= INTERNAL_READ; -- INTERNAL read
  	   	   regSRAM_OEn_out <= '1';
		  end case;
   	else
			readAccess <= OTHER_READ;
		  regSRAM_OEn_out <= '1';
		end if;
  end process TestRegister_read;
  
 
  DataxD <= DataxD_out;
  SRAM_Addr <= SRAM_Addr_out(15 downto 0);
  SRAM_Data <= regSRAM_Data_out;
  SRAM_CEn  <= CSxBI;
  SRAM_OEn  <= regSRAM_OEn_out;
  SRAM_WEn  <= regSRAM_WEn_out;
  SRAM_BLEn <= '0';
  SRAM_BHEn <= '0';
 end hierarchical;
