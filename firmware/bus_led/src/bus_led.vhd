---------------------------------------------------------------------------
-- Company     : ARMadeus Systems
-- Author(s)   : Fabien Marteau
-- 
-- Creation Date : 15/02/2008
-- File          : bus_led.vhd
--
-- Abstract : 
--
---------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

---------------------------------------------------------------------------
Entity bus_led is 
	---------------------------------------------------------------------------
	port 
	(
		-- imx signals
		imx_data   	: inout std_logic_vector(15 downto 0);
		imx_addr 	: in	std_logic_vector(12 downto 1);
		imx_cs_n   	: in	std_logic ;
		imx_eb3_n   : in	std_logic ;
		imx_oe_n   	: in 	std_logic ;
		-- Clock 96MHz
		clk  		: in 	std_logic ;
		LED     	: out	std_logic ;
		-- Reset active low
		Rst_n   	: in	std_logic 
	);
end entity;


---------------------------------------------------------------------------
Architecture RTL of bus_led is
	---------------------------------------------------------------------------
	signal Reg        : std_logic_vector(15 downto 0) := (others => '0');
	signal address    : std_logic_vector(11 downto 0);
	signal readdata   : std_logic_vector(15 downto 0);
	signal writedata  : std_logic_vector(15 downto 0);
	signal read       : std_logic;
	signal write      : std_logic;

begin

	-- Led connection
	LED <= Reg(0);

	imx_data <= readdata when (read = '1') else (others => 'Z');  

	-----------------------------------------------------
	-- Synchro external signal
	-----------------------------------------------------
	sync_proc : process(clk, rst_n)
	 begin
	   if Rst_n = '0' then
	     address <= (others => '0');
	     read    <= '0';
	     write   <= '0';
	   elsif rising_edge(clk) then
	     write     <= not (imx_cs_n or imx_eb3_n);
	     read      <= not (imx_cs_n or imx_oe_n);
	     address   <= imx_addr;
	     writedata <= imx_data;
	   end if;
	 end process;

	writeprocess : process (clk,Rst_n)
	begin
		if Rst_n = '0' then
			Reg    <= (others => '0');
		elsif rising_edge(clk) then
			if (write = '1') then
				case address is
					when "000000000000" => 
						-- Save imx_data value in Reg
						Reg <= writedata;
					when others => Null; 
				end case;
			end if;
		end if;
	end process writeprocess;

	readprocess : process (clk, Rst_n)
	begin
		if Rst_n = '0' then
			readdata <= (others => '0');
		elsif rising_edge(clk) then
			readdata <= (others => '0');
			if(read = '1') then
				case address is
					when "000000000000" => 
					-- Write Reg value on imx_data bus
						readdata <= Reg;
					when others => Null; 
				end case;
			end if;
		end if;
	end process readprocess;

end architecture RTL;

