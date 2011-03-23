/*
 * Eric Jarrige <jorasse@users.sourceforge.net>
 *
 * Configuration settings for the Armadeus Project motherboard 9328 board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * 2005/01/14 Initial version taken from the scb9328 configuration file
 * 2005/03/10 apf9328 configuration file and trial to improve of
 *		hardware register control.
 * 2005/08/16 added APF9328 fpgas and ethernet
 * 2005/12/02 added filesystem boot over NFS
 * 2006/11/26 added filesystem boot over NFS
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_ENV_VERSION 	"4.0"
#define CONFIG_IDENT_STRING	" apf9328 patch 4.0"

#define CONFIG_ARM920T		1	/* this is an ARM920T CPU */
#define CONFIG_IMX		1	/* in a Motorola MC9328MXL Chip */
#define CONFIG_apf9328		1	/* on a Armadeus project board */
#undef	CONFIG_USE_IRQ			/* don't need use IRQ/FIQ */

/*
 * Enable the call to misc_init_r() for miscellaneous platform
 * dependent initialization.
 */

#define CONFIG_MISC_INIT_R


/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/*
 * Select serial console configuration
 */
#define CONFIG_IMX_SERIAL1
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400 }
#define CONFIG_BAUDRATE 	115200

/*
 * Definition of u-boot build in commands. Check out CONFIG_CMD_DFL if
 * neccessary in include/cmd_confdefs.h file. (Un)comment for getting
 * functionality or size of u-boot code.
 */

#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV	/* ask for env variable		*/
#define CONFIG_CMD_BSP		/* Board Specific functions	*/
#undef  CONFIG_CMD_CACHE	/* icache, dcache		*/
#define CONFIG_CMD_CDP		/* Cisco Discovery Protocol	*/
#define CONFIG_CMD_DATE		/* support for RTC, date/time...*/
#define CONFIG_CMD_DHCP		/* DHCP Support			*/
#define CONFIG_CMD_DIAG		/* Diagnostics			*/
#define CONFIG_CMD_EEPROM	/* EEPROM read/write support	*/
#define CONFIG_CMD_FAT		/* FAT support			*/
#define CONFIG_CMD_FLASH	/* flinfo, erase, protect	*/
#define CONFIG_CMD_I2C		/* I2C serial bus support	*/
#define CONFIG_CMD_IMLS		/* List all found images	*/
#define CONFIG_CMD_IMMAP	/* IMMR dump support		*/
#define CONFIG_CMD_JFFS2	/* JFFS2 Support		*/
#define CONFIG_CMD_MII		/* MII support			*/
#undef  CONFIG_CMD_NAND		/* NAND support			*/
#define CONFIG_CMD_PING		/* ping support			*/
#undef  CONFIG_CMD_PORTIO	/* Port I/O			*/
#undef  CONFIG_CMD_REGINFO	/* Register dump		*/
#define CONFIG_CMD_SAVES	/* save S record dump		*/
#define CONFIG_CMD_SETEXPR	/* setexpr support		*/
#define CONFIG_CMD_SNTP		/* SNTP support			*/

#undef CONFIG_DISPLAY_BOARDINFO
#undef CONFIG_DISPLAY_CPUINFO

/*
 * Select some advanced features of the commande line parser
 */
#define CONFIG_AUTO_COMPLETE 	1	/* Enable auto completion of */
					/* commands using TAB */
#define CFG_HUSH_PARSER		1	/* enable the "hush" shell */
#define CFG_PROMPT_HUSH_PS2	"> "	/* secondary prompt string */
#define CONFIG_CMDLINE_EDITING  1

/*
 * Boot options. Setting delay to -1 stops autostart count down.
 */
#define CONFIG_BOOTDELAY	20

#define CONFIG_BOOTARGS \
	CONFIG_CONSOLE " root=/dev/mtdblock4 rootfstype=jffs2 " MTDPARTS_DEFAULT

#define CONFIG_CONSOLE "console=ttySMX0,"MK_STR(CONFIG_BAUDRATE)"n8"
#define CONFIG_MTDMAP  "physmap-flash.0"
#define MTDIDS_DEFAULT	"nor0=" CONFIG_MTDMAP
#define MTDPARTS_DEFAULT "mtdparts=" CONFIG_MTDMAP ":256k(U-boot)ro,"	\
			"128k(U-boot_env),256k(firmware),2048k(kernel),-(rootfs)"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"env_version="		CONFIG_ENV_VERSION		"\0"	\
	"fileaddr="		MK_STR(CFG_LOAD_ADDR)		"\0"	\
	"filesize="		MK_STR(CFG_MONITOR_LEN)		"\0"	\
	"console="		CONFIG_CONSOLE			"\0"	\
	"mtdparts="		MTDPARTS_DEFAULT		"\0"	\
	"partition=nor0,3\0"						\
	"uboot_addr="		MK_STR(CFG_MONITOR_BASE)	"\0"	\
	"uboot_len="		MK_STR(CFG_MONITOR_LEN)		"\0"	\
	"env_addr="		MK_STR(CFG_ENV_ADDR)		"\0"	\
	"env_len="		MK_STR(CFG_ENV_SIZE)		"\0"	\
	"firmware_addr="	MK_STR(CONFIG_FIRMWARE_ADDR)	"\0"	\
	"firmware_len="	MK_STR(CONFIG_FIRMWARE_LEN)	"\0"	\
	"kernel_addr="		MK_STR(CONFIG_KERNEL_ADDR)	"\0"	\
	"kernel_len="		MK_STR(CONFIG_KERNEL_LEN)	"\0"	\
	"rootfs_addr="		MK_STR(CONFIG_ROOTFS_ADDR)	"\0"	\
	"rootfs_len="		MK_STR(CONFIG_ROOTFS_LEN)	"\0"	\
	"board_name="		MK_STR(CONFIG_BOARD_NAME)	"\0"	\
	"serverpath="		MK_STR(CONFIG_SRV_PATH)		"\0"	\
	"ntpserverip=217.147.208.1\0"					\
	"mmcroot=/dev/mmcblk0p1\0"					\
	"mmcrootfstype=ext2\0"						\
	"addnfsargs=setenv bootargs ${bootargs} "			\
		"root=/dev/nfs rw nfsroot=${serverip}:${rootpath}\0"    \
	"addjffsargs=setenv bootargs ${bootargs} "			\
		"root=/dev/mtdblock4 rootfstype=jffs2\0"   		\
	"addubifsargs=setenv bootargs ${bootargs} "			\
		"ubi.mtd=rootfs root=ubi0:rootfs rootfstype=ubifs\0"    \
	"addmmcargs=setenv bootargs ${bootargs} "			\
		"root=${mmcroot} rootfstype=${mmcrootfstype}\0"   	\
	"addipargs=setenv bootargs ${bootargs} "			\
		"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}::off \0"\
	"nfsboot=setenv bootargs ${console} ${mtdparts};"		\
		"run addnfsargs addipargs; bootm ${kernel_addr}\0"	\
	"jffsboot=setenv bootargs ${console} ${mtdparts};"		\
		"run addjffsargs addipargs; bootm ${kernel_addr}\0"	\
	"ubifsboot=setenv bootargs ${console} ${mtdparts};"		\
		"run addubifsargs addipargs; bootm ${kernel_addr}\0"	\
	"mmcboot=setenv bootargs ${console} ${mtdparts};"		\
		"run addmmcargs addipargs; bootm ${kernel_addr}\0"	\
	"firmware_autoload=0\0"					\
	"flash_uboot=protect off ${uboot_addr} +${uboot_len};"		\
		"echo Erasing FLASH;"					\
		"era ${uboot_addr} +${uboot_len};"			\
		"if cp.b ${fileaddr} ${uboot_addr} ${filesize};"	\
			"then protect on ${uboot_addr} +${uboot_len};"	\
				"echo Flashing of uboot succeed;"	\
			"else echo Flashing of uboot failed;"		\
		"fi; \0"						\
	"flash_firmware=echo Erasing FLASH;"				\
		"era ${firmware_addr} +${firmware_len};"		\
		"if cp.b ${fileaddr} ${firmware_addr} ${filesize} ;"	\
			"then echo Flashing of Firmware succeed;"	\
			"else echo Flashing of Firmware failed;"	\
		"fi\0"							\
	"flash_kernel=echo Erasing FLASH;"				\
		"era ${kernel_addr} +${kernel_len};"			\
		"if cp.b ${fileaddr} ${kernel_addr} ${filesize} ;"	\
			"then echo Flashing of kernel succeed;"		\
			"else echo Flashing of kernel failed;"		\
		"fi\0"							\
	"flash_rootfs=echo Erasing FLASH;"				\
		"era ${rootfs_addr} +${rootfs_len};"			\
		"if cp.b ${fileaddr} ${rootfs_addr} ${filesize};"	\
			"then echo Flashing of rootfs succeed;"		\
			"else echo Flashing of rootfs failed;"		\
		"fi\0"							\
	"flash_reset_env=protect off ${env_addr} +${env_len};"		\
		"era ${env_addr} +${env_len};"				\
		"echo Erasing of flash environment variables done!\0"	\
	"download_uboot=tftpboot ${loadaddr} "				\
		" ${serverpath}${board_name}-u-boot.bin\0"		\
	"download_kernel=tftpboot ${loadaddr} "				\
		" ${serverpath}${board_name}-linux.bin\0"		\
	"download_rootfs=tftpboot ${loadaddr} "				\
		" ${serverpath}${board_name}-rootfs.jffs2\0"	\
	"update_uboot=run download_uboot flash_uboot\0"			\
	"update_kernel=run download_kernel flash_kernel\0"		\
	"update_rootfs=run download_rootfs flash_rootfs\0"		\
	"update_all=run download_kernel flash_kernel download_rootfs "	\
		"flash_rootfs download_uboot flash_uboot\0"		\
	"unlock_regs=mw 00200008 0; mw 00210008 0\0"			\

