/*
 *
 * Configuration settings for the Armadeus Project apf51 board.
 *
 * (C) Copyright 2010 Eric Jarrige, Armadeus Project 
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
 * 2010/03/31 Initial version based on apf27 configuration file
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_VERSION_VARIABLE
#define CONFIG_ENV_VERSION 	"0.2"
#define CONFIG_IDENT_STRING	" apf51 patch 0.1"

 /* High Level Configuration Options */
#define CONFIG_ARMV7		1	/* This is armv7 Cortex-A8 CPU	*/
#define CONFIG_MX51		1	/* in a Freescale i.MX51 Chip */
#define CONFIG_APF51		1	/* in a Armadeus project APF51 board */
#define CONFIG_L2_OFF
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

#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_DNS
#define CONFIG_BOOTP_DNS2

/*
 * Select serial console configuration
 */
#define CONFIG_MXC_UART
#define CONFIG_SYS_MX51_UART2		/* use UART2 for console */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400 }
#define CONFIG_BAUDRATE			115200

/*
 * Definition of u-boot build in commands. Check out CONFIG_CMD_DFL if
 * neccessary in include/cmd_confdefs.h file. (Un)comment for getting
 * functionality or size of u-boot code.
 */

#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV	/* ask for env variable		*/
#define CONFIG_CMD_BSP		/* Board Specific functions	*/
#define CONFIG_CMD_CACHE	/* icache, dcache		*/
#define CONFIG_CMD_CDP		/* Cisco Discovery Protocol	*/
//#define CONFIG_CMD_DATE		/* support for RTC, date/time...*/
#define CONFIG_CMD_DHCP		/* DHCP Support			*/
#define CONFIG_CMD_DIAG		/* Diagnostics			*/
//#define CONFIG_CMD_EEPROM	/* EEPROM read/write support	*/
#define CONFIG_CMD_ELF		/* bootelf, bootvx		*/
#define CONFIG_CMD_FAT		/* FAT support			*/
#define CONFIG_CMD_EXT2		/* EXT2 Support			*/
#undef  CONFIG_CMD_FLASH	/* flinfo, erase, protect	*/
//#define CONFIG_CMD_I2C		/* I2C serial bus support	*/
#define CONFIG_CMD_IIM		/* imx iim fuse			*/
#undef CONFIG_CMD_IMLS		/* List all found images	*/
#define CONFIG_CMD_IMMAP	/* IMMR dump support		*/
#define CONFIG_CMD_JFFS2	/* JFFS2 Support		*/
#define CONFIG_CMD_MII		/* MII support			*/
#define CONFIG_CMD_MTDPARTS	/* MTD partition support 	*/
#define CONFIG_CMD_MMC		/* MMC memory mapped support	*/
#define CONFIG_CMD_NAND		/* NAND support			*/
#define CONFIG_CMD_NAND_LOCK_UNLOCK
#define CONFIG_CMD_PING		/* ping support			*/
#define CONFIG_CMD_SAVES	/* save S record dump		*/
#define CONFIG_CMD_SETEXPR	/* setexpr support		*/
//#define CONFIG_CMD_SNTP		/* SNTP support			*/
#define CONFIG_CMD_NFS		/* NFS support			*/
#define CONFIG_CMD_DNS
//#define CONFIG_CMD_UBI
//#define CONFIG_CMD_UBIFS
//#define CONFIG_CMD_IMX_FUSE	/* iMX fuse access commands	*/

#define CONFIG_DISPLAY_BOARDINFO
#define CONFIG_DISPLAY_CPUINFO

/*
 * Select some advanced features of the commande line parser
 */
#define CONFIG_AUTO_COMPLETE 		1	/* Enable auto completion of */
						/* commands using TAB */
#define CONFIG_SYS_HUSH_PARSER		1	/* enable the "hush" shell */
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "	/* secondary prompt string */
#define CONFIG_CMDLINE_EDITING  	1

/*
 * Boot options. Setting delay to -1 stops autostart count down.
 */
#define CONFIG_BOOTDELAY	20
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_BOOTARGS \
	CONFIG_CONSOLE " root=/dev/mtdblock4 rootfstype=jffs2 " MTDPARTS_DEFAULT

#define CONFIG_CONSOLE "console=ttymxc1,"MK_STR(CONFIG_BAUDRATE)
#define CONFIG_MTDMAP   "mxc_nand.0"
#define MTDIDS_DEFAULT	"nand0=" CONFIG_MTDMAP
#define MTDPARTS_DEFAULT "mtdparts=" CONFIG_MTDMAP ":1M(U-boot)ro,"	\
			"1M(U-boot_env),1M(firmware),"			\
			"8M(kernel),"	\
			"-(rootfs)"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"env_version="		CONFIG_ENV_VERSION		"\0"	\
	"fileaddr="		MK_STR(CONFIG_SYS_LOAD_ADDR)		"\0"	\
	"filesize="		MK_STR(CONFIG_SYS_MONITOR_LEN)		"\0"	\
	"console="		CONFIG_CONSOLE			"\0"	\
	"mtdparts="		MTDPARTS_DEFAULT		"\0"	\
	"partition=nand0,4\0"						\
	"uboot_offset="		MK_STR(CONFIG_SYS_MONITOR_OFFSET)	"\0"	\
	"uboot_len="		MK_STR(CONFIG_SYS_MONITOR_LEN)		"\0"	\
	"env_offset="		MK_STR(CONFIG_ENV_OFFSET)		"\0"	\
	"env_len="		MK_STR(CONFIG_ENV_RANGE)		"\0"	\
	"firmware_offset="	MK_STR(CONFIG_FIRMWARE_OFFSET)	"\0"	\
	"firmware_len="	MK_STR(CONFIG_FIRMWARE_LEN)	"\0"	\
	"kernel_offset="	MK_STR(CONFIG_KERNEL_OFFSET)	"\0"	\
	"kernel_len="		MK_STR(CONFIG_KERNEL_LEN)	"\0"	\
	"rootfs_offset="	MK_STR(CONFIG_ROOTFS_OFFSET)	"\0"	\
	"rootfs_len="		MK_STR(CONFIG_ROOTFS_LEN)	"\0"	\
	"board_name="		MK_STR(CONFIG_BOARD_NAME)	"\0"	\
	"serverpath="		MK_STR(CONFIG_SRV_PATH)		"\0"	\
	"ntpserverip=217.147.208.1\0"					\
	"mmcroot=/dev/mmcblk0p1\0"					\
	"mmcrootfstype=ext2\0"						\
	"quiet=1\0"						\
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
		"run addnfsargs addipargs; setenv autostart yes;"	\
		"nboot.jffs2 90800000 0 ${kernel_offset}\0"		\
	"jffsboot=setenv bootargs ${console} ${mtdparts};"		\
		"run addjffsargs addipargs; setenv autostart yes;"	\
		"nboot.jffs2 90800000 0 ${kernel_offset}\0"		\
	"ubifsboot=setenv bootargs ${console} ${mtdparts};"		\
		"run addubifsargs addipargs; setenv autostart yes;"	\
		"nboot.jffs2 90800000 0 ${kernel_offset}\0"		\
	"mmcboot=setenv bootargs ${console} ${mtdparts};"		\
		"run addmmcargs addipargs; setenv autostart yes;"	\
		"nboot.jffs2 90800000 0 ${kernel_offset}\0"		\
	"firmware_autoload=0\0"					\
	"flash_uboot=nand unlock ${uboot_offset} ${uboot_len};"		\
		"nand erase ${uboot_offset} ${uboot_len};"		\
		"if nand write.jffs2 ${fileaddr} ${uboot_offset} ${filesize};"	\
			"then nand lock; nand unlock ${env_offset};"	\
				"echo Flashing of uboot succeed;"		\
			"else echo Flashing of uboot failed;"		\
		"fi; \0"						\
	"flash_firmware=nand erase ${firmware_offset} ${firmware_len};"\
		"if nand write.jffs2 ${fileaddr} ${firmware_offset} ${filesize} ;"\
			"then echo Flashing of Firmware succeed;"		\
			"else echo Flashing of Firmware failed;"		\
		"fi\0"							\
	"fpga_config=tftpboot ${fileaddr} $firmware_name;"\
		"run flash_firmware;"\
		"fpga load 0 ${fileaddr} ${firmware_len};\0"\
	"flash_kernel=nand erase ${kernel_offset} ${kernel_len};"	\
		"if nand write.jffs2 ${fileaddr} ${kernel_offset} ${filesize} ;"\
			"then echo Flashing of kernel succeed;"		\
			"else echo Flashing of kernel failed;"		\
		"fi\0"							\
	"flash_rootfs=nand erase clean ${rootfs_offset} ${rootfs_len};"	\
		"if nand write.jffs2 ${fileaddr} ${rootfs_offset} ${filesize};"\
			"then echo Flashing of rootfs succeed;"		\
			"else echo Flashing of rootfs failed;"		\
		"fi\0"							\
	"flash_reset_env=nand unlock ${env_offset} ${env_len};"		\
		"nand erase ${env_offset} ${env_len};"			\
		"echo Flash environment variables erased!\0"		\
	"download_uboot=tftpboot ${loadaddr} "				\
		" ${serverpath}${board_name}-u-boot.bin\0"		\
	"download_kernel=tftpboot ${loadaddr} "				\
		" ${serverpath}${board_name}-linux.bin\0"		\
	"download_rootfs=tftpboot ${loadaddr} "				\
		" ${serverpath}${board_name}-rootfs.arm.ubi\0"	\
	"update_uboot=run download_uboot flash_uboot\0"			\
	"update_kernel=run download_kernel flash_kernel\0"		\
	"update_rootfs=run download_rootfs flash_rootfs\0"		\
	"update_all=run download_kernel flash_kernel download_rootfs "	\
		"flash_rootfs download_uboot flash_uboot\0"		\
	"unlock_regs=mw 10000008 0; mw 10020008 0;TODO\0"

