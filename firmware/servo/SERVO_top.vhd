--------------------------------------------------------------------------------
-- Copyright (C) 2008  Armadeus Project

-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.


-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.

-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------
-- Company: Armadeus Project
-- Engineer: Yvan ROCH
-- 
-- Create Date: 2007-12-03
-- Design Name:
-- Module Name: SERVO_top - structural 
-- Project Name: SERVO
-- Target Devices: Spartan3 Armadeus
-- Tool versions: ISE 9.2.02i
-- Description: Servo Contoller
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Revision 0.02 - 2008-05-11
-- Application of sonzerro and Fabrice Mousset recommendations:
-- 		- All comments are in english.
-- 		- Synchronous design, using of a single clock and removing the writeValue
-- 		signal.
-- 		- Using a reset signal.
-- 		- Using of rising_edge.
-- 		- Using generic parameters (number of servos...).
-- 		- Using generator for multiple entity.
-- 		- Using integer with range.
-- Removing unedeed signal readAccessPulse.
-- Adding MagicID register for firmware recognition from Linux driver.
-- Adding Firmware Version register for versatile configuration of Linux driver.
-- Adding Number of servos register for versatile configuration of Linux driver.
-- Adding Command/Status register for send command (like reset) to the firmware.
-- Revision 0.03 - 2008-07-09
-- 		- Use IEEE.NUMERIC_STD only. Avoid the use of SYNOPSYS Library
-- 		IEEE.STD_LOGIC_UNSIGNED and IEEE.STD_LOGIC_ARITH.
-- 		- Synchronous design, using of a single clock (removing 4MHz unused clock)
-- 		- Asynchronous Reset
-- 
-- Additional Comments:
-- This project is a R/C servos controller FPGA Firmware for the Xilinx Spartan3 embedded 
-- on the Armadeus board. It is inspired from the project 
-- "Atmel AVR 2004 Design Contest Entry Project Number A3722" of Circuit Cellar
-- from Eric Gagnon :
-- http://www.circuitcellar.com/avr2004/HA3722.html
-- 
-- This controler is able to drive from 1 to 32 R/C servos. 
-- This number is controled by the generic parameter 'nbrServos'
-- 
-- Addresses Mapping
-- Spartan3 Base Address on Armadeus Board:	0x12000000
-- Magic ID Register:				0x12000000
-- Firmware Version Register:		0x12000002
-- Number of R/C servos managed:	0x12000004
-- Status Register:					0x12000006
-- Command Register:				0x12000008
-- Enable Servo Register LSB:		0x1200000A
-- Enable Servo Register MSB:		0x1200000C
-- Base Address of Servos Position Registers	0x12000010
--
-- Each servo has a 16 bits position register with only 12 signifiant bits that containt the servo position.
-- Example:
-- 0000 written at address 0x12000010 will put the servo 0 on the maximal counterclockwise position.
-- 0800 written at address 0x12000010 will put the servo 0 on the median position.
-- 0FFF written at address 0x12000010 will put the servo 0 on the maximal clockwise position.
--
-- The Enable Servo Register LSB and Enable Servo Register MSB are the servos enable control registers:
-- XXX1 written at address 0x1200000A will enable the servo 0 (the servo will keep his planed position)
-- XXX0 written at address 0x1200000A will disable the servo 0 (the servo is freewheeling)
-- XXX1 written at address 0x1200000C will enable the servo 16 (the servo will keep his planed position)
-- XXX0 written at address 0x1200000C will disable the servo 16 (the servo is freewheeling)
--
-- Status Register:
-- bit0: If set, a write error occured
--
-- Command Register:
-- bit0: If set, the firmware will reset
--------------------------------------------------------------------------------