#define CONFIG_BOOTCOMMAND	"run jffsboot"
#define CFG_AUTOLOAD		"no"

#define CONFIG_MACH_TYPE MACH_TYPE_APF9328
#define CONFIG_BOOT_PARAMS_ADDR	0x08000100

#undef CONFIG_SHOW_BOOT_PROGRESS

#define CONFIG_ETHADDR
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.000.10
#define CONFIG_BOARD_NAME	"apf9328"
#define CONFIG_HOSTNAME		"apf9328"
#define CONFIG_GATEWAYIP	192.168.000.1
#define CONFIG_SERVERIP		192.168.000.2
#define CONFIG_SRV_PATH		""
#define CONFIG_ROOTPATH		"/tftpboot/apf9328-root"

/*
 * General options for u-boot. Modify to save memory foot print
 */

#define CFG_LONGHELP
#define CFG_PROMPT		"BIOS> "	/* prompt string      */
#define CFG_CBSIZE		256		/* console I/O buffer */
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)	/* print buffer size  */
#define CFG_MAXARGS		16		/* max command args   */
#define CFG_BARGSIZE		CFG_CBSIZE	/* boot args buf size */

#define CFG_MEMTEST_START	0x08000000	/* memtest test area  */
#define CFG_MEMTEST_END		0x08700000

#undef	CFG_CLKS_IN_HZ				/* use HZ for freq. display  */
#define CFG_HZ			1000000		/* incrementer freq: 1 MHz   */

/*
 * Definitions related to passing arguments to kernel.
 */

#define CONFIG_CMDLINE_TAG	1	/* send commandline to Kernel        */
#define CONFIG_SETUP_MEMORY_TAGS 1	/* send memory definition to kernel */
#define CONFIG_INITRD_TAG	1	/* send initrd params                */
#undef	CONFIG_VFD			/* do not send framebuffer setup    */

/*
 * Malloc pool need to host env + 128 Kb reserve for other allocations.
 */
#define CFG_MALLOC_LEN		(CFG_ENV_SIZE + (128<<10) )

#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

#define CONFIG_STACKSIZE	(120<<10)	/* stack size                 */

#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4<<10)		/* IRQ stack                  */
#define CONFIG_STACKSIZE_FIQ	(4<<10)		/* FIQ stack                  */
#endif

/*
* Clocks configuration
*/
/*
 * PLL configuration

   f_{dpll}=2*f{ref}*(MFI+MFN/(MFD+1))/(PD+1)
   f_ref=16,777216MHz
   32768 Hz xtal
   0x07B32DA5: 192.0000173
   0x002a141f: 191,9944MHz
   0x040b2007: 144MHz
   0x0FB32DA5: 96.00000864 MHz
   0x042a141f: 96MHz
   0x0811140d: 64MHz
   0x040e200e: 150MHz
   0x00321431: 200MHz

  16 MHz xtal 
   0x08001800: 64MHz mit 16er Quarz
   0x04001800: 96MHz mit 16er Quarz
   0x04002400: 144MHz mit 16er Quarz

   31 |x x x x|x x x x|x x x x|x x x x|x x x x|x x x x|x x x x|x x x x| 0
      |XXX|--PD---|-------MFD---------|XXX|--MFI--|-----MFN-----------|
 */

#define CFG_OSC32	32768	/* 32768 or 32000 Hz crystal */
#undef  CFG_OSC16		/* there is no external 16MHz external clock */

/* MPU CLOCK source before PLL  (should be named CFG_SYS_CLK_FREQ) */
#define CONFIG_SYS_CLK_FREQ	(512*CFG_OSC32)
#define CFG_MPCTL0_VAL		0x07B32DA5	/* 192.000017 MHz */
#define CFG_MPCTL1_VAL		0

/* system clock source before PLL (should be named CFG_SYSPLL_CLK_FREQ) */
#ifndef CFG_OSC16
#define CONFIG_SYSPLL_CLK_FREQ	(512*CFG_OSC32)
#if (CFG_OSC32 == 32000)
#define CFG_SPCTL0_VAL		0x043F1437	/* 96 MHz */
#define CFG_SPCTL1_VAL		0
#define CONFIG_SYS_FREQ		96		/* MHz */
#else /* CFG_OSC32 == 32768*/
#define CFG_SPCTL0_VAL		0x0FB32DA5	/* 96.000009 MHz */
#define CFG_SPCTL1_VAL		0
#define CONFIG_SYS_FREQ		96		/* MHz */
#endif /* CFG_OSC32 */
#else /* CFG_OSC16 in use*/
#define CONFIG_SYSPLL_CLK_FREQ	CFG_OSC16
#define CFG_SPCTL0_VAL		0x04001401	/* 96 MHz */
#define CFG_SPCTL1_VAL		0x0C000040
#define CONFIG_SYS_FREQ		96		/* MHz */
#endif /* CFG_OSC16 */