#define CONFIG_BOOTCOMMAND	"run ubifsboot"
#define CONFIG_SYS_AUTOLOAD		"no"

#define CONFIG_MACH_TYPE MACH_TYPE_APF51
#define CONFIG_BOOT_PARAMS_ADDR	0x90000100

#define CONFIG_SHOW_BOOT_PROGRESS

#define CONFIG_ETHADDR
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.0.10
#define CONFIG_BOARD_NAME	"apf51"
#define CONFIG_HOSTNAME		"apf51"
#define CONFIG_GATEWAYIP	192.168.0.1
#define CONFIG_SERVERIP		192.168.0.2
#define CONFIG_SRV_PATH		""
#define CONFIG_ROOTPATH		"/tftpboot/apf51-root"

/**
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP				/* undef to save memory */
#define CONFIG_SYS_PROMPT		"BIOS> "	/* prompt string      */
#define CONFIG_SYS_CBSIZE		2048		/* console I/O buffer */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)	/* print buffer size  */
#define CONFIG_SYS_MAXARGS		16		/* max command args   */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE	/* boot args buf size */

#define CONFIG_SYS_MEMTEST_START	0x90000000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x90300000

#define CONFIG_SYS_HZ			1000 		/* incrementer freq: 1 kHz   */

/* To check */

//#define CONFIG_DOWNLOAD_BY_DEBUGGER

#ifndef CONFIG_DOWNLOAD_BY_DEBUGGER
#define CONFIG_FLASH_HEADER		1
#define CONFIG_FLASH_HEADER_OFFSET	0x400
#define CONFIG_FLASH_HEADER_BARKER	0xB1
#endif

//#define CONFIG_SKIP_RELOCATE_UBOOT

#define BOARD_LATE_INIT
/* End of To check */

/*
 * Definitions related to passing arguments to kernel.
 */

#define CONFIG_CMDLINE_TAG		1	/* send commandline to Kernel        */
#define CONFIG_SETUP_MEMORY_TAGS	1	/* send memory definition to kernel */
#define CONFIG_INITRD_TAG		1	/* send initrd params                */
#undef	CONFIG_VFD				/* do not send framebuffer setup    */
//#define CONFIG_REVISION_TAG		1

/*
 * Malloc pool need to host env. + 2 MB reserve for other allocations.
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (2 * 1024 * 1024) )
#define CONFIG_SYS_GBL_DATA_SIZE	256     	/* size in bytes reserved for initial data */
#define CONFIG_STACKSIZE		(256<<10)	/* stack size	*/

#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4<<10)		/* IRQ stack                  */
#define CONFIG_STACKSIZE_FIQ	(4<<10)		/* FIQ stack                  */
#endif

/**
 * iMX51 PLL constants
 */

/* PLL parameters assuming 32.768kHz *1024 input clock with doubler ON */
/*				  MFI         PDF */
#define DP_OP_850_32768		((6 << 4) + ((1 - 1)  << 0))
#define DP_MFD_850_32768	(1048576 - 1)
#define DP_MFN_850_32768	349169

#define DP_OP_800_32768		((11 << 4) + ((2 - 1)  << 0))
#define DP_MFD_800_32768	(65536 - 1)
#define DP_MFN_800_32768	60354

#define DP_OP_700_32768		((5 << 4) + ((1 - 1)  << 0))
#define DP_MFD_700_32768	(524288 - 1)
#define DP_MFN_700_32768	112935

#define DP_OP_665_32768		((14 << 4) + ((3 - 1)  << 0))
#define DP_MFD_665_32768	(2097152 - 1)
#define DP_MFN_665_32768	1811747

#define DP_OP_532_32768		((15 << 4) + ((4 - 1)  << 0))
#define DP_MFD_532_32768	(524288 - 1)
#define DP_MFN_532_32768	448180

#define DP_OP_400_32768		((14 << 4) + ((5 - 1)  << 0))
#define DP_MFD_400_32768	(131072 - 1)
#define DP_MFN_400_32768	118117

#define DP_OP_216_32768		((8 << 4) + ((5 - 1)  << 0))
#define DP_MFD_216_32768	(262144 - 1)
#define DP_MFN_216_32768	12223

/* PLL parameters assuming 32kHz *1024 input clock with doubler ON */
/*				  MFI         PDF */
#define DP_OP_850_32000		((6 << 4) + ((1 - 1)  << 0))
#define DP_MFD_850_32000	(8192 - 1)
#define DP_MFN_850_32000	3973

#define DP_OP_800_32000		((6 << 4) + ((1 - 1)  << 0))
#define DP_MFD_800_32000	(512 - 1)
#define DP_MFN_800_32000	53

#define DP_OP_700_32000		((5 << 4) + ((1 - 1)  << 0))
#define DP_MFD_700_32000	(4096 - 1)
#define DP_MFN_700_32000	1395

#define DP_OP_665_32000		((5 << 4) + ((1 - 1)  << 0))
#define DP_MFD_665_32000	(16384 - 1)
#define DP_MFN_665_32000	1205

#define DP_OP_532_32000		((8 << 4) + ((2 - 1)  << 0))
#define DP_MFD_532_32000	(4096 - 1)
#define DP_MFN_532_32000	482

