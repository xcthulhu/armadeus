---------------------------------------------------------------------------
-- Company     : ARMades Systems
-- Author(s)   : Fabien Marteau <fabien.marteau@armadeus.com>
-- 
-- Creation Date : 10/03/2008
-- File          : Wb_button.vhd
--
-- Abstract : 
--
---------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

---------------------------------------------------------------------------
Entity Wb_button is 
---------------------------------------------------------------------------
	port 
	(
		-- global signals
		gls_reset : in std_logic ;
		gls_clk 	: in std_logic ;
		-- Wishbone signals
--	wbs_writedata : in std_logic_vector( 15 downto 0);
		wbs_readdata  : out std_logic_vector( 15 downto 0);
		wbs_strobe    : in std_logic ;
		wbs_write	  : in std_logic ;
		wbs_ack	      : out std_logic;
		-- irq
		wbs_irq_s : out std_logic ;
		-- fpga input
		button 		: in std_logic 
	);
end entity;


---------------------------------------------------------------------------
Architecture Wb_button_1 of Wb_button is
	---------------------------------------------------------------------------
	signal button_r : std_logic ;
	signal reg : std_logic_vector( 15 downto 0);
begin

	-- connect button
	cbutton : process(gls_clk,gls_reset)
	begin
		if gls_reset = '1' then
			reg <= (others => '0');
		elsif rising_edge(gls_clk) then
			reg <= "000000000000000"&button;
		end if;
	end process cbutton;


	-- rise interruption
	pbutton : process(gls_clk,gls_reset)
	begin
		if gls_reset = '1' then
			wbs_irq_s <= '0';
			button_r <= '0';
		elsif rising_edge(gls_clk) then
			if button_r /= button then
				wbs_irq_s <= '1';
			else
				wbs_irq_s <= '0';
			end if;
			button_r <= button;
		end if;
	end process pbutton;

	-- register reading process
	pread : process(gls_clk,gls_reset)
	begin
		if(gls_reset = '1') then
			wbs_ack <= '0';
			wbs_readdata <= (others => '0');
		elsif(falling_edge(gls_clk)) then
			wbs_ack <= '1';
			if(wbs_strobe = '1' and wbs_write = '0')then
				wbs_readdata <= reg;
			end if;
		end if;
	end process pread;

end architecture Wb_button_1;