/* external bus frequency (have to be a CONFIG_SYS_FREQ ratio) */
#define CONFIG_SYS_BUS_FREQ 	96	/* 96|48... MHz (BCLOCK and HCLOCK) */
#define CONFIG_USB_FREQ		48	/* 48 MHz */
#define CONFIG_PERIF1_FREQ	16	/* 16 MHz UART, Timer PWM*/
#define CONFIG_PERIF2_FREQ 	48	/* 48 MHz LCD SD SPI*/
#define CONFIG_PERIF3_FREQ	16	/* 16 MHz SSI*/


/*
 * SDRAM definition parameter
 */
#define CONFIG_NR_DRAM_BANKS	1	/* we have and support only 1 bank of SDRAM */

#define CFG_SDRAM_MBYTE_SYZE 16

#if (CFG_SDRAM_MBYTE_SYZE == 8)
/* micron 8MB */
#define CFG_SDRAM_1_BASE	0x08000000 /* SDRAM bank #1 32bits         */
#define CFG_SDRAM_1_SIZE	0x00800000 /* 8 MB                */
#define CFG_SDRAM_NUM_COL		8  /* 8, 9, 10 or 11 column address bits */
#define CFG_SDRAM_NUM_ROW		11 /* 11, 12 or 13 row address bits */
#define CFG_SDRAM_REFRESH		2  /* 0=OFF 1=2048 2=4096 3=8192 refresh */
#define CFG_SDRAM_CLOCK_CYCLE_CL_1	20 /* ns clock cycle time when CL=1 */
#define CFG_SDRAM_ROW_PRECHARGE_DELAY	20 /* ns SRP */
#define CFG_SDRAM_ROW_2_COL_DELAY	20 /* ns SRCD */
#define CFG_SDRAM_ROW_CYCLE_DELAY	70 /* ns SRC */
#define CFG_SDRAM_BURST_LENGTH		3  /* 2^N BYTES (N=0..3) */
#define CFG_SDRAM_SINGLE_ACCESS		0  /* 1= single access; 0 = Burst mode */
#endif

#if (CFG_SDRAM_MBYTE_SYZE == 16)
/* micron 16MB */
#define CFG_SDRAM_1_BASE	0x08000000 /* SDRAM bank #1 32bits    */
#define CFG_SDRAM_1_SIZE	0x01000000 /* 16 MB                   */
#define CFG_SDRAM_NUM_COL		8  /* 8, 9, 10 or 11 column address bits */
#define CFG_SDRAM_NUM_ROW		12 /* 11, 12 or 13 row address bits */
#define CFG_SDRAM_REFRESH		2  /* 0=OFF 1=2048 2=4096 3=8192 refresh */
#define CFG_SDRAM_CLOCK_CYCLE_CL_1	20 /* ns clock cycle time when CL=1 */
#define CFG_SDRAM_ROW_PRECHARGE_DELAY	20 /* ns SRP */
#define CFG_SDRAM_ROW_2_COL_DELAY	20 /* ns SRCD */
#define CFG_SDRAM_ROW_CYCLE_DELAY	70 /* ns SRC */
#define CFG_SDRAM_BURST_LENGTH		3  /* 2^N BYTES (N=0..3) */
#define CFG_SDRAM_SINGLE_ACCESS		0  /* 1= single access; 0 = Burst mode */
#endif

#if (CFG_SDRAM_MBYTE_SYZE == 32)
/* micron 32MB */
#define CFG_SDRAM_1_BASE	0x08000000 /* SDRAM bank #1 32bits    */
#define CFG_SDRAM_1_SIZE	0x02000000 /* 32 MB                   */
#define CFG_SDRAM_NUM_COL		9  /* 8, 9, 10 or 11 column address bits */
#define CFG_SDRAM_NUM_ROW		12 /* 11, 12 or 13 row address bits */
#define CFG_SDRAM_REFRESH		2  /* 0=OFF 1=2048 2=4096 3=8192 refresh */
#define CFG_SDRAM_CLOCK_CYCLE_CL_1	20 /* ns clock cycle time when CL=1 */
#define CFG_SDRAM_ROW_PRECHARGE_DELAY	20 /* ns SRP */
#define CFG_SDRAM_ROW_2_COL_DELAY	20 /* ns SRCD */
#define CFG_SDRAM_ROW_CYCLE_DELAY	70 /* ns SRC */
#define CFG_SDRAM_BURST_LENGTH		3  /* 2^N BYTES (N=0..3) */
#define CFG_SDRAM_SINGLE_ACCESS		0  /* 1= single access; 0 = Burst mode */
#endif

#if (CFG_SDRAM_MBYTE_SYZE == 64)
/* micron 64MB */
#define CFG_SDRAM_1_BASE	0x08000000 /* SDRAM bank #1 32bits    */
#define CFG_SDRAM_1_SIZE	0x04000000 /* 64 MB                   */
#define CFG_SDRAM_NUM_COL		9  /* 8, 9, 10 or 11 column address bits */
#define CFG_SDRAM_NUM_ROW		13 /* 11, 12 or 13 row address bits */
#define CFG_SDRAM_REFRESH		3  /* 0=OFF 1=2048 2=4096 3=8192 refresh */
#define CFG_SDRAM_CLOCK_CYCLE_CL_1	20 /* ns clock cycle time when CL=1 */
#define CFG_SDRAM_ROW_PRECHARGE_DELAY	20 /* ns SRP */
#define CFG_SDRAM_ROW_2_COL_DELAY	20 /* ns SRCD */
#define CFG_SDRAM_ROW_CYCLE_DELAY	70 /* ns SRC */
#define CFG_SDRAM_BURST_LENGTH		3  /* 2^N BYTES (N=0..3) */
#define CFG_SDRAM_SINGLE_ACCESS		0  /* 1= single access; 0 = Burst mode */
#endif

/*
 * Configuration for a maximum 32MB of FLASH memory
 */
#define CFG_MAX_FLASH_BANKS	1	/* FLASH banks count (not chip count) */
#define CFG_MAX_FLASH_SECT	256	/* number of sector in FLASH bank     */
#define CFG_FLASH_BASE		0x10000000 /* location of flash memory        */

/* This should be defined if CFI FLASH device is present. */
#define	CFG_FLASH_CFI			1
#define CFG_FLASH_CFI_DRIVER		1
#define CFG_FLASH_USE_BUFFER_WRITE 	1 /* use buffered writes (20x faster) */
#define CFG_FLASH_PROTECTION		1

/*
 * This is setting for JFFS2 support in u-boot.
 * NOTE: Enable CFG_CMD_JFFS2 for JFFS2 support.
 */
#define CFG_JFFS2_FIRST_BANK		0
#define CFG_JFFS2_FIRST_SECTOR		5
#define CFG_JFFS2_NUM_BANKS		1
/*
 * Environment setup. Definitions of monitor location and size with
 * definition of environment setup ends up in 2 possibilities.
 * 1. Embeded environment - in u-boot code is space for environment
 * 2. Environment is read from predefined sector of flash
 * Right now we support 2. possiblity, but expecting no env placed
 * on mentioned address right now. This also needs to provide whole
 * sector for it - for us 256Kb is really waste of memory. U-boot uses
 * default env. and until kernel parameters could be sent to kernel
 * env. has no sense to us.
 */