#define DP_OP_400_32000		((6 << 4) + ((2 - 1)  << 0))
#define DP_MFD_400_32000	(1024 - 1)
#define DP_MFN_400_32000	106

#define DP_OP_216_32000		((13 << 4) + ((8 - 1)  << 0))
#define DP_MFD_216_32000	(2048 - 1)
#define DP_MFN_216_32000	376

/*
* Clocks configuration
*/

#define CONFIG_MX51_CLK32		32768
//#define	CONFIG_SYS_MX51_HCLK_FREQ	24000000

#ifdef CONFIG_SYS_MX51_HCLK_FREQ
#define CONFIG_MX51_HCLK_FREQ		CONFIG_SYS_MX51_HCLK_FREQ
#else
#define CONFIG_MX51_HCLK_FREQ		1024*CONFIG_MX51_CLK32
#endif

/* external bus frequencies */
#define CONFIG_DDR_CLK_FREQ	200 	/* MHZ DDR frequency from PLL1*/
#define CONFIG_ARM_CLK_FREQ	800	/* MHZ CPU frequency from PLL1*/
#define CONFIG_AXIA_CLK_FREQ	166	/* MHZ AXI A frequency from PLL2*/
#define CONFIG_AXIB_CLK_FREQ	110	/* 110.8 133 MHZ AXI B frequency from PLL2*/
#define CONFIG_EMI_CLK_FREQ	83	/* MHZ AHB frequency from PLL2*/
#define CONFIG_AHB_CLK_FREQ	133	/* MHZ AHB frequency from PLL2*/
#define CONFIG_IPG_CLK_FREQ	66	/* 66.5 MHZ IPG frequency from PLL2*/
#define CONFIG_PERCLK_FREQ	55	/* 55.4 MHZ PERCLK frequency */
#define CONFIG_NFC_FREQ		27	/* MHZ NFC Clock from EMI clk*/
#define CONFIG_USB_FREQ		54	/* external USB serial bus frequency PLL3 */
#define CONFIG_ESDHC_FREQ 	54	/* MHz ESDHC controller frequency / PLL3 */
#define CONFIG_UART_FREQ	66	/* 66.5 MHz UART controller frequency from PLL2*/

//#define CONFIG_SSI_FREQ		54	/* MHz SSI controller frequency */
//#define CONFIG_TVE_FREQ		216	/* MHz TVE controller frequency */
//#define CONFIG_DI_FREQ		0	/* MHz DISPLAY controller frequency*/
//#define CONFIG_VPU_FREQ		0	/* MHz VPU controller frequency*/
//#define CONFIG_SPDIF_FREQ	10	/* MHz SPDIF controller frequency 10.3 MHz*/
//#define CONFIG_SIM_FREQ		54	/* MHz SIM controller frequency */
//#define CONFIG_FIRI_FREQ	54	/* MHz FIRI controller frequency */
//#define CONFIG_HSI2C_FREQ	54	/* MHz HS I2C controller frequency */
//#define CONFIG_CSI_FREQ		54	/* MHz HS CSI controller frequency */
//#define CONFIG_ECSPI_FREQ	54	/* MHz HECSPI controller frequency */
//#define CONFIG_WRCK_FREQ	13	/* MHz WRCK 13.3 MHz */
//#define CONFIG_LPSR_FREQ	0	/* MHz  */
#define CONFIG_PGC_FREQ		66	/* MHz 66.5 MHz */

#ifndef CONFIG_SYS_MX51_HCLK_FREQ
#if (CONFIG_MX51_CLK32 == 32000)

#define CONFIG_PLL1_FREQ		800		/* MHz */
#define CONFIG_PLL1_OP			DP_OP_800_32000
#define CONFIG_PLL1_MFD			DP_MFD_800_32000
#define CONFIG_PLL1_MFN			DP_MFN_800_32000

#define CONFIG_PLL2_FREQ		665		/* MHz */
#define CONFIG_PLL2_OP			DP_OP_665_32000
#define CONFIG_PLL2_MFD			DP_MFD_665_32000
#define CONFIG_PLL2_MFN			DP_MFN_665_32000

#define CONFIG_PLL3_FREQ		216		/* MHz */
#define CONFIG_PLL3_OP			DP_OP_216_32000
#define CONFIG_PLL3_MFD			DP_MFD_216_32000
#define CONFIG_PLL3_MFN			DP_MFN_216_32000

#else /* CONFIG_MX51_CLK32 == 32768*/

#define CONFIG_PLL1_FREQ		800		/* MHz */
#define CONFIG_PLL1_OP			DP_OP_800_32768
#define CONFIG_PLL1_MFD			DP_MFD_800_32768
#define CONFIG_PLL1_MFN			DP_MFN_800_32768

#define CONFIG_PLL2_FREQ		665		/* MHz */
#define CONFIG_PLL2_OP			DP_OP_665_32768
#define CONFIG_PLL2_MFD			DP_MFD_665_32768
#define CONFIG_PLL2_MFN			DP_MFN_665_32768

#define CONFIG_PLL3_FREQ		216		/* MHz */
#define CONFIG_PLL3_OP			DP_OP_216_32768
#define CONFIG_PLL3_MFD			DP_MFD_216_32768
#define CONFIG_PLL3_MFN			DP_MFN_216_32768

#endif /* CONFIG_MX51_CLK32 == 32768*/
#else /* CONFIG_SYS_MX51_HCLK_FREQ in use*/

#define CONFIG_PLL1_FREQ		800		/* MHz */
#define CONFIG_PLL1_OP			DP_OP_800
#define CONFIG_PLL1_MFD			DP_MFD_800
#define CONFIG_PLL1_MFN			DP_MFN_800

#define CONFIG_PLL2_FREQ		665		/* MHz */
#define CONFIG_PLL2_OP			DP_OP_665
#define CONFIG_PLL2_MFD			DP_MFD_665
#define CONFIG_PLL2_MFN			DP_MFN_665

#define CONFIG_PLL3_FREQ		216		/* MHz */
#define CONFIG_PLL3_OP			DP_OP_216
#define CONFIG_PLL3_MFD			DP_MFD_216
#define CONFIG_PLL3_MFN			DP_MFN_216

#endif /* CONFIG_SYS_MX51_HCLK_FREQ in use*/

/*
 * SDRAM Configs
 */
#define CONFIG_SYS_NR_DRAM_BANKS 1
#define CONFIG_SYS_SDRAM_MBYTE_SYZE 128
#define CONFIG_SYS_SDRAM_TYPE_MDDR

#define CONFIG_SYS_SDRAM_1_BASE	CSD0_BASE_ADDR		/* SDRAM bank #1 */
#define CONFIG_SYS_SDRAM_2_BASE	CSD1_BASE_ADDR		/* SDRAM bank #2 */

/*
 * Configuration for no NOR FLASH memory
 */
#define CONFIG_SYS_NO_FLASH

/*=====================*/
/* Flash & Environment */
/*=====================*/
#define CONFIG_ENV_SECT_SIZE		(128 * 1024)

/*-----------------------------------------------------------------------
 * NAND FLASH driver setup
 */
#define CONFIG_NAND_MXC_V2
#define CONFIG_MXC_NAND_REGS_BASE	0xCFFF0000
#define CONFIG_SYS_NAND_BASE		CONFIG_MXC_NAND_REGS_BASE
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_MXC_NAND_HWECC
#define CONFIG_SYS_NAND_LARGEPAGE
//#define CONFIG_MTD_NAND_VERIFY_WRITE
#define CONFIG_MTD_NAND_ECC_JFFS2