library IEEE;
-- IEEE.STD_LOGIC_UNSIGNED.ALL and IEEE.STD_LOGIC_ARITH.ALL are not used because
-- they may have some incompability with some synthesizer (SYNOPSYS library)
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity SERVO_top is
	-- nbrServos is the number of servos managed by this module
	-- nbrServos is not declared as a constant because it is used is the
	-- port declaration section of this module (the synthetizer does not permit that)
	-- When changing nbrServos you must modify the servo.ucf file for connecting
	-- pwmOut<XX> properly.
 	generic ( nbrServos : integer := 32);
	port ( -- MC9328MXL pins
		-- MC9328MXL 16 bits Data Bus
		DataxD : inout std_logic_vector(15 downto 0);
		
		-- MC9328MXL Address Bus A0-A12
		-- The Spartan3 base addressis 0x12000000
		AddrxDI : in std_logic_vector(12 downto 0);
		
		-- Write access (APF9328: CTRL9, Spartan3 pin 60, MC9328MXL: EB3n pin M5) Active Low
		-- This signal from the MC9328MXL is active low for write AND read operations
		-- According to MC9328MXL Data Sheet p4:
		-- LSB Byte Strobe Active low external enable byte signal that controls D [7:0].
		-- See Write timing p28
		WRxBI : in std_logic;
		
		-- Read access (APF9328: CTRL10, Spartan3 pin 63, MC9328MXL: OEn pin R5) Active Low
		-- This signal from the MC9328MXL is active low for read operation SOLELY
		-- According to MC9328MXL Data Sheet p4:
		-- Memory Output Enable?Active low output enables external data bus.
		-- See Read timing P26
		RDxBI : in std_logic;
		
		-- Comments on  WRxBI et RDxBI:
		-- RDxBI is in fact OEn, active SOLELY on read operations on the MC9328MXL external bus.
		-- WRxBI is in fact EB3n, active on read AND write operations on the LSB byte of the data bus.
		-- It is not very clear because these signals are not mutually exclusive!!!
		-- To resume:
		-- On read operations: RDxBI=0 et WRxBI=0 (both acvite low)
		-- On write operations: RDxBI=1 et WRxBI=0 (both acvite low)
		
		-- Chip select (APF9328: CTRL1, Spartan3 pin 59, MC9328MXL: CS1n M8) Active Low
		CSxBI : in std_logic; 
		
		-- Clock input at 96 MHz from  MC9328MXL on Spartan3 pin P55
		clkInImx96MHz : in  STD_LOGIC;
		
		-- nbrServos 1 bit register declaration for PWM outputs
		pwmOut : out std_logic_vector(nbrServos-1 downto 0)
	);
end entity SERVO_top;

architecture structural of SERVO_top is
	-- Contants declaration
	-- Magic ID
	constant regMagicID : std_logic_vector(15 downto 0) := X"7207";
	-- Firmware Version
	constant regFirmwareVersion : std_logic_vector(15 downto 0) := X"0003";
	-- MagicID register Address: 0x12000000
	constant regMagicIDAddr : std_logic_vector(12 downto 0) := "0000000000000";
	-- Firmaware version register Address:0x12000002
	constant regFirmwareVersionAddr : std_logic_vector(12 downto 0) := "0000000000010";
	-- Number of servos register Address: 0x12000004
	constant regNumberServosAddr : std_logic_vector(12 downto 0) := "0000000000100";
	-- Status register Address: 0x12000006
	constant regStatusAddr : std_logic_vector(12 downto 0) := "0000000000110";
	-- Command register Address: 0x12000008
	constant regCmdAddr : std_logic_vector(12 downto 0) := "0000000001000";
	-- Servos Validation Register LSB Address: 0x1200000A
	constant regServosValidationLSBAddr : std_logic_vector(12 downto 0) := "0000000001010";
	-- Servos Validation Register MSB Address: 0x1200000C
	constant regServosValidationMSBAddr : std_logic_vector(12 downto 0) := "0000000001100";
	-- Servos Position Registers base address : 0x12000010
	-- Servo N Position Register address is : N*2 + addrRegServosPositionDase
	constant addrRegServosPositionDaseAddr : std_logic_vector(12 downto 0) := "0000000010000";

	-- Type reg12Array declaration that containts nbrServos signals of 12 bits
	-- This type is used for the sorvos positions registers
	type reg12Array is array(0 to nbrServos-1) of std_logic_vector(11 downto 0);
	
	-- Reset Generator Declaration
	component RESET_MODULE
		-- Ports declaration
		port 
		(
			-- Clock Input
			clkIn : in std_logic ;
			-- External Reset Input
			resetIn : in std_logic ;
			-- Well formed reset signal
			resetOut : out std_logic 
		);
	end component RESET_MODULE;

	-- Counter declaration
	component COUNTER_MODULE
		-- Ports declaration
		port (clkIn : in  STD_LOGIC;
			counterOut16Bits	: out std_logic_vector(15 downto 0));
	end component COUNTER_MODULE;

	-- PWM generator declaration
	component PWM_MODULE
		-- Ports declaration
		Port (count : in std_logic_vector(15 downto 0);
			clkIn : in std_logic;
			servosReg : in std_logic_vector( 11 downto 0);
			enableReg: in std_logic;
			pwmOut : out std_logic);
	end component PWM_MODULE;
	
	-- Signals declaration
	-- nbrServos 16 bits positon registers declaration
	signal servosReg : reg12Array;
	-- nbrServos 1 bit enable registers declaration
	signal regEnable : std_logic_vector(31 downto 0);
	-- Status register
	signal regStatus : std_logic;
	-- Command register
	signal regCmd : std_logic_vector(15 downto 0);
	-- Reset active on high level from the RESET_MODULE
	signal reset : std_logic;
	-- Reset request send to the RESET_MODULE
	signal requestReset : std_logic;
	-- Signal buffer for the main counter
	signal count : std_logic_vector(15 downto 0);