#define	CFG_MONITOR_BASE	0x10000000
#define	CFG_MONITOR_LEN		0x00040000	/* 256kB ( 2 flash sector )  */
#define	CFG_ENV_IS_IN_FLASH	1
#define	CFG_ENV_ADDR		0x10040000	/* absolute address for now  */
#define	CFG_ENV_SIZE		0x00020000
#define	CFG_ENV_SECT_SIZE	0x00020000
#define	CONFIG_ENV_OVERWRITE	1		/* env is writable now   */
#define	CONFIG_FIRMWARE_ADDR	0x10060000
#define	CONFIG_FIRMWARE_LEN	0x00040000	/* 256kB ( 2 flash sector )  */
#define	CONFIG_KERNEL_ADDR	0x100A0000
#define	CONFIG_KERNEL_LEN	0x00200000	/* 2MB */
#define	CONFIG_ROOTFS_ADDR	0x102A0000
#define	CONFIG_ROOTFS_LEN			/* entire flash if not specified */

/*
 * Default load address for user programs and kernel
 */
#define CFG_LOAD_ADDR		0x08000000
#define	CONFIG_LOADADDR		0x08000000
#define CFG_TFTP_LOADADDR	0x08000000
/*
 * Hardware drivers
 */

/* DM9000 ethernet chipset assigned to CS4 */
/* For the apf board DM9000 base is 0x15C00000 and data is base+2	*/
/* For the apm board DM9000 base is 0x15C3FFFC and data is base+4	*/
#define CONFIG_DRIVER_DM9000	1
#undef  CONFIG_DM9000_DEBUG
#define CONFIG_DM9000_BASE    0x15C00000
#define DM9000_IO		CONFIG_DM9000_BASE
#define DM9000_DATA		(CONFIG_DM9000_BASE+2)
#define CONFIG_DM9000_USE_16BIT

/*-----------------------------------------------------------------------
 * FPGA stuff
 */
#define CONFIG_FPGA_COUNT	1
/* only digital value for CONFIG_FPGA is supported (investigation pending)    */
#define CONFIG_FPGA		0x801		/* CFG_XILINX_SPARTAN3 */
#define CONFIG_FPGA_XILINX
#define CONFIG_FPGA_SPARTAN3
#define CFG_FPGA_WAIT		250
#define CFG_FPGA_PROG_FEEDBACK
#define CFG_FPGA_CHECK_CTRLC
#define CFG_FPGA_CHECK_ERROR

/* FPGA program pin configuration */
#define CFG_FPGA_PRG	(GPIO_PORTB | 18)	/* FPGA prog pin (SSI output) */
#define CFG_FPGA_CLK	(GPIO_PORTB | 19)	/* FPGA clk pin  (SSI output) */
#define CFG_FPGA_DATA	(GPIO_PORTB | 17)	/* FPGA data pin (SSI output) */
#define CFG_FPGA_INIT	(GPIO_PORTB | 15)	/* FPGA init pin (SSI input)  */
#define CFG_FPGA_DONE	(GPIO_PORTB | 16)	/* FPGA done pin (SSI input)  */

/*
 * I2C bus
 */

#define	CONFIG_HARD_I2C		1	/* I2C with hardware support    */

#define CFG_I2C_SPEED		400000	/* 400 kHz */
#define CFG_I2C_SLAVE		0x7F

# define CFG_I2C_EEPROM_ADDR	0x50	/* EEPROM 24LC256               */
# define CFG_I2C_EEPROM_ADDR_LEN 2	/* bytes of address             */
/* mask of address bits that overflow into the "EEPROM chip address"    */
/*#define CFG_I2C_EEPROM_ADDR_OVERFLOW	0x00*/
#define CFG_EEPROM_PAGE_WRITE_BITS	6
#define CFG_EEPROM_PAGE_WRITE_DELAY_MS	10	/* takes up to 10 msec */

#define CONFIG_RTC_DS1374
/*#define CONFIG_MMC		1
*/
/*
 * External interfaces module
 *
 * CSxU_VAL:
 * 63|    x    |x|x x|x x x x|x x| x | x  |x x x x|48
 *   |DTACK_SEL|0|BCD|  BCS  |PSZ|PME|SYNC|  DOL  |
 *
 * 47| x x  | x x x x x x | x | x x x x | x x x x |32
 *   | CNC  |     WSC     | 0 |   WWS   |   EDC   |
 *
 * CSxL_VAL:
 * 31|  x x x x  | x x x x  | x x x x  | x x x x  |24
 *   |    OEA    |   OEN    |   WEA    |   WEN    |
 * 23|x x x x| x | x x x | x x  x x |x x| x |  x  | 0
 *   |  CSA  |EBC|  DSZ  | 0|SP|0|WP|0 0|PA |CSEN |
 */

/* CS0 configuration for  flash memory Micron MT28F128J3-150 */
#define CFG_CS0_CHIP_SELECT_ENABLE	1	/* 1 : enable CS0 peripherals */
#define CFG_CS0_DATA_PORT_SIZE		0x5	/* 5=16 bits on D[15:0] pins  */
						/* 3=8bits on D[7:0] 6=32 bits..*/
#define CFG_CS0_SUPERVISOR_PROTECT	0   /* 1 : user mode access prohibited  */
#define CFG_CS0_WRITE_PROTECT		0   /* 1 : write access prohibited  */
#define CFG_CS0_EB_SIGNAL_CONTROL_WRITE	1   /* 1 when EB is used as write signal */
#define CFG_CS0_READ_CYC_LGTH		150	/* ns */
#define CFG_CS0_OE_ASSERT_DLY		0	/* ns */
#define CFG_CS0_OE_NEG_DLY		0	/* ns */

#define CFG_CS0_CS_NEG_LGTH 	0	/* ns CS HIGH to CS LOW : tCWH */
#define CFG_CS0_XTRA_DEAD_CYC	35	/* ns from CS HIGH to tristate bus */

#define CFG_CS0_WRITE_XTRA_LGTH		0	/* ns */
#define CFG_CS0_EB_ASSERT_DLY		0	/* ns */
#define CFG_CS0_EB_NEG_DLY		0	/* ns */
#define CFG_CS0_CS_ASSERT_NEG_DLY	0	/* ns */

#define CFG_CS0_SYNC_ENABLE		1	/* enable synchronous burst mode */
#define CFG_CS0_SYNC_PAGE_MODE_EMUL	1	/* enable page mode emulation */
#define CFG_CS0_SYNC_PAGE_SIZE		0	/* 4 words page size (8bytes) */
#define CFG_CS0_SYNC_BURST_CLK_START	0	/* 0 ns burst clock delay */
#define CFG_CS0_SYNC_BURST_CLK_DIV	0	/* 0 : divider is 1 */
#define CFG_CS0_SYNC_DAT_OUT_LGTH	25	/* ns */

/* CS1 configuration for  */
#define CFG_CS1_CHIP_SELECT_ENABLE	1	/* 1 : enable CS0 peripherals  */
#define CFG_CS1_PIN_ASSERT		0	/* chip select pin state when */
						/* chip select disabled  */
#define CFG_CS1_DATA_PORT_SIZE		0x5	/* 5=16 bits on D[15:0] pins*/
						/* 3=8bits on D[7:0] 6=32 bits..*/ 
#define CFG_CS1_SUPERVISOR_PROTECT	0	/* 1 : user mode access prohibited  */
#define CFG_CS1_WRITE_PROTECT		0	/* 1 : write access prohibited  */
#define CFG_CS1_EB_SIGNAL_CONTROL_WRITE	1	/* 1 when EB is used as write signal */