//#define CONFIG_SYS_DIRECT_FLASH_TFTP	1
#define CONFIG_FLASH_SHOW_PROGRESS	45
#define NAND_MAX_CHIPS			1

/*=====================*/

#define CONFIG_NAND_U_BOOT
#define CONFIG_SYS_NAND_U_BOOT_DST	CONFIG_SYS_LOAD_ADDR	/* Copy SPL+U-Boot here	     */
#define CONFIG_SYS_NAND_U_BOOT_SIZE	CONFIG_SYS_MONITOR_LEN /* Size is the partion size  */
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

#define	CONFIG_SYS_MONITOR_BASE		0x9FF00000
#define	CONFIG_SYS_MONITOR_OFFSET	0x00000000
#define	CONFIG_SYS_MONITOR_LEN		0x00100000 /* 1MB ( 8 flash blocks )  */
#define CONFIG_ENV_IS_IN_NAND		1
#define	CONFIG_ENV_OFFSET		0x00100000 /* NAND offset address for now*/
#define	CONFIG_ENV_SIZE			0x00020000 /* 128kB  */
#define CONFIG_ENV_RANGE		0X00100000 /* 1MB ( 8 flash blocks )  */
#define	CONFIG_ENV_OVERWRITE		1	   /* env is writable now   */
#define	CONFIG_FIRMWARE_OFFSET		0x00200000
#define	CONFIG_FIRMWARE_LEN		0x00100000 /* 1MB ( 8 flash blocks )  */
#define	CONFIG_KERNEL_OFFSET		0x00300000
#define	CONFIG_KERNEL_LEN		0x00800000 /* 8MB (BB ready)*/
#define	CONFIG_ROOTFS_OFFSET		0x00B00000
#define	CONFIG_ROOTFS_LEN			/* entire flash if not specified ~501MB*/

/*
 * Default load address for user programs and kernel
 */
#define CONFIG_SYS_LOAD_ADDR		0x90000000
#define	CONFIG_LOADADDR			0x90000000
#define CONFIG_SYS_TFTP_LOADADDR	0x90000000

/*
 * Partitions & Filsystems
 */
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MAC_PARTITION
#define CONFIG_DOS_PARTITION
#define CONFIG_ISO_PARTITION
#define CONFIG_SUPPORT_VFAT

/*
 * JFFS2
 */
#define CONFIG_JFFS2_NAND	1
#define CONFIG_JFFS2_CMDLINE	1
#define CONFIG_JFFS2_DEV	"nand0"

/*
 * This is setting for JFFS2 support in u-boot.
 * NOTE: Enable CONFIG_SYS_CMD_JFFS2 for JFFS2 support.
 */
#define CONFIG_SYS_JFFS_CUSTOM_PART

/*
 * UBIFS
 */
#define CONFIG_RBTREE
#define CONFIG_LZO

/*
 * Hardware drivers
 */

/**
 * Ethernet: Support for internal Fast Ethernet Controller
 */
#define CONFIG_FEC_MXC
#define IMX_FEC_BASE			FEC_BASE_ADDR
#define CONFIG_SYS_PHY_RESET_GPIO	MX51_PIN_DI1_PIN11

#define CONFIG_NET_MULTI		1
#define CONFIG_FEC_MXC_PHYADDR		0x02
#define CONFIG_MII			1
#define CONFIG_DISCOVER_PHY

/*-----------------------------------------------------------------------
 * FPGA
 */
#define CONFIG_FPGA			CONFIG_SYS_SPARTAN3
//#define FPGA_DEBUG
#define CONFIG_FPGA_COUNT		1
#define CONFIG_FPGA_XILINX
#define CONFIG_FPGA_SPARTAN3
#define CONFIG_SYS_FPGA_WAIT		20000 /* 20 ms */
#define CONFIG_SYS_FPGA_PROG_FEEDBACK
#define CONFIG_SYS_FPGA_CHECK_CTRLC
#define CONFIG_SYS_FPGA_CHECK_ERROR

/* FPGA program pin configuration */
#define CONFIG_SYS_FPGA_PWR	MX51_PIN_DI1_D0_CS
#define CONFIG_SYS_FPGA_PRG	MX51_PIN_CSI2_D12
#define CONFIG_SYS_FPGA_RDATA	CS1_BASE_ADDR
#define CONFIG_SYS_FPGA_WDATA	CS1_BASE_ADDR
#define CONFIG_SYS_FPGA_INIT	MX51_PIN_CSI2_D18
#define CONFIG_SYS_FPGA_DONE	MX51_PIN_CSI2_D13
#define CONFIG_SYS_FPGA_SUSPEND MX51_PIN_DISPB2_SER_DIO

/**
 * I2C Configs TODO TODO TODO TODO
 */
//#define	CONFIG_HARD_I2C			1	/* I2C with hardware support    */
//#define CONFIG_I2C_MXC			1
#define CONFIG_SYS_I2C_PORT		I2C2_BASE_ADDR
#define CONFIG_SYS_I2C_SPEED		400000
#define CONFIG_SYS_I2C_SLAVE		0xfe

#define CONFIG_I2C_CMD_TREE
//#define CONFIG_I2C_MULTI_BUS	2
#define CONFIG_SYS_I2C_NOPROBES 	{ }

//# define CONFIG_SYS_I2C_EEPROM_ADDR	0x50	/* EEPROM 24LC02                */
//# define CONFIG_SYS_I2C_EEPROM_ADDR_LEN 1	/* bytes of address             */
/* mask of address bits that overflow into the "EEPROM chip address"    */
/*#define CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW	0x00*/
//#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	3
//#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	10	/* takes up to 10 msec */

/**
 * RTC PMIC TODO TODO TODO TODO
 */
//#define CONFIG_RTC_DS1374
//#define CONFIG_SYS_RTC_BUS_NUM		0
//#define DEBUG_RTC

/*
 * SD/MMC Configs
 */
#define CONFIG_MMC              1
#define CONFIG_GENERIC_MMC
#define CONFIG_FSL_MMC		1
#define CONFIG_DEFAULT_MMC	2	/* 0-SD1, 1-SD2, 2-SD3, 3-SD4 */
#define CONFIG_MULTI_MMC	1
#define CONFIG_MMC_BASE         0x0

//#define CONFIG_MXC_MMC
//#define CONFIG_MXC_MCI_REGS_BASE

/**
 * SPI Configs TODO TODO TODO TODO TODO
 */
//#define CONFIG_IMX_SPI


/**
 * Watchdog Config
 */

#define CONFIG_HW_WATCHDOG

/**
 * IIM fuse Config
 */

#define CONFIG_IMX_IIM
#define IMX_IIM_BASE    IIM_BASE_ADDR
#define CONFIG_IIM_MAC_BANK     1
#define CONFIG_IIM_MAC_ROW      9

/*
 * From here, there should not be any user configuration.
 * All Equations are automatic
 */

/**
 * SDRAM controler configs
 */
#define CONFIG_SYS_SDRAM_1_SIZE	(CONFIG_SYS_SDRAM_MBYTE_SYZE) * 1024 * 1024
#define CONFIG_SYS_SDRAM_2_SIZE	(CONFIG_SYS_SDRAM_MBYTE_SYZE) * 1024 * 1024
#define CONFIG_NR_DRAM_BANKS 1

