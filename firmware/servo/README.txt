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

This project is a FPGA firmware for R/C servos.

A description of such R/C servos can be found at:
http://fribotte.free.fr/bdtech/pic/pic_et_servo.html

It is inspired from the project:
"Atmel AVR 2004 Design Contest Entry Project Number A3722"
of Circuit Cellar by Eric Gagnon :
http://www.circuitcellar.com/avr2004/HA3722.html

This controler is able to drive from 1 to 32 R/C servos. 

The FPGA use is justify because with classical microcontoller outputs, hardware timers
and assembly programming, the strict respect of PWM timing is almost impossible with
numerous R/C servos.
FPGA can build a module with a "true" concurrency in PWM signals generation.
With this controller, there is no R/C servo jittering. For more explanation 
about servo jittering:
http://www.circuitcellar.com/avr2004/wentries/A3722.zip

1 - CREATE A NEW PROJECT IN XILINX ISE:
Xilinx ISE Version: 9.2.04i
Create a new project:
File->New Project
Select Top Level Source: HDL (Harware Description Language)
For the Aramadeus FPGA choose:
Family: Spartan3
Device: XC3S200
Package: TQ144
Speed: -4
Top Level Source: HDL
Synthesis Tool: XST
Simulator: ISE Simulator (VHDL/Verilog)
Preferred Language: VHDL

Import the source files in the project:
Projet->Add Source
Select files:
SERVO_top.vhd
COUNTER.vhd
pwm_module.vhd
servo.ucf

2 - SYNTHESIZE - SERVO_top.bit FIRMWARE FILE CREATION
Generate Programming File->right clic->Run

3 - LOADING OF SERVO_top.bit FIRMWARE FILE IN APF9328
Go to the SERVO_top.bit directory

Establish a RS232 link to APF9328 as describe:
http://www.armadeus.com/wiki/index.php?title=Connection_with_U-Boot_on_Linux

Power up APF9328
At message Hit any key to stop autoboot:
Hit any key to obtain the command prompt:
BIOS> 

Choose the upload memory address:
loadb 08000000

The message:
## Ready for binary (kermit) download to 0x08000000 at 115200 bps...
is printed.

Change to the Kermit command mode:
Ctrl+Altgr+\+c 

File upload:
send SERVO_top.bit

Kermit upload the file.
Hit c to reconnect the terminal.

Firmware Flashing:
run flash_firmware

Loading the firmware in the FPGA:
fpga load 0 ${firmware_addr} ${firmware_len}

For an automatic loading at boot:
setenv firmware_autoload 1
saveenv

The firmware is now usable:

4 - USING FIRMWARE
Actually, only the firmware is written.
R/C servos must be connected to the DevLight ports via a 74HCT244 buffer:

L24N_3/P87 -> servo 00
L24P_3/P86 -> servo 01
L23N_3/P85 -> servo 02
L40P_2/P92 -> servo 03
L40N_3/P90 -> servo 04
L40P_3/P89 -> servo 05
L24N_2/P96 -> servo 06
L24P_2/P95 -> servo 07
L40N_2/P93 -> servo 08
L22P_2/P99 -> servo 09
L23N_2/P98 -> servo 10
L23P_2/P97 -> servo 11
L21N_2/P103 -> servo 12
L21P_2/P102 -> servo 13
L22N_2/P100 -> servo 14
L01P_2/P107 -> servo 15
L20N_2/P105 -> servo 16
L20P_2/P104 -> servo 17
L32N_0/P128 -> servo 18
L32P_0/P127 -> servo 19
L01N_2/P108 -> servo 20
L31P_0/P129 -> servo 21
L31N_0/P130 -> servo 22
L32N_1/P125 -> servo 23
L30P_0/P131 -> servo 24
L30N_0/P132 -> servo 25
L27P_0/P135 -> servo 26
L28N_1/P119 -> servo 27
L28P_1/P118 -> servo 28
IO1/P116 -> servo 29
L01P_1/P112 -> servo 30
L01P_1/P112 -> servo 31

BIOS commands for R/C servos controlling:
At firmware initialisation, all R/C servos are disable.

Servos 0-15 enabling:
mw.w 1200000A FFFF

Servos 16-31 enabling:
mw.w 1200000C FFFF

At firmware initialisation, all R/C servos are in medium position.
Put the servo 0 on the maximal counterclockwise position:
mw.w 12000010 0000
Put the servo 0 on the median position:
mw.w 12000010 0800
Put the servo 0 on the maximal clockwise position:
mw.w 12000010 0FFF

R/C servos addresses:
0x12000010 -> servo 0
0x12000012 -> servo 1
0x12000014 -> servo 2
0x12000016 -> servo 3
0x12000018 -> servo 4
0x1200001A -> servo 5
0x1200001C -> servo 6
0x1200001E -> servo 7
0x12000020 -> servo 8
0x12000022 -> servo 9
0x12000024 -> servo 10
0x12000026 -> servo 11
0x12000028 -> servo 12
0x1200002A -> servo 13
0x1200002C -> servo 14
0x1200002E -> servo 15
0x12000030 -> servo 16
0x12000032 -> servo 17
0x12000034 -> servo 18
0x12000036 -> servo 19
0x12000038 -> servo 20
0x1200003A -> servo 21
0x1200003C -> servo 22
0x1200003E -> servo 23
0x12000040 -> servo 24
0x12000042 -> servo 25
0x12000044 -> servo 26
0x12000046 -> servo 27
0x12000048 -> servo 28
0x1200004A -> servo 29
0x1200004C -> servo 30
0x1200004E -> servo 31

5 - ROAD MAP
Developements in progress:
	- Linux Kernel Driver for this firmware
	- Command Daemon for the Linux Kernel Driver,
	- Network Client for the Command Daemon...