#define CFG_CS1_READ_CYC_LGTH		50	/* ns */
#define CFG_CS1_OE_ASSERT_DLY		0	/* ns */
#define CFG_CS1_OE_NEG_DLY		0	/* ns */

#define CFG_CS1_CS_NEG_LGTH 	0	/* max 30 ns CS HIGH to CS LOW at 100MHz */
#define CFG_CS1_XTRA_DEAD_CYC	0	/* ns from CS HIGH to tristate bus */

#define CFG_CS1_WRITE_XTRA_LGTH		0	/* ns */
#define CFG_CS1_EB_ASSERT_DLY		0	/* ns */
#define CFG_CS1_EB_NEG_DLY		0	/* ns */
#define CFG_CS1_CS_ASSERT_NEG_DLY	0	/* ns */

#define CFG_CS1_SYNC_ENABLE		0	/* enable synchronous burst mode */
#define CFG_CS1_SYNC_PAGE_MODE_EMUL	0	/* enable page mode emulation */
#define CFG_CS1_SYNC_PAGE_SIZE		0	/* 4 words page size (8bytes) */
#define CFG_CS1_SYNC_BURST_CLK_START	0	/* 0 ns burst clock delay */
#define CFG_CS1_SYNC_BURST_CLK_DIV	0	/* 0 : divider is 1 */
#define CFG_CS1_SYNC_DAT_OUT_LGTH	0	/* ns */

/* CS2 configuration for  */
#define CFG_CS2_CHIP_SELECT_ENABLE	0	/* 1 : enable CS0 peripherals  */
#define CFG_CS2_PIN_ASSERT		0	/* chip select pin state when */
						/*chip select disabled  */
#define CFG_CS2_DATA_PORT_SIZE		0x5	/* 5=16 bits on D[15:0] pins */
						/* 3=8bits on D[7:0] 6=32 bits..*/
#define CFG_CS2_SUPERVISOR_PROTECT	0	/* 1 : user mode access prohibited  */
#define CFG_CS2_WRITE_PROTECT		0	/* 1 : write access prohibited  */
#define CFG_CS2_EB_SIGNAL_CONTROL_WRITE	1	/* 1 when EB is used as write signal */

#define CFG_CS2_READ_CYC_LGTH		0	/* ns */
#define CFG_CS2_OE_ASSERT_DLY		0	/* ns */
#define CFG_CS2_OE_NEG_DLY		0	/* ns */

#define CFG_CS2_CS_NEG_LGTH 	0	/* max 30 ns CS HIGH to CS LOW at 100MHz */
#define CFG_CS2_XTRA_DEAD_CYC	0	/* ns from CS HIGH to tristate bus */

#define CFG_CS2_WRITE_XTRA_LGTH		0	/* ns */
#define CFG_CS2_EB_ASSERT_DLY		0	/* ns */
#define CFG_CS2_EB_NEG_DLY		0	/* ns */
#define CFG_CS2_CS_ASSERT_NEG_DLY	0	/* ns */

#define CFG_CS2_SYNC_ENABLE		0	/* enable synchronous burst mode */
#define CFG_CS2_SYNC_PAGE_MODE_EMUL	0	/* enable page mode emulation*/
#define CFG_CS2_SYNC_PAGE_SIZE		0	/* 4 words page size (8bytes) */
#define CFG_CS2_SYNC_BURST_CLK_START	0	/* 0 ns burst clock delay */
#define CFG_CS2_SYNC_BURST_CLK_DIV	0	/* 0 : divider is 1 */
#define CFG_CS2_SYNC_DAT_OUT_LGTH	0	/* ns */

/* CS3 configuration for ISP1760 */
#define CFG_CS3_CHIP_SELECT_ENABLE	1	/* 1 : enable CS0 peripherals  */
#define CFG_CS3_PIN_ASSERT		0	/* chip select pin state when */
						/* chip select disabled  */
#define CFG_CS3_DATA_PORT_SIZE		0x5	/* 5=16 bits on D[15:0] pins */
						/* 3=8bits on D[7:0] 6=32 bits..*/
#define CFG_CS3_SUPERVISOR_PROTECT	0	/* 1: user mode access prohibited*/
#define CFG_CS3_WRITE_PROTECT		0	/* 1: write access prohibited  */
#define CFG_CS3_EB_SIGNAL_CONTROL_WRITE	1	/* 1 when EB is used as write signal*/

#define CFG_CS3_READ_CYC_LGTH		110	/* ns */
#define CFG_CS3_OE_ASSERT_DLY		45	/* ns */
#define CFG_CS3_OE_NEG_DLY		40	/* ns */

#define CFG_CS3_CS_NEG_LGTH 	30	/* max 30 ns CS HIGH to CS LOW at 100MHz */
#define CFG_CS3_XTRA_DEAD_CYC	0	/* ns from CS HIGH to tristate bus */

#define CFG_CS3_WRITE_XTRA_LGTH		0	/* ns */
#define CFG_CS3_EB_ASSERT_DLY		45	/* ns */
#define CFG_CS3_EB_NEG_DLY		45	/* ns */
#define CFG_CS3_CS_ASSERT_NEG_DLY	35	/* ns */

#define CFG_CS3_SYNC_ENABLE		0	/* enable synchronous burst mode*/
#define CFG_CS3_SYNC_PAGE_MODE_EMUL	0	/* enable page mode emulation*/
#define CFG_CS3_SYNC_PAGE_SIZE		0	/* 4 words page size (8bytes) */
#define CFG_CS3_SYNC_BURST_CLK_START	0	/* 0 ns burst clock delay */
#define CFG_CS3_SYNC_BURST_CLK_DIV	0	/* 0 : divider is 1 */
#define CFG_CS3_SYNC_DAT_OUT_LGTH	0	/* ns */

/* CS4 configuration for DM9000 ethernet chipset */
#define CFG_CS4_CHIP_SELECT_ENABLE	1	/* 1 : enable CS0 peripherals  */
#define CFG_CS4_PIN_ASSERT		0	/* chip select pin state when */
						/* chip select disabled  */
#define CFG_CS4_DATA_PORT_SIZE		0x5	/* 5=16 bits on D[15:0] pins */
						/* 3=8bits on D[7:0] 6=32 bits..*/
#define CFG_CS4_SUPERVISOR_PROTECT	0	/* 1 : user mode access prohibited*/
#define CFG_CS4_WRITE_PROTECT		0	/* 1 : write access prohibited  */
#define CFG_CS4_EB_SIGNAL_CONTROL_WRITE	1	/* 1 when EB is used as write signal*/

#define CFG_CS4_READ_CYC_LGTH		72	/* ns */
#define CFG_CS4_OE_ASSERT_DLY		5	/* ns */
#define CFG_CS4_OE_NEG_DLY		45	/* ns */

#define CFG_CS4_CS_NEG_LGTH 	30	/* max 30 ns CS HIGH to CS LOW at 100MHz*/
#define CFG_CS4_XTRA_DEAD_CYC	0	/* ns from CS HIGH to tristate bus */

#define CFG_CS4_WRITE_XTRA_LGTH		0	/* ns */
#define CFG_CS4_EB_ASSERT_DLY		5	/* ns */
#define CFG_CS4_EB_NEG_DLY		45	/* ns */
#define CFG_CS4_CS_ASSERT_NEG_DLY	0	/* ns */