#ifdef CONFIG_SYS_SDRAM_TYPE_MDDR
#if (CONFIG_SYS_SDRAM_MBYTE_SYZE == 64) /* micron MT46H16M32LF -6 */
/* micron 64MB */
#define CONFIG_SYS_SDRAM_NUM_COL		9  /* 8, 9, 10 or 11 column address bits */
#define CONFIG_SYS_SDRAM_NUM_ROW		13 /* 11, 12 or 13 row address bits */
#define CONFIG_SYS_SDRAM_REFRESH		3  /* 0=OFF 1=2048 2=4096 3=8192.. refresh */
#define CONFIG_SYS_SDRAM_XSR_DELAY		138 /* ns Exit self refresh to any command */
#define CONFIG_SYS_SDRAM_EXIT_PWD		10 /* ns exit power down delay */
#define CONFIG_SYS_SDRAM_W2R_DELAY		1  /* WRITE to READ Command Delay 0=1ck 1=2ck  */
#define CONFIG_SYS_SDRAM_ROW_PRECHARGE_DELAY	15 /* ns (>2 x ddr_clk)*/
#define CONFIG_SYS_SDRAM_TMRD_DELAY		2  /* Load mode register cycle delay 1..4 */
#define CONFIG_SYS_SDRAM_TWR_DELAY		1  /* DDR: 0=2ck 1=3ck */
#define CONFIG_SYS_SDRAM_RAS_DELAY		40 /* ns ACTIVE-to-PRECHARGE delay */
#define CONFIG_SYS_SDRAM_RRD_DELAY		10 /* ns ACTIVE-to-ACTIVE delay */
#define CONFIG_SYS_SDRAM_RCD_DELAY		15 /* ns Row to Column delay */
#define CONFIG_SYS_SDRAM_RC_DELAY		55 /* ns Row cycle delay (tRFC refresh to command)*/
#define CONFIG_SYS_SDRAM_RFC_DELAY		110 /* ns (> 10xddr_clk) auto refresh to any command*/
#define CONFIG_SYS_SDRAM_PARTIAL_ARRAY_SR	0  /* 0=full 1=half 2=quater 3=Eighth 4=Sixteenth */
#define CONFIG_SYS_SDRAM_DRIVE_STRENGH		0  /* 0=Full-strength 1=half 2=quater 3=Eighth */
#define CONFIG_SYS_SDRAM_BURST_LENGTH		3  /* 2^N BYTES (N=0..3) */
#define CONFIG_SYS_SDRAM_SINGLE_ACCESS		0  /* 1= single access; 0 = Burst mode */
#endif

#if (CONFIG_SYS_SDRAM_MBYTE_SYZE == 128)
/* micron 128MB */
#define CONFIG_SYS_SDRAM_NUM_COL		10  /* 8, 9, 10 or 11 column address bits */
#define CONFIG_SYS_SDRAM_NUM_ROW		13 /* 11, 12 or 13 row address bits */
#define CONFIG_SYS_SDRAM_REFRESH		3  /* 0=OFF 1=2048 2=4096 3=8192.. refresh */
#define CONFIG_SYS_SDRAM_XSR_DELAY		138 /* ns Exit self refresh to any command */
#define CONFIG_SYS_SDRAM_EXIT_PWD		10 /* ns exit power down delay */
#define CONFIG_SYS_SDRAM_W2R_DELAY		1  /* WRITE to READ Command Delay 0=1ck 1=2ck  */
#define CONFIG_SYS_SDRAM_ROW_PRECHARGE_DELAY	15 /* ns (>2 x ddr_clk)*/
#define CONFIG_SYS_SDRAM_TMRD_DELAY		2  /* Load mode register cycle delay 1..4 */
#define CONFIG_SYS_SDRAM_TWR_DELAY		1  /* DDR: 0=2ck 1=3ck */
#define CONFIG_SYS_SDRAM_RAS_DELAY		40 /* ns ACTIVE-to-PRECHARGE delay */
#define CONFIG_SYS_SDRAM_RRD_DELAY		10 /* ns ACTIVE-to-ACTIVE delay */
#define CONFIG_SYS_SDRAM_RCD_DELAY		15 /* ns Row to Column delay */
#define CONFIG_SYS_SDRAM_RC_DELAY		55 /* ns Row cycle delay (tRFC refresh to command)*/
#define CONFIG_SYS_SDRAM_RFC_DELAY		110 /* ns (> 10xddr_clk) auto refresh to any command*/
#define CONFIG_SYS_SDRAM_PARTIAL_ARRAY_SR	0  /* 0=full 1=half 2=quater 3=Eighth 4=Sixteenth */
#define CONFIG_SYS_SDRAM_DRIVE_STRENGH		0  /* 0=Full-strength 1=half 2=quater 3=Eighth */
#define CONFIG_SYS_SDRAM_BURST_LENGTH		3  /* 2^N BYTES (N=0..3) */
#define CONFIG_SYS_SDRAM_SINGLE_ACCESS		0  /* 1= single access; 0 = Burst mode */
#endif

#if (CONFIG_SYS_SDRAM_MBYTE_SYZE == 256)
/* micron 256MB */
#define CONFIG_SYS_SDRAM_NUM_COL		10  /* 8, 9, 10 or 11 column address bits */
#define CONFIG_SYS_SDRAM_NUM_ROW		14 /* 11, 12 or 13 row address bits */
#define CONFIG_SYS_SDRAM_REFRESH		3  /* 0=OFF 1=2048 2=4096 3=8192.. refresh */
#define CONFIG_SYS_SDRAM_XSR_DELAY		113 /* ns Exit self refresh to any command */
#define CONFIG_SYS_SDRAM_EXIT_PWD		10 /* ns exit power down delay */
#define CONFIG_SYS_SDRAM_W2R_DELAY		1  /* WRITE to READ Command Delay 0=1ck 1=2ck */
#define CONFIG_SYS_SDRAM_ROW_PRECHARGE_DELAY	15 /* ns (>2 x ddr_clk)*/
#define CONFIG_SYS_SDRAM_TMRD_DELAY		2  /* Load mode register cycle delay 1..4 */
#define CONFIG_SYS_SDRAM_TWR_DELAY		1  /* DDR: 0=2ck 1=3ck */
#define CONFIG_SYS_SDRAM_RAS_DELAY		40 /* ns ACTIVE-to-PRECHARGE delay */
#define CONFIG_SYS_SDRAM_RRD_DELAY		10 /* ns ACTIVE-to-ACTIVE delay */
#define CONFIG_SYS_SDRAM_RCD_DELAY		15 /* ns Row to Column delay */
#define CONFIG_SYS_SDRAM_RC_DELAY		55 /* ns Row cycle delay (tRFC refresh to command)*/
#define CONFIG_SYS_SDRAM_RFC_DELAY		72 /* ns (> 10xddr_clk) auto refresh to any command*/
#define CONFIG_SYS_SDRAM_PARTIAL_ARRAY_SR	0  /* 0=full 1=half 2=quater 3=Eighth 4=Sixteenth */
#define CONFIG_SYS_SDRAM_DRIVE_STRENGH		0  /* 0=Full-strength 1=half 2=quater 3=Eighth */
#define CONFIG_SYS_SDRAM_BURST_LENGTH		3  /* 2^N BYTES (N=0..3) */
#define CONFIG_SYS_SDRAM_SINGLE_ACCESS		0  /* 1= single access; 0 = Burst mode */
#endif

/* Issue Mode register Command to SDRAM*/
#define CONFIG_SYS_DRR_MR_VAL ( 0x0\
	| (((CONFIG_SYS_SDRAM_BURST_LENGTH)&0x7)<<(0))\
	| ((3)<<(4)) \
	| (((CONFIG_SYS_SDRAM_SINGLE_ACCESS)&0x1)<<(3)))

