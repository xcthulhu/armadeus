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
	signal Reg  : std_logic_vector(15 downto 0) := (others => '0');

begin

	-- Led connection
	LED <= Reg(0);

	write : process (clk,Rst_n)
	begin
		if Rst_n = '0' then
			Reg    <= (others => '0');
		elsif rising_edge(clk) then
			if (imx_cs_n = '0' and	imx_oe_n = '1' and imx_eb3_n = '0' ) then
				case imx_addr is
					when "000000000000" => 
						-- Save imx_data value in Reg
						Reg <= imx_data;
					when others => 
						Reg <= Reg;
				end case;

			end if;
		end if;
	end process write;

	read : process (clk, Rst_n)
	begin
		if Rst_n = '0' then
			imx_data <= (others => 'Z');
		elsif falling_edge(clk) then
			if(imx_cs_n = '0' and imx_oe_n = '0' and imx_eb3_n = '1' ) then
				case imx_addr is
					when "000000000000" => 
					-- Write Reg value on imx_data bus
						imx_data <= Reg;
					when others => 
					-- High impedance if others address
						imx_data <= (others => 'Z');
				end case;
			else
					-- High impedance if others address
				imx_data <= (others => 'Z');
			end if;
		end if;
	end process read;

end architecture RTL;