#define CFG_CS4_SYNC_ENABLE		0	/* enable synchronous burst mode*/
#define CFG_CS4_SYNC_PAGE_MODE_EMUL	0	/* enable page mode emulation */
#define CFG_CS4_SYNC_PAGE_SIZE		0	/* 4 words page size (8bytes) */
#define CFG_CS4_SYNC_BURST_CLK_START	0	/* 0 ns burst clock delay */
#define CFG_CS4_SYNC_BURST_CLK_DIV	0	/* 0 : divider is 1 */
#define CFG_CS4_SYNC_DAT_OUT_LGTH	0	/* ns */

/* CS5 configuration for  */
#define CFG_CS5_CHIP_SELECT_ENABLE	1	/* 1 : enable CS0 peripherals */
#define CFG_CS5_PIN_ASSERT		0	/* chip select pin state when */
						/* chip select disabled  */
#define CFG_CS5_DATA_PORT_SIZE		0x5	/* 5=16 bits on D[15:0] pins */
						/* 3=8bits on D[7:0] 6=32 bits..*/
#define CFG_CS5_SUPERVISOR_PROTECT	0	/* 1: user mode access prohibited*/
#define CFG_CS5_WRITE_PROTECT		0	/* 1 : write access prohibited*/
#define CFG_CS5_EB_SIGNAL_CONTROL_WRITE	1	/* 1 when EB is used as write signal*/
#define CFG_CS5_DTACK_SELECT		1	/* Activate DTACK fonction */
						/* (asynchrone bus) */

#define CFG_CS5_READ_CYC_LGTH		-1	/* ns or -1 to DTACK function */
#define CFG_CS5_OE_ASSERT_DLY		10	/* ns */
#define CFG_CS5_OE_NEG_DLY		10	/* ns */

#define CFG_CS5_CS_NEG_LGTH 	10	/* max 30 ns CS HIGH to CS LOW at 100MHz*/
#define CFG_CS5_XTRA_DEAD_CYC	0	/* ns from CS HIGH to tristate bus */

#define CFG_CS5_WRITE_XTRA_LGTH		0	/* ns */
#define CFG_CS5_EB_ASSERT_DLY		10	/* ns */
#define CFG_CS5_EB_NEG_DLY		10	/* ns */
#define CFG_CS5_CS_ASSERT_NEG_DLY	0	/* ns */

#define CFG_CS5_SYNC_ENABLE		0	/* enable synchron burst mode */
#define CFG_CS5_SYNC_PAGE_MODE_EMUL	0	/* enable page mode emulation */
#define CFG_CS5_SYNC_PAGE_SIZE		0	/* 4 words page size (8bytes) */
#define CFG_CS5_SYNC_BURST_CLK_START	0	/* 0 ns burst clock delay */
#define CFG_CS5_SYNC_BURST_CLK_DIV	0	/* 0 : divider is 1 */
#define CFG_CS5_SYNC_DAT_OUT_LGTH	0	/* ns */

/*
 * Default configuration for GPIOs and peripherals
 */
#define CFG_DDIR_A_VAL		0x00000000
#define CFG_OCR1_A_VAL		0x00000000
#define CFG_OCR2_A_VAL		0x00000000
#define CFG_ICFA1_A_VAL		0xFFFFFFFF
#define CFG_ICFA2_A_VAL		0xFFFFFFFF
#define CFG_ICFB1_A_VAL		0xFFFFFFFF
#define CFG_ICFB2_A_VAL		0xFFFFFFFF
#define CFG_DR_A_VAL		0x00000000

/* Setup for PA23 which is Reset Default PA23 but has to become
   CS5 */
#define CFG_GIUS_A_VAL		0x00007FF8
#define CFG_ICR1_A_VAL		0x00000000
#define CFG_ICR2_A_VAL		0x00000000
#define CFG_IMR_A_VAL		0x00000000
#define CFG_GPR_A_VAL		0x00000000
#define CFG_PUEN_A_VAL		0xFFFFFFFF

#define CFG_DDIR_B_VAL		0x00000000
#define CFG_OCR1_B_VAL		0x00000000
#define CFG_OCR2_B_VAL		0x00000000
#define CFG_ICFA1_B_VAL		0xFFFFFFFF
#define CFG_ICFA2_B_VAL		0xFFFFFFFF
#define CFG_ICFB1_B_VAL		0xFFFFFFFF
#define CFG_ICFB2_B_VAL		0xFFFFFFFF
#define CFG_DR_B_VAL		0x00000000
#define CFG_GIUS_B_VAL		0xFFFFFFFF
#define CFG_ICR1_B_VAL		0x00000000
#define CFG_ICR2_B_VAL		0x00000000
#define CFG_IMR_B_VAL		0x00000000
#define CFG_GPR_B_VAL		0x00000000
#define CFG_PUEN_B_VAL		0xFFFFFFFF

#define CFG_DDIR_C_VAL		0x00000000
#define CFG_OCR1_C_VAL		0x00000000
#define CFG_OCR2_C_VAL		0x00000000
#define CFG_ICFA1_C_VAL		0xFFFFFFFF
#define CFG_ICFA2_C_VAL		0xFFFFFFFF
#define CFG_ICFB1_C_VAL		0xFFFFFFFF
#define CFG_ICFB2_C_VAL		0xFFFFFFFF
#define CFG_DR_C_VAL		0x00000000
#define CFG_GIUS_C_VAL		0x0007FFFF
#define CFG_ICR1_C_VAL		0x00000000
#define CFG_ICR2_C_VAL		0x00000000
#define CFG_IMR_C_VAL		0x00000000
#define CFG_GPR_C_VAL		0x00000000
#define CFG_PUEN_C_VAL		0xF913FFFF

#define CFG_DDIR_D_VAL		0x00000000
#define CFG_OCR1_D_VAL		0x00000000
#define CFG_OCR2_D_VAL		0x00000000
#define CFG_ICFA1_D_VAL		0xFFFFFFFF
#define CFG_ICFA2_D_VAL		0xFFFFFFFF
#define CFG_ICFB1_D_VAL		0xFFFFFFFF
#define CFG_ICFB2_D_VAL		0xFFFFFFFF
#define CFG_DR_D_VAL		0x00000000
#define CFG_GIUS_D_VAL		0xFFFFFFFF
#define CFG_ICR1_D_VAL		0x00000000
#define CFG_ICR2_D_VAL		0x00000000
#define CFG_IMR_D_VAL		0x00000000
#define CFG_GPR_D_VAL		0x00000000
#define CFG_PUEN_D_VAL		0xFFFFFFFF

#define CFG_GPCR_VAL		0x000003AB

/* FMCR Bit 1 becomes 0 to make CS3 if we have only one sdram bank*/
#if (CONFIG_NR_DRAM_BANKS == 1)
#define CFG_FMCR_VAL 0x00000001
#elif (CONFIG_NR_DRAM_BANKS == 2)
#define CFG_FMCR_VAL 0x00000003
#endif

/*
 * FIXME: From here, there should not be any user configuration.
 * All Equations are automatic
 */


#define CFG_HCLK_LGTH		(1000/CONFIG_SYS_BUS_FREQ)	/* ns */

/* USB 48 MHz ; BUS 96MHz*/
#ifdef CFG_OSC16
#define CSCR_MASK 0x23030003
#else
#define CSCR_MASK 0x23000003
#endif
#define CFG_CSCR_VAL\
	(CSCR_MASK 						\
	|((((CONFIG_SYS_FREQ/CONFIG_USB_FREQ)-1)&0x07)<<26)	\
	|((((CONFIG_SYS_FREQ/CONFIG_SYS_BUS_FREQ)-1)&0x0F)<<10))