/* Issue Extended Mode register Command to SDRAM*/
#define CONFIG_SYS_DRR_EMR_VAL\
	(((CONFIG_SYS_SDRAM_PARTIAL_ARRAY_SR&0x7)<<0)\
	| ((CONFIG_SYS_SDRAM_DRIVE_STRENGH&0x7)<<(5)))

#define CONFIG_SYS_SDRAM_ESDGPR_REGISTER_VAL	0x20020000
#define CONFIG_SYS_SDRAM_ESDCDLY1_REGISTER_VAL	0x00F48000
#define CONFIG_SYS_SDRAM_ESDCDLY2_REGISTER_VAL	0x00F48000
#define CONFIG_SYS_SDRAM_ESDCDLY3_REGISTER_VAL	0x00F48000
#define CONFIG_SYS_SDRAM_ESDCDLY4_REGISTER_VAL	0x00F48000
#define CONFIG_SYS_SDRAM_ESDCDLY5_REGISTER_VAL	0x00F48000

/* SDRAM IO Muxing */
#define CONFIG_IOMUX_PAD_DRAM_RAS	PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST /* imx default */	
#define CONFIG_IOMUX_PAD_DRAM_CAS	PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST /* imx default */
#define CONFIG_IOMUX_PAD_DRAM_SDWE	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST /* imx default */
#define CONFIG_IOMUX_PAD_DRAM_SDCKE0	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST /* imx default */
#define CONFIG_IOMUX_PAD_DRAM_SDCKE1	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST /* imx default */
#define CONFIG_IOMUX_PAD_DRAM_SDCLK	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU \
					+PAD_CTL_DRV_MAX+PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_PAD_DRAM_SDQS0	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PD \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_PAD_DRAM_SDQS1	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PD \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_PAD_DRAM_SDQS2	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PD \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_PAD_DRAM_SDQS3	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PD \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_PAD_DRAM_CS0	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST /* imx default */
#define CONFIG_IOMUX_PAD_DRAM_CS1	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU \
					+PAD_CTL_DRV_HIGH		      /* imx default */
#define CONFIG_IOMUX_PAD_DRAM_DQM0	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_PAD_DRAM_DQM1	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_PAD_DRAM_DQM2	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_PAD_DRAM_DQM3	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU \
					+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW

#define CONFIG_IOMUX_PAD_EIM_SDBA2	PAD_CTL_DRV_VOT_HIGH+PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL \
					+PAD_CTL_100K_PU	      /* imx default */
#define CONFIG_IOMUX_PAD_EIM_SDODT1	PAD_CTL_DRV_VOT_HIGH+PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL \
					+PAD_CTL_100K_PD+PAD_CTL_DRV_LOW+PAD_CTL_SRE_FAST /* imx default */
#define CONFIG_IOMUX_PAD_EIM_SDODT0	PAD_CTL_DRV_VOT_HIGH+PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL \
					+PAD_CTL_100K_PD+PAD_CTL_DRV_LOW+PAD_CTL_SRE_FAST /* imx default */

#define CONFIG_IOMUX_GRP_DDRPKS		PAD_CTL_PUE_PULL	/* imx default */
#define CONFIG_IOMUX_GRP_DRAM_B4	PAD_CTL_DRV_HIGH	/* imx default */
#define CONFIG_IOMUX_GRP_INDDR		PAD_CTL_DDR_INPUT_CMOS	/* imx default */
#define CONFIG_IOMUX_GRP_PKEDDR		PAD_CTL_PKE_ENABLE	/* imx default */
#define CONFIG_IOMUX_GRP_DDR_A0		PAD_CTL_DRV_HIGH	/* imx default */
#define CONFIG_IOMUX_GRP_DDR_A1		PAD_CTL_DRV_MEDIUM	/* imx default */
#define CONFIG_IOMUX_GRP_DDRAPUS	PAD_CTL_100K_PU		/* imx default */
#define CONFIG_IOMUX_GRP_HYSDDR0	PAD_CTL_HYS_NONE	/* imx default */
#define CONFIG_IOMUX_GRP_HYSDDR1	PAD_CTL_HYS_NONE	/* imx default */
#define CONFIG_IOMUX_GRP_HYSDDR2	PAD_CTL_HYS_NONE	/* imx default */
#define CONFIG_IOMUX_GRP_HVDDR		PAD_CTL_DRV_VOT_HIGH	/* imx default */
#define CONFIG_IOMUX_GRP_HYSDDR3	PAD_CTL_HYS_NONE	/* imx default */
#define CONFIG_IOMUX_GRP_DRAM_SR_B0	PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_GRP_DDRAPKS	PAD_CTL_PUE_PULL	/* imx default */

#define CONFIG_IOMUX_GRP_DRAM_SR_B1	PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_GRP_DDRPUS		PAD_CTL_100K_PU		/* imx default */
#define CONFIG_IOMUX_GRP_DRAM_SR_B2	PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_GRP_PKEADDR	PAD_CTL_PKE_ENABLE	/* imx default */
#define CONFIG_IOMUX_GRP_DRAM_SR_B4	PAD_CTL_SRE_SLOW

#define CONFIG_IOMUX_GRP_INMODE1	PAD_CTL_DDR_INPUT_CMOS	/* imx default */
#define CONFIG_IOMUX_GRP_DRAM_B0	PAD_CTL_DRV_HIGH	/* imx default */
#define CONFIG_IOMUX_GRP_DRAM_B1	PAD_CTL_DRV_HIGH	/* imx default */

#define CONFIG_IOMUX_GRP_DDR_SR_A0	PAD_CTL_SRE_FAST	/* imx default */

#define CONFIG_IOMUX_GRP_DRAM_B2	PAD_CTL_DRV_HIGH	/* imx default */

#define CONFIG_IOMUX_GRP_DDR_SR_A1	PAD_CTL_SRE_FAST	/* imx default */
/* end of SDRAM IO Muxing */

#else /* use DDR2 */

#if (CONFIG_SYS_SDRAM_MBYTE_SYZE == 256)
/* micron 256MB */
#define CONFIG_SYS_SDRAM_NUM_COL		10  /* 8, 9, 10 or 11 column address bits */
#define CONFIG_SYS_SDRAM_NUM_ROW		13 /* 11, 12 or 13 row address bits */
#define CONFIG_SYS_SDRAM_REFRESH		3  /* 0=OFF 1=2048 2=4096 3=8192.. refresh */
#define CONFIG_SYS_SDRAM_XSR_DELAY		140 /* ns Exit self refresh to any command */
#define CONFIG_SYS_SDRAM_EXIT_PWD		10 /* ns exit power down delay */
#define CONFIG_SYS_SDRAM_W2R_DELAY		1  /* WRITE to READ Command Delay 0=1ck 1=2ck  */
#define CONFIG_SYS_SDRAM_ROW_PRECHARGE_DELAY	15 /* ns (>2 x ddr_clk)*/
#define CONFIG_SYS_SDRAM_TMRD_DELAY		2  /* Load mode register cycle delay 1..4 */
#define CONFIG_SYS_SDRAM_TWR_DELAY		1  /* DDR: 0=2ck 1=3ck */
#define CONFIG_SYS_SDRAM_RAS_DELAY		40 /* ns ACTIVE-to-PRECHARGE delay */
#define CONFIG_SYS_SDRAM_RRD_DELAY		10 /* ns ACTIVE-to-ACTIVE delay */
#define CONFIG_SYS_SDRAM_RCD_DELAY		15 /* ns Row to Column delay */
#define CONFIG_SYS_SDRAM_RC_DELAY		55 /* ns Row cycle delay (tRFC refresh to command)*/
#define CONFIG_SYS_SDRAM_RFC_DELAY		130 /* ns (> 10xddr_clk) auto refresh to any command */
#define CONFIG_SYS_SDRAM_BURST_LENGTH		2  /* 2^N BYTES (N=0..3) */
#define CONFIG_SYS_SDRAM_SINGLE_ACCESS		0  /* 1= single access; 0 = Burst mode */
#endif