-- Architecture definition
begin
 	-- Reset generator Instantiation
 	resetGenerator : RESET_MODULE
 	-- Port mapping for Reset generator
	port map (
		clkIn => clkInImx96MHz,
		resetIn => requestReset,
		resetOut => reset
	);
	-- Counter instantiation
	counterGenerator : COUNTER_MODULE
	-- Port mapping for Counter
	port map (
		clkIn => clkInImx96MHz,
		counterOut16Bits => count
	);
	-- PWM Modules Instantiation
	Pwm : for N in 0 to nbrServos-1 generate
		PWM_MOD : PWM_MODULE
			port map (
				count      => count,
				clkIn => clkInImx96MHz,
				servosReg  => servosReg(N),
				enableReg  => regEnable(N),
				pwmOut     => pwmOut(N));
	end generate Pwm;

	-- Write process in the firmware registers from the µP bus
	registerWrite: process (clkInImx96MHz,reset)
	begin  -- process registerWrite
		-- General Initialisation Process
		-- This initialisation process is here because if it is located in
		-- an other process the synthesis will failed because there are multiple
		-- asignments to the same signals
		-- Asynchronous Reset
		if reset = '1' then
			requestReset <= '0';
			regStatus <= '0';
			regCmd <= X"0000";
			-- All servos are disable
			regEnable <= X"00000000";
			-- Medium position for all servos
			for N in 0 to nbrServos-1 loop
				servosReg(N) <= X"800";
			end loop;
		elsif rising_edge(clkInImx96MHz) then
			-- Write access
			if CSxBI = '0' and WRxBI = '0' then
				-- Addresses decoding, base address is 0x12000000
				case AddrxDI is
					-- MagicID, Firmaware version, regNumberServos, Status
					-- register not processed: read only registers!!!
					when regCmdAddr =>
						-- Save in the regCmd
						regCmd <= DataxD(15 downto 0);
						-- Here: code for command execution...
						-- Software Reset
						if regCmd = X"0001" then
							requestReset <= '1';
						end if;
						if regCmd = X"0000" then
							requestReset <= '0';
						end if;
					-- Servos Validation Register LSB
					when regServosValidationLSBAddr =>
						-- save in regEnable LSB
						regEnable(15 downto 0) <= DataxD(15 downto 0);
					-- Servos Validation Register MSB
					when regServosValidationMSBAddr =>
						-- save in regEnable MSB
						regEnable(31 downto 16) <= DataxD(15 downto 0);
					when others => null;
					-- Add here read error processing
				end case;				
				-- Save position value for all servos
				for N in 0 to nbrServos-1 loop
					-- unigned cast must be used with IEEE.NUMERIC_STD
					if (std_logic_vector(unsigned(addrRegServosPositionDaseAddr) + (N*2))=AddrxDI) then
						-- Save in Servo N Position Register
						servosReg(N) <= DataxD(11 downto 0);
					end if;
				end loop;
			end if;
		end if;
	end process registerWrite;

	-- Read process of firmware registers from the µP bus
	registerRead : process (CSxBI, RDxBI, AddrxDI, clkInImx96MHz, servosReg, regStatus, regCmd, regEnable)
	begin -- process registerRead
		-- Read access (RDxBI=0 et CSxBI=0)
		if CSxBI = '0' and RDxBI = '0' then
			-- Read the general register:
			-- regMagicID,regFirmwareVersion, 	regNumberServos,regStatus
			-- regCmd,regServosValidationLSBAddr,regServosValidationMSBAddr
			case AddrxDI is
				-- Read MagicID
				when regMagicIDAddr =>
					-- Send on the µP data bus the MagicID
					DataxD <= regMagicID;
				-- Read Firmaware version
				when regFirmwareVersionAddr =>
					-- Send on the µP data bus the Firmaware version
					DataxD <= regFirmwareVersion;
				-- Read regNumberServos
				when regNumberServosAddr =>
					-- Send on the µP data bus the number of managed servos
					DataxD <= std_logic_vector(to_unsigned(nbrServos,16));
				-- Read regStatus
				when regStatusAddr =>
					-- Send on the µP data bus the Status
					DataxD <= X"000" & "000" & regStatus;
				-- Read regCmd
				when regCmdAddr =>
					-- Send on the µP data bus the last cms code
					DataxD <= regCmd;
				-- Servos Validation Register LSB
				when regServosValidationLSBAddr =>
					-- Send on the µP data bus regEnable LSB
					DataxD <= regEnable(15 downto 0);
				-- Servos Validation Register MSB
				when regServosValidationMSBAddr =>
					-- Send on the µP data bus regEnable MSB
					DataxD <= regEnable(31 downto 16);
				when others		=>
					DataxD <= (others => '0');
			end case;
			-- Read position value for all servos
			for N in 0 to nbrServos-1 loop
				-- unigned cast must be used with IEEE.NUMERIC_STD
				if (std_logic_vector(unsigned(addrRegServosPositionDaseAddr) + (N*2))=AddrxDI) then
					-- Send on the µP data bus Servo N Position Register
					DataxD <= "0000" & servosReg(N)(11 downto 0);
				end if;
			end loop;
		
		else
			-- hig impedance si acces autre qu'en lecture
			DataxD <= (others => 'Z');
		end if;
	end process registerRead;
end architecture structural;