/* PERCLKx 16MHz */
#define CFG_PCDR_VAL\
	(((((CONFIG_SYS_FREQ/CONFIG_PERIF1_FREQ)-1)&0x0F)<<0)	\
	|((((CONFIG_SYS_FREQ/CONFIG_PERIF2_FREQ)-1)&0x0F)<<4)	\
	|((((CONFIG_SYS_FREQ/CONFIG_PERIF3_FREQ)-1)&0x7F)<<16))

/* SDRAM controller programming Values */
#if ((CFG_SDRAM_CLOCK_CYCLE_CL_1>(3*CFG_HCLK_LGTH))\
	||(CFG_SDRAM_CLOCK_CYCLE_CL_1<1))
#define REG_FIELD_SCL_VAL 3
#else
#define REG_FIELD_SCL_VAL\
	((CFG_SDRAM_CLOCK_CYCLE_CL_1+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)
#endif

#if ((CFG_SDRAM_ROW_PRECHARGE_DELAY>(2*CFG_HCLK_LGTH))\
	||(CFG_SDRAM_ROW_PRECHARGE_DELAY<1))
#define REG_FIELD_SRP_VAL 0
#else
#define REG_FIELD_SRP_VAL 1
#endif

#if (CFG_SDRAM_ROW_2_COL_DELAY>(3*CFG_HCLK_LGTH))
#define REG_FIELD_SRCD_VAL 0
#else
#define REG_FIELD_SRCD_VAL\
	((CFG_SDRAM_ROW_2_COL_DELAY+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)
#endif

#if (CFG_SDRAM_ROW_CYCLE_DELAY>(7*CFG_HCLK_LGTH))
#define REG_FIELD_SRC_VAL 0
#else
#define REG_FIELD_SRC_VAL\
	((CFG_SDRAM_ROW_CYCLE_DELAY+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)
#endif

#define REG_SDCTL_BASE_CONFIG (0x800a1000\
				| (((CFG_SDRAM_NUM_ROW-11)&0x3)<<24)\
				| (((CFG_SDRAM_NUM_COL-8)&0x3)<<20)\
				| (((CFG_SDRAM_REFRESH)&0x3)<<14)\
				| (((REG_FIELD_SCL_VAL)&0x3)<<8)\
				| (((REG_FIELD_SRP_VAL)&0x1)<<6)\
				| (((REG_FIELD_SRCD_VAL)&0x3)<<4)\
				| (((REG_FIELD_SRC_VAL)&0x7)<<0))

#define CFG_NORMAL_RW_CMD	((0x0<<28)+REG_SDCTL_BASE_CONFIG)
#define CFG_PRECHARGE_CMD	((0x1<<28)+REG_SDCTL_BASE_CONFIG)
#define CFG_AUTOREFRESH_CMD	((0x2<<28)+REG_SDCTL_BASE_CONFIG)
#define CFG_SET_MODE_REG_CMD	((0x3<<28)+REG_SDCTL_BASE_CONFIG)

/* Issue Mode register Command to SDRAM*/
#define CFG_SDRAM_MODE_REGISTER_VAL\
	((((CFG_SDRAM_BURST_LENGTH)&0x7)<<(CFG_SDRAM_NUM_COL+4))\
	| (((REG_FIELD_SCL_VAL)&0x3)<<(CFG_SDRAM_NUM_COL+4+4))\
	| (((CFG_SDRAM_SINGLE_ACCESS)&0x1)<<(CFG_SDRAM_NUM_COL+4+9)))

/* Issue Precharge all Command to SDRAM*/
#define CFG_SDRAM_PRECHARGE_ALL_VAL\
	(((CFG_SDRAM_SINGLE_ACCESS)&0x1)<<(CFG_SDRAM_NUM_COL+4+9))

/* undef CONFIG_INIT_CRITICAL is no more required to boot from sdram */
#undef CONFIG_INIT_CRITICAL

/*
 * CSxU_VAL:
 * 63|    x    |x|x x|x x x x|x x| x | x  |x x x x|48
 *   |DTACK_SEL|0|BCD|  BCS  |PSZ|PME|SYNC|  DOL  |
 *
 * 47| x x  | x x x x x x | x | x x x x | x x x x |32
 *   | CNC  |     WSC     | 0 |   WWS   |   EDC   |
 *
 * CSxL_VAL:
 * 31|  x x x x  | x x x x  | x x x x  | x x x x  |24
 *   |    OEA    |   OEN    |   WEA    |   WEN    |
 * 23|x x x x| x | x x x | x x  x x |x x| x |  x  | 0
 *   |  CSA  |EBC|  DSZ  | 0|SP|0|WP|0 0|PA |CSEN |
 */
#define CFG_CS0U_VAL\
	((((CFG_CS0_XTRA_DEAD_CYC+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)\
	|((((CFG_CS0_WRITE_XTRA_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<4)\
	|(((((CFG_CS0_READ_CYC_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)-1)&0x3F)<<8)\
	|((((CFG_CS0_CS_NEG_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x03)<<14)\
	|((((CFG_CS0_SYNC_DAT_OUT_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|((CFG_CS0_SYNC_ENABLE&0x01)<<20)\
	|((CFG_CS0_SYNC_PAGE_MODE_EMUL&0x01)<<21)\
	|((CFG_CS0_SYNC_PAGE_SIZE&0x03)<<22)\
	|((((CFG_CS0_SYNC_BURST_CLK_START+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|((CFG_CS0_SYNC_BURST_CLK_DIV&0x03)<<28))

#define CFG_CS0L_VAL\
	((CFG_CS0_CHIP_SELECT_ENABLE&0x01)\
	|((CFG_CS0_WRITE_PROTECT&0x01)<<4)\
	|((CFG_CS0_SUPERVISOR_PROTECT&0x01)<<6)\
	|((CFG_CS0_DATA_PORT_SIZE&0x07)<<8)\
	|((CFG_CS0_EB_SIGNAL_CONTROL_WRITE&0x01)<<11)\
	|((((CFG_CS0_CS_ASSERT_NEG_DLY+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<12)\
	|(((((CFG_CS0_EB_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|(((((CFG_CS0_EB_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<20)\
	|(((((CFG_CS0_OE_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|(((((CFG_CS0_OE_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<28))

#define CFG_CS1U_VAL\
	((((CFG_CS1_XTRA_DEAD_CYC+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)\
	|((((CFG_CS1_WRITE_XTRA_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<4)\
	|(((((CFG_CS1_READ_CYC_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)-1)&0x3F)<<8)\
	|((((CFG_CS1_CS_NEG_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x03)<<14)\
	|((((CFG_CS1_SYNC_DAT_OUT_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|((CFG_CS1_SYNC_ENABLE&0x01)<<20)\
	|((CFG_CS1_SYNC_PAGE_MODE_EMUL&0x01)<<21)\
	|((CFG_CS1_SYNC_PAGE_SIZE&0x03)<<22)\
	|((((CFG_CS1_SYNC_BURST_CLK_START+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|((CFG_CS1_SYNC_BURST_CLK_DIV&0x03)<<28))

#define CFG_CS1L_VAL\
	((CFG_CS1_CHIP_SELECT_ENABLE&0x01)\
	|((CFG_CS1_PIN_ASSERT&0x01)<<1)\
	|((CFG_CS1_WRITE_PROTECT&0x01)<<4)\
	|((CFG_CS1_SUPERVISOR_PROTECT&0x01)<<6)\
	|((CFG_CS1_DATA_PORT_SIZE&0x07)<<8)\
	|((CFG_CS1_EB_SIGNAL_CONTROL_WRITE&0x01)<<11)\
	|((((CFG_CS1_CS_ASSERT_NEG_DLY+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<12)\
	|(((((CFG_CS1_EB_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|(((((CFG_CS1_EB_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<20)\
	|(((((CFG_CS1_OE_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|(((((CFG_CS1_OE_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<28))

#define CFG_CS2U_VAL\
	(((((CFG_CS2_XTRA_DEAD_CYC+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)\
	|((((CFG_CS2_WRITE_XTRA_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<4)\
	|(((((CFG_CS2_READ_CYC_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)-1)&0x3F)<<8)\
	|((((CFG_CS2_CS_NEG_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x03)<<14)\
	|(((CFG_CS2_SYNC_DAT_OUT_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|((CFG_CS2_SYNC_ENABLE&0x01)<<20)\
	|((CFG_CS2_SYNC_PAGE_MODE_EMUL&0x01)<<21)\
	|((CFG_CS2_SYNC_PAGE_SIZE&0x03)<<22)\
	|((((CFG_CS2_SYNC_BURST_CLK_START+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|((CFG_CS2_SYNC_BURST_CLK_DIV&0x03)<<28))

#define CFG_CS2L_VAL\
	((CFG_CS2_CHIP_SELECT_ENABLE&0x01)\
	|((CFG_CS2_PIN_ASSERT&0x01)<<1)\
	|((CFG_CS2_WRITE_PROTECT&0x01)<<4)\
	|((CFG_CS2_SUPERVISOR_PROTECT&0x01)<<6)\
	|((CFG_CS2_DATA_PORT_SIZE&0x07)<<8)\
	|((CFG_CS2_EB_SIGNAL_CONTROL_WRITE&0x01)<<11)\
	|((((CFG_CS2_CS_ASSERT_NEG_DLY+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<12)\
	|(((((CFG_CS2_EB_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|(((((CFG_CS2_EB_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<20)\
	|(((((CFG_CS2_OE_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|(((((CFG_CS2_OE_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<28))

#define CFG_CS3U_VAL\
	((((CFG_CS3_XTRA_DEAD_CYC+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)\
	|((((CFG_CS3_WRITE_XTRA_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<4)\
	|(((((CFG_CS3_READ_CYC_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)-1)&0x3F)<<8)\
	|((((CFG_CS3_CS_NEG_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x03)<<14)\
	|((((CFG_CS3_SYNC_DAT_OUT_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|((CFG_CS3_SYNC_ENABLE&0x01)<<20)\
	|((CFG_CS3_SYNC_PAGE_MODE_EMUL&0x01)<<21)\
	|((CFG_CS3_SYNC_PAGE_SIZE&0x03)<<22)\
	|((((CFG_CS3_SYNC_BURST_CLK_START+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|((CFG_CS3_SYNC_BURST_CLK_DIV&0x03)<<28))

#define CFG_CS3L_VAL\
	((CFG_CS3_CHIP_SELECT_ENABLE&0x01)\
	|((CFG_CS3_PIN_ASSERT&0x01)<<1)\
	|((CFG_CS3_WRITE_PROTECT&0x01)<<4)\
	|((CFG_CS3_SUPERVISOR_PROTECT&0x01)<<6)\
	|((CFG_CS3_DATA_PORT_SIZE&0x07)<<8)\
	|((CFG_CS3_EB_SIGNAL_CONTROL_WRITE&0x01)<<11)\
	|((((CFG_CS3_CS_ASSERT_NEG_DLY+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<12)\
	|(((((CFG_CS3_EB_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|(((((CFG_CS3_EB_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<20)\
	|(((((CFG_CS3_OE_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|(((((CFG_CS3_OE_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<28))

#define CFG_CS4U_VAL\
	(((((CFG_CS4_XTRA_DEAD_CYC+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)\
	|((((CFG_CS4_WRITE_XTRA_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<4)\
	|(((((CFG_CS4_READ_CYC_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)-1)&0x3F)<<8)\
	|((((CFG_CS4_CS_NEG_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x03)<<14)\
	|((((CFG_CS4_SYNC_DAT_OUT_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16))\
	|((CFG_CS4_SYNC_ENABLE&0x01)<<20)\
	|((CFG_CS4_SYNC_PAGE_MODE_EMUL&0x01)<<21)\
	|((CFG_CS4_SYNC_PAGE_SIZE&0x03)<<22)\
	|((((CFG_CS4_SYNC_BURST_CLK_START+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|((CFG_CS4_SYNC_BURST_CLK_DIV&0x03)<<28))

#define CFG_CS4L_VAL\
	((CFG_CS4_CHIP_SELECT_ENABLE&0x01)\
	|((CFG_CS4_PIN_ASSERT&0x01)<<1)\
	|((CFG_CS4_WRITE_PROTECT&0x01)<<4)\
	|((CFG_CS4_SUPERVISOR_PROTECT&0x01)<<6)\
	|((CFG_CS4_DATA_PORT_SIZE&0x07)<<8)\
	|((CFG_CS4_EB_SIGNAL_CONTROL_WRITE&0x01)<<11)\
	|((((CFG_CS4_CS_ASSERT_NEG_DLY+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<12)\
	|(((((CFG_CS4_EB_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|(((((CFG_CS4_EB_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<20)\
	|(((((CFG_CS4_OE_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|(((((CFG_CS4_OE_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<28))

#define CFG_CS5U_VAL\
	((((CFG_CS5_XTRA_DEAD_CYC+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)\
	|((((CFG_CS5_WRITE_XTRA_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<4)\
	|(((((CFG_CS5_READ_CYC_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)-1)&0x3F)<<8)\
	|((((CFG_CS5_CS_NEG_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x03)<<14)\
	|((((CFG_CS5_SYNC_DAT_OUT_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|((CFG_CS5_SYNC_ENABLE&0x01)<<20)\
	|((CFG_CS5_SYNC_PAGE_MODE_EMUL&0x01)<<21)\
	|((CFG_CS5_SYNC_PAGE_SIZE&0x03)<<22)\
	|((((CFG_CS5_SYNC_BURST_CLK_START+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|((CFG_CS5_SYNC_BURST_CLK_DIV&0x03)<<28)\
	|((CFG_CS5_DTACK_SELECT&0x01)<<31))

#define CFG_CS5L_VAL\
	((CFG_CS5_CHIP_SELECT_ENABLE&0x01)\
	|((CFG_CS5_PIN_ASSERT&0x01)<<1)\
	|((CFG_CS5_WRITE_PROTECT&0x01)<<4)\
	|((CFG_CS5_SUPERVISOR_PROTECT&0x01)<<6)\
	|((CFG_CS5_DATA_PORT_SIZE&0x07)<<8)\
	|((CFG_CS5_EB_SIGNAL_CONTROL_WRITE&0x01)<<11)\
	|((((CFG_CS5_CS_ASSERT_NEG_DLY+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<12)\
	|(((((CFG_CS5_EB_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|(((((CFG_CS5_EB_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<20)\
	|(((((CFG_CS5_OE_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|(((((CFG_CS5_OE_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<28))

#endif /* __CONFIG_H */