/* Issue Mode register Command to SDRAM*/
#define CONFIG_SYS_DRR_MR_VAL ( 0x0\
	| (((CONFIG_SYS_SDRAM_BURST_LENGTH)&0x7)<<(0))\
	| ((3)<<(4)) /* CAS latency imx51=3 */ \
	| (((CONFIG_SYS_SDRAM_TWR_DELAY+1)&0x7)<<(9)) \
	| (((CONFIG_SYS_SDRAM_SINGLE_ACCESS)&0x1)<<(3)))


/* Issue Extended Mode register Command to SDRAM*/
#define CONFIG_SYS_DRR_EMR_VAL		0x040 /* 150ohms */ 

#define CONFIG_SYS_SDRAM_ESDGPR_REGISTER_VAL 0x90000000
#define CONFIG_SYS_SDRAM_ESDCDLY1_REGISTER_VAL 0x00048000
#define CONFIG_SYS_SDRAM_ESDCDLY2_REGISTER_VAL 0x000e8000
#define CONFIG_SYS_SDRAM_ESDCDLY3_REGISTER_VAL 0x00ff8000
#define CONFIG_SYS_SDRAM_ESDCDLY4_REGISTER_VAL 0x00fa8000
#define CONFIG_SYS_SDRAM_ESDCDLY5_REGISTER_VAL 0x00ed8000

/* SDRAM IO Muxing */
#define CONFIG_IOMUX_PAD_DRAM_RAS	PAD_CTL_SRE_FAST+PAD_CTL_DRV_MEDIUM	
#define CONFIG_IOMUX_PAD_DRAM_CAS	PAD_CTL_SRE_FAST+PAD_CTL_DRV_MEDIUM
#define CONFIG_IOMUX_PAD_DRAM_SDWE	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU+PAD_CTL_DRV_MEDIUM+PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_PAD_DRAM_SDCKE0	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU+PAD_CTL_DRV_MEDIUM+PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_PAD_DRAM_SDCKE1	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU+PAD_CTL_DRV_MEDIUM+PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_PAD_DRAM_SDCLK	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU+PAD_CTL_DRV_MAX+PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_PAD_DRAM_SDQS0	PAD_CTL_PKE_NONE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PD+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_PAD_DRAM_SDQS1	PAD_CTL_PKE_NONE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PD+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_PAD_DRAM_SDQS2	PAD_CTL_PKE_NONE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PD+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_PAD_DRAM_SDQS3	PAD_CTL_PKE_NONE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PD+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_PAD_DRAM_CS0	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU+PAD_CTL_DRV_MEDIUM+PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_PAD_DRAM_CS1	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU+PAD_CTL_DRV_MEDIUM+PAD_CTL_SRE_SLOW

#define CONFIG_IOMUX_PAD_DRAM_DQM0	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_PAD_DRAM_DQM1	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_PAD_DRAM_DQM2	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW
#define CONFIG_IOMUX_PAD_DRAM_DQM3	PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_SLOW

#define CONFIG_IOMUX_PAD_EIM_SDBA2	PAD_CTL_DRV_VOT_HIGH+PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL+PAD_CTL_100K_PU /* imx default */
#define CONFIG_IOMUX_PAD_EIM_SDODT1	PAD_CTL_DRV_VOT_HIGH+PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL \
					+PAD_CTL_100K_PD+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_PAD_EIM_SDODT0	PAD_CTL_DRV_VOT_HIGH+PAD_CTL_PKE_ENABLE+PAD_CTL_PUE_PULL \
					+PAD_CTL_100K_PD+PAD_CTL_DRV_HIGH+PAD_CTL_SRE_FAST

#define CONFIG_IOMUX_GRP_DDRPKS		PAD_CTL_PUE_KEEPER
#define CONFIG_IOMUX_GRP_DRAM_B4	PAD_CTL_DRV_MAX
#define CONFIG_IOMUX_GRP_INDDR		PAD_CTL_DDR_INPUT_CMOS /* imx default */
#define CONFIG_IOMUX_GRP_PKEDDR		PAD_CTL_PKE_ENABLE
#define CONFIG_IOMUX_GRP_DDR_A0		PAD_CTL_DRV_MEDIUM
#define CONFIG_IOMUX_GRP_DDR_A1		PAD_CTL_DRV_MEDIUM
#define CONFIG_IOMUX_GRP_DDRAPUS	PAD_CTL_100K_PU
#define CONFIG_IOMUX_GRP_HYSDDR0	PAD_CTL_HYS_NONE
#define CONFIG_IOMUX_GRP_HYSDDR1	PAD_CTL_HYS_NONE
#define CONFIG_IOMUX_GRP_HYSDDR2	PAD_CTL_HYS_NONE
#define CONFIG_IOMUX_GRP_HVDDR		PAD_CTL_DRV_VOT_HIGH
#define CONFIG_IOMUX_GRP_HYSDDR3	PAD_CTL_HYS_NONE
#define CONFIG_IOMUX_GRP_DRAM_SR_B0	PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_GRP_DDRAPKS	PAD_CTL_PUE_PULL

#define CONFIG_IOMUX_GRP_DRAM_SR_B1	PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_GRP_DDRPUS		PAD_CTL_100K_PU
#define CONFIG_IOMUX_GRP_DRAM_SR_B2	PAD_CTL_SRE_FAST
#define CONFIG_IOMUX_GRP_PKEADDR	PAD_CTL_PKE_ENABLE
#define CONFIG_IOMUX_GRP_DRAM_SR_B4	PAD_CTL_SRE_FAST

#define CONFIG_IOMUX_GRP_INMODE1	PAD_CTL_DDR_INPUT_DDR
#define CONFIG_IOMUX_GRP_DRAM_B0	PAD_CTL_DRV_MAX
#define CONFIG_IOMUX_GRP_DRAM_B1	PAD_CTL_DRV_MAX

#define CONFIG_IOMUX_GRP_DDR_SR_A0	PAD_CTL_SRE_FAST

#define CONFIG_IOMUX_GRP_DRAM_B2	PAD_CTL_DRV_MAX

#define CONFIG_IOMUX_GRP_DDR_SR_A1	PAD_CTL_SRE_FAST
/* end of SDRAM IO Muxing */

#endif /* CONFIG_SYS_SDRAM_TYPE_MDDR */

/* CCM Configuration */

#ifndef CONFIG_SYS_MX51_HCLK_FREQ
#define CONFIG_SYS_PLL_DP_CTL_VAL	0x00001332	/* use fpm_clk */
#else
#define CONFIG_SYS_PLL_DP_CTL_VAL	0x00001232 	/* use osc_clk */
#endif

#ifndef CONFIG_SYS_MX51_HCLK_FREQ
#define CONFIG_SYS_CCR_VAL 		0x00001FFF
#define CONFIG_SYS_CSR_VAL 		0x0000003E
#define CONFIG_SYS_CCSR_VAL 		0x00000200
#else
#define CONFIG_SYS_CCR_VAL 		0x000009FF
#define CONFIG_SYS_CSR_VAL 		0x00000032
#define CONFIG_SYS_CCSR_VAL 		0x00000000
#endif

#define CCBDR_MASK 0x40000000

#define CONFIG_SYS_CBCDR_VAL\
	(CCBDR_MASK 							\
	|((((CONFIG_PLL1_FREQ/CONFIG_DDR_CLK_FREQ)-1)&0x07)<<27)	\
	|((((CONFIG_PLL2_FREQ/CONFIG_EMI_CLK_FREQ)-1)&0x07)<<22)	\
	|((((CONFIG_PLL2_FREQ/CONFIG_AXIB_CLK_FREQ)-1)&0x07)<<19)	\
	|((((CONFIG_PLL2_FREQ/CONFIG_AXIA_CLK_FREQ)-1)&0x07)<<16)	\
	|((((CONFIG_EMI_CLK_FREQ/CONFIG_NFC_FREQ)-1)&0x07)<<13)		\
	|((((CONFIG_PLL2_FREQ/CONFIG_AHB_CLK_FREQ)-1)&0x07)<<10)	\
	|((((CONFIG_AHB_CLK_FREQ/CONFIG_IPG_CLK_FREQ)-1)&0x03)<<8)	\
	|(((((CONFIG_PLL2_FREQ/CONFIG_PERCLK_FREQ)/8))&0x03)<<6)	\
	|(((((CONFIG_PLL2_FREQ/CONFIG_PERCLK_FREQ)			\
	 /((CONFIG_PLL2_FREQ/CONFIG_PERCLK_FREQ)/8+1))-1)&0x07)<<0)	\
	)

#define CONFIG_SYS_CBCMR_VAL 0x000020C0

#define CONFIG_SYS_CBCMR1_VAL 0xA5A2A020

#define CONFIG_SYS_CSCDR1_VAL\
	( 0x0								\
	|((((CONFIG_PLL3_FREQ/CONFIG_ESDHC_FREQ)/8)&0x07)<<19)		\
	|(((((CONFIG_PLL3_FREQ/CONFIG_ESDHC_FREQ)			\
	 /((CONFIG_PLL3_FREQ/CONFIG_ESDHC_FREQ)/8+1))-1)&0x07)<<22)	\
	|((((CONFIG_PLL3_FREQ/CONFIG_ESDHC_FREQ)/8)&0x07)<<11)		\
	|((((CONFIG_IPG_CLK_FREQ/CONFIG_PGC_FREQ)-1)&0x07)<<14)		\
	|(((((CONFIG_PLL3_FREQ/CONFIG_ESDHC_FREQ)			\
	 /((CONFIG_PLL3_FREQ/CONFIG_ESDHC_FREQ)/8+1))-1)&0x07)<<16)	\
	|((((CONFIG_PLL3_FREQ/CONFIG_USB_FREQ)/4)&0x07)<<6)		\
	|(((((CONFIG_PLL3_FREQ/CONFIG_USB_FREQ)				\
	 /((CONFIG_PLL3_FREQ/CONFIG_USB_FREQ)/4+1))-1)&0x03)<<8)	\
	|((((CONFIG_PLL2_FREQ/CONFIG_UART_FREQ)/8)&0x07)<<0)		\
	|(((((CONFIG_PLL2_FREQ/CONFIG_UART_FREQ)			\
	 /((CONFIG_PLL2_FREQ/CONFIG_UART_FREQ)/8+1))-1)&0x07)<<3)	\
	)

/* clock cycle time with x2 for a 0.5ns accuracy */
#define CONFIG_SYS_2XDDRCLK_LGTH	(2000/CONFIG_DDR_CLK_FREQ)	/* ns */

#ifdef CONFIG_SYS_SDRAM_TYPE_MDDR
#define CONFIG_SYS_SDRAM_ESDMISC_REGISTER_VAL 0x000a5680
#else
#define CONFIG_SYS_SDRAM_ESDMISC_REGISTER_VAL 0x000ad6d0
#endif

#if ((2*CONFIG_SYS_SDRAM_RC_DELAY)>(16*CONFIG_SYS_2XDDRCLK_LGTH))
#define REG_FIELD_RC_VAL 0
#else
#define REG_FIELD_RC_VAL \
	(((2*CONFIG_SYS_SDRAM_RC_DELAY+CONFIG_SYS_2XDDRCLK_LGTH-1)/CONFIG_SYS_2XDDRCLK_LGTH)-1)
#endif

#if ((2*CONFIG_SYS_SDRAM_RFC_DELAY)>(25*CONFIG_SYS_2XDDRCLK_LGTH))
#define REG_FIELD_RFC_VAL \
	(((2*CONFIG_SYS_SDRAM_RFC_DELAY+CONFIG_SYS_2XDDRCLK_LGTH-1)/CONFIG_SYS_2XDDRCLK_LGTH/2)-10)
#define REG_FIELD_ESDCTL_PAT 0x80820000
#else
#define REG_FIELD_RFC_VAL \
	(((2*CONFIG_SYS_SDRAM_RFC_DELAY+CONFIG_SYS_2XDDRCLK_LGTH-1)/CONFIG_SYS_2XDDRCLK_LGTH)-10)
#define REG_FIELD_ESDCTL_PAT 0x80020000
#endif

/* ESDCTLx Configuration Registers: 32bits */
#define CONFIG_SYS_SDRAM_ESDCTL_REGISTER_VAL (REG_FIELD_ESDCTL_PAT\
	| (((CONFIG_SYS_SDRAM_REFRESH)&0x0F)<<28)\
	| (((CONFIG_SYS_SDRAM_NUM_ROW-11)&0x7)<<24)\
	| (((CONFIG_SYS_SDRAM_NUM_COL-8)&0x3)<<20))


#define CONFIG_SYS_SDRAM_ESDCFG_REGISTER_VAL (0x00000000\
	| ((REG_FIELD_RFC_VAL&0x0F)<<28)\
	| (((((2*CONFIG_SYS_SDRAM_XSR_DELAY+CONFIG_SYS_2XDDRCLK_LGTH-1)/CONFIG_SYS_2XDDRCLK_LGTH)-25)&0x0F)<<24)\
	| (((((2*CONFIG_SYS_SDRAM_EXIT_PWD+CONFIG_SYS_2XDDRCLK_LGTH-1)/CONFIG_SYS_2XDDRCLK_LGTH)-1)&0x7)<<20)\
	| ((CONFIG_SYS_SDRAM_W2R_DELAY)<<20)\
	| (((((2*CONFIG_SYS_SDRAM_ROW_PRECHARGE_DELAY+CONFIG_SYS_2XDDRCLK_LGTH-1)/CONFIG_SYS_2XDDRCLK_LGTH)-2)&0x3)<<18)\
	| (((CONFIG_SYS_SDRAM_TMRD_DELAY-1)&0x3)<<16)\
	| (((((2*CONFIG_SYS_SDRAM_RAS_DELAY+CONFIG_SYS_2XDDRCLK_LGTH-1)/CONFIG_SYS_2XDDRCLK_LGTH)-1)&0x0F)<<12)\
	| (((((2*CONFIG_SYS_SDRAM_RRD_DELAY+CONFIG_SYS_2XDDRCLK_LGTH-1)/CONFIG_SYS_2XDDRCLK_LGTH)-1)&0x3)<<10)\
	| (((CONFIG_SYS_SDRAM_TWR_DELAY)&0x1)<<7)\
	| (((((2*CONFIG_SYS_SDRAM_RCD_DELAY+CONFIG_SYS_2XDDRCLK_LGTH-1)/CONFIG_SYS_2XDDRCLK_LGTH)-1)&0x7)<<4)\
	| (((REG_FIELD_RC_VAL)&0x0F)<<0))

#endif				/* __CONFIG_H */

