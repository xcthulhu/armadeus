/*
 * imx51regs.h - i.MX51 registers definition
 *
 * Copyright (C) 2010 armadeus systems
 * Derivated from pxaregs (c) Copyright 2002 by M&N Logistik-Loesungen Online GmbH
 * Author: Julien Boibessot
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "definitions.h"

#define IMX_TYPE "i.MX51"

static struct reg_info regs[] =
{
/* CCM */
{ "CCR",	0x73fd4000, 0, 0xffffffff, 'x', "CCM Control Register" },
{ "CCDR",	0x73fd4004, 0, 0xffffffff, 'x', "CCM Control Divider Register" },
{ "CSR",	0x73fd4008, 0, 0xffffffff, 'x', "CCM Status Register" },
{ "CCSR",	0x73fd400c, 0, 0xffffffff, 'x', "CCM Clock Switcher Register" },
{ "CACRR",	0x73fd4010, 0, 0xffffffff, 'x', "CCM Arm Clock Root Register" },
{ "CBCDR",	0x73fd4014, 0, 0xffffffff, 'x', "CCM Bus Clock Divider Register" },
{ "CBCMR",	0x73fd4018, 0, 0xffffffff, 'x', "CCM Bus Clock Multiplexer Register" },
{ "CSCMR1",	0x73fd401c, 0, 0xffffffff, 'x', "CCM Serial Clock Multiplexer Register 1" },
{ "CSCMR2",	0x73fd4020, 0, 0xffffffff, 'x', "CCM Serial Clock Multiplexer Register 2" },
{ "CSCDR1",	0x73fd4024, 0, 0xffffffff, 'x', "CCM Serial Clock Divider Register" },
{ "CS1CDR",	0x73fd4028, 0, 0xffffffff, 'x', "CCM SSI1 Clock Divider Register" },
{ "CS2CDR",	0x73fd402c, 0, 0xffffffff, 'x', "CCM SSI2 Clock Divider Register" },
{ "CDCDR",	0x73fd4030, 0, 0xffffffff, 'x', "CCM DI Clock Divider Register" },
{ "CSCDR2",	0x73fd4038, 0, 0xffffffff, 'x', "CCM Serial Clock Divider Register" },
{ "CSCDR3",	0x73fd403c, 0, 0xffffffff, 'x', "CCM Serial Clock Divider Register" },
{ "CSCDR4",	0x73fd4040, 0, 0xffffffff, 'x', "CCM Serial Clock Divider Register" },
{ "CWDR",	0x73fd4044, 0, 0xffffffff, 'x', "CCM Wakeup Detector Register" },
{ "CDHIPR",	0x73fd4048, 0, 0xffffffff, 'x', "CCM Divider Handshake In-Process Register" },
{ "CDCR",	0x73fd404c, 0, 0xffffffff, 'x', "CCM DVFS Control Register" },
{ "CTOR",	0x73fd4050, 0, 0xffffffff, 'x', "CCM Testing Observability Register" },
{ "CLPCR",	0x73fd4054, 0, 0xffffffff, 'x', "CCM Low Power Control Register" },
{ "CISR",	0x73fd4058, 0, 0xffffffff, 'x', "CCM Interrupt Status Register" },
{ "CIMR",	0x73fd405c, 0, 0xffffffff, 'x', "CCM Interrupt Mask Register" },
{ "CCOSR",	0x73fd4060, 0, 0xffffffff, 'x', "CCM Clock Output Source Register" },
{ "CGPR",	0x73fd4064, 0, 0xffffffff, 'x', "CCM General Purpose Register" },
{ "CCGR0",	0x73fd4068, 0, 0xffffffff, 'x', "CCM Clock Gating Register 0" },
{ "CCGR1",	0x73fd406c, 0, 0xffffffff, 'x', "CCM Clock Gating Register 1" },
{ "CCGR2",	0x73fd4070, 0, 0xffffffff, 'x', "CCM Clock Gating Register 2" },
{ "CCGR3",	0x73fd4074, 0, 0xffffffff, 'x', "CCM Clock Gating Register 3" },
{ "CCGR4",	0x73fd4078, 0, 0xffffffff, 'x', "CCM Clock Gating Register 4" },
{ "CCGR5",	0x73fd407c, 0, 0xffffffff, 'x', "CCM Clock Gating Register 5" },
{ "CCGR6",	0x73fd4080, 0, 0xffffffff, 'x', "CCM Clock Gating Register 6" },
{ "CMEOR",	0x73fd4084, 0, 0xffffffff, 'x', "CCM Module Enable Override Register" },
/* GPT */
{ "GPTCR_1",		0x73fa0000, 0, 0xffffffff, 'x', "GPT1 Control Register" },
{ "GPTPR_1",		0x73fa0004, 0, 0xffffffff, 'x', "GPT1 Prescaler Register" },
{ "GPTSR_1",		0x73fa0008, 0, 0xffffffff, 'x', "GPT1 Status Register" },
{ "GPTIR_1",		0x73fa000c, 0, 0xffffffff, 'x', "GPT1 Interrupt Register" },
{ "GPTOCR1_1",		0x73fa0010, 0, 0xffffffff, 'x', "GPT1 Output Compare Register 1" },
{ "GPTOCR2_1",		0x73fa0014, 0, 0xffffffff, 'x', "GPT1 Output Compare Register 2" },
{ "GPTOCR3_1",		0x73fa0018, 0, 0xffffffff, 'x', "GPT1 Output Compare Register 3" },
{ "GPTICR1_1",		0x73fa001c, 0, 0xffffffff, 'x', "GPT1 Input capture Register 1" },
{ "GPTICR2_1",		0x73fa0020, 0, 0xffffffff, 'x', "GPT1 Input capture Register 2" },
{ "GPTCNT_1",		0x73fa0024, 0, 0xffffffff, 'x', "GPT1 Counter Register" },
/* I2C1 */
{ "IADR_1",	0x83fc8000, 0, 0xffffffff, 'x', "I2C1 Address Register" },
{ "IFDR_1",	0x83fc8004, 0, 0xffffffff, 'x', "I2C1 Frequency Divider Register" },
{ "I2CR_1",	0x83fc8008, 0, 0xffffffff, 'x', "I2C1 Control Register" },
{ "I2SR_1",	0x83fc800c, 0, 0xffffffff, 'x', "I2C1 Status Register" },
{ "I2DR_1",	0x83fc8010, 0, 0xffffffff, 'x', "I2C1 Data I/O Register" },
/* I2C2 */
{ "IADR_2",	0x83fc4000, 0, 0xffffffff, 'x', "I2C2 Address Register" },
{ "IFDR_2",	0x83fc4004, 0, 0xffffffff, 'x', "I2C2 Frequency Divider Register" },
{ "I2CR_2",	0x83fc4008, 0, 0xffffffff, 'x', "I2C2 Control Register" },
{ "I2SR_2",	0x83fc400c, 0, 0xffffffff, 'x', "I2C2 Status Register" },
{ "I2DR_2",	0x83fc4010, 0, 0xffffffff, 'x', "I2C2 Data I/O Register" },
/* FEC */
{ "EIR",     0x83fec004, 0, 0xffffffff, 'x', "FEC Interrupt Event Register" },
/* AUDMUX */
{ "PTCR1",	0x83fd0000, 0, 0xffffffff, 'x', "Port Timing Control Register 1" },
{ "PDCR1",	0x83fd0004, 0, 0xffffffff, 'x', "Port Data Control Register 1" },
{ "PTCR2",	0x83fd0008, 0, 0xffffffff, 'x', "Port Timing Control Register 2" },
{ "PDCR2",	0x83fd000C, 0, 0xffffffff, 'x', "Port Data Control Register 2" },
{ "PTCR3",	0x83fd0010, 0, 0xffffffff, 'x', "Port Timing Control Register 3" },
{ "PDCR3",	0x83fd0014, 0, 0xffffffff, 'x', "Port Data Control Register 3" },
{ "PTCR4",	0x83fd0018, 0, 0xffffffff, 'x', "Port Timing Control Register 4" },
{ "PDCR4",	0x83fd001C, 0, 0xffffffff, 'x', "Port Data Control Register 4" },
{ "PTCR5",	0x83fd0020, 0, 0xffffffff, 'x', "Port Timing Control Register 5" },
{ "PDCR5",	0x83fd0024, 0, 0xffffffff, 'x', "Port Data Control Register 5" },
{ "PTCR6",	0x83fd0028, 0, 0xffffffff, 'x', "Port Timing Control Register 6" },
{ "PDCR6",	0x83fd002C, 0, 0xffffffff, 'x', "Port Data Control Register 6" },
{ "PTCR7",	0x83fd0030, 0, 0xffffffff, 'x', "Port Timing Control Register 7" },
{ "PDCR7",	0x83fd0034, 0, 0xffffffff, 'x', "Port Data Control Register 7" },
/* SSI 1 */
{ "STX0_1",	0x83fcc000, 0, 0xffffffff, 'x', "SSI1 Transmit Data Register 0" },
{ "STX1_1",	0x83fcc004, 0, 0xffffffff, 'x', "SSI1 Transmit Data Register 1" },
{ "SRX0_1",	0x83fcc008, 0, 0xffffffff, 'x', "SSI1 Receive Data Register 0" },
{ "SRX1_1",	0x83fcc00c, 0, 0xffffffff, 'x', "SSI1 Receive Data Register 1" },
{ "SCR_1",	0x83fcc010, 0, 0xffffffff, 'x', "SSI1 Control Register" },
{ "SISR_1",	0x83fcc014, 0, 0xffffffff, 'x', "SSI1 Interrupt Status Register" },
{ "SIER_1",	0x83fcc018, 0, 0xffffffff, 'x', "SSI1 Interrupt Enable Register" },
{ "STCR_1",	0x83fcc01c, 0, 0xffffffff, 'x', "SSI1 Transmit Configuration Register" },
{ "SRCR_1",	0x83fcc020, 0, 0xffffffff, 'x', "SSI1 Receive Configuration Register" },
{ "STCCR_1",	0x83fcc024, 0, 0xffffffff, 'x', "SSI1 Transmit Clock Control Register" },
{ "SRCCR_1",	0x83fcc028, 0, 0xffffffff, 'x', "SSI& Receive Clock Control Register" },
{ "SFCSR_1",	0x83fcc02c, 0, 0xffffffff, 'x', "SSI1 FIFO Control/Status Register" },
{ "STR_1",	0x83fcc030, 0, 0xffffffff, 'x', "SSI1 Test Register1" },
{ "SOR_1",	0x83fcc034, 0, 0xffffffff, 'x', "SSI1 Option Register2" },
{ "SACNT_1",	0x83fcc038, 0, 0xffffffff, 'x', "SSI1 AC97 Control Register" },
{ "SACADD_1",	0x83fcc03c, 0, 0xffffffff, 'x', "SSI1 AC97 Command Address Register" },
{ "SACDAT_1",	0x83fcc040, 0, 0xffffffff, 'x', "SSI1 AC97 Command Data Register" },
{ "SATAG_1",	0x83fcc044, 0, 0xffffffff, 'x', "SSI1 AC97 Tag Register" },
{ "STMSK_1",	0x83fcc048, 0, 0xffffffff, 'x', "SSI1 Transmit Time Slot Mask Register" },
{ "SRMSK_1",	0x83fcc04c, 0, 0xffffffff, 'x', "SSI1 Receive Time Slot Mask Register" },
{ "SACCST_1",	0x83fcc050, 0, 0xffffffff, 'x', "SSI1 AC97 Channel Status Register" },
{ "SACCEN_1",	0x83fcc054, 0, 0xffffffff, 'x', "SSI1 AC97 Channel Enable Register" },
{ "SACCDIS_1",	0x83fcc058, 0, 0xffffffff, 'x', "SSI1 AC97 Channel Disable Register" },
/* SSI 2 */
{ "STX0_2",	0x70014000, 0, 0xffffffff, 'x', "SSI2 Transmit Data Register 0" },
{ "STX1_2",	0x70014004, 0, 0xffffffff, 'x', "SSI2 Transmit Data Register 1" },
{ "SRX0_2",	0x70014008, 0, 0xffffffff, 'x', "SSI2 Receive Data Register 0" },
{ "SRX1_2",	0x7001400c, 0, 0xffffffff, 'x', "SSI2 Receive Data Register 1" },
{ "SCR_2",	0x70014010, 0, 0xffffffff, 'x', "SSI2 Control Register" },
{ "SISR_2",	0x70014014, 0, 0xffffffff, 'x', "SSI2 Interrupt Status Register" },
{ "SIER_2",	0x70014018, 0, 0xffffffff, 'x', "SSI2 Interrupt Enable Register" },
{ "STCR_2",	0x7001401c, 0, 0xffffffff, 'x', "SSI2 Transmit Configuration Register" },
{ "SRCR_2",	0x70014020, 0, 0xffffffff, 'x', "SSI2 Receive Configuration Register" },
{ "STCCR_2",	0x70014024, 0, 0xffffffff, 'x', "SSI2 Transmit Clock Control Register" },
{ "SRCCR_2",	0x70014028, 0, 0xffffffff, 'x', "SSI2 Receive Clock Control Register" },
{ "SFCSR_2",	0x7001402c, 0, 0xffffffff, 'x', "SSI2 FIFO Control/Status Register" },
{ "STR_2",	0x70014030, 0, 0xffffffff, 'x', "SSI2 Test Register1" },
{ "SOR_2",	0x70014034, 0, 0xffffffff, 'x', "SSI2 Option Register2" },
{ "SACNT_2",	0x70014038, 0, 0xffffffff, 'x', "SSI2 AC97 Control Register" },
{ "SACADD_2",	0x7001403c, 0, 0xffffffff, 'x', "SSI2 AC97 Command Address Register" },
{ "SACDAT_2",	0x70014040, 0, 0xffffffff, 'x', "SSI2 AC97 Command Data Register" },
{ "SATAG_2",	0x70014044, 0, 0xffffffff, 'x', "SSI2 AC97 Tag Register" },
{ "STMSK_2",	0x70014048, 0, 0xffffffff, 'x', "SSI2 Transmit Time Slot Mask Register" },
{ "SRMSK_2",	0x7001404c, 0, 0xffffffff, 'x', "SSI2 Receive Time Slot Mask Register" },
{ "SACCST_2",	0x70014050, 0, 0xffffffff, 'x', "SSI2 AC97 Channel Status Register" },
{ "SACCEN_2",	0x70014054, 0, 0xffffffff, 'x', "SSI2 AC97 Channel Enable Register" },
{ "SACCDIS_2",	0x70014058, 0, 0xffffffff, 'x', "SSI2 AC97 Channel Disable Register" },
/* SSI 3 */
{ "STX0_3",	0x83fe8000, 0, 0xffffffff, 'x', "SSI3 Transmit Data Register 0" },
{ "STX1_3",	0x83fe8004, 0, 0xffffffff, 'x', "SSI3 Transmit Data Register 1" },
{ "SRX0_3",	0x83fe8008, 0, 0xffffffff, 'x', "SSI3 Receive Data Register 0" },
{ "SRX1_3",	0x83fe800c, 0, 0xffffffff, 'x', "SSI3 Receive Data Register 1" },
{ "SCR_3",	0x83fe8010, 0, 0xffffffff, 'x', "SSI3 Control Register" },
{ "SISR_3",	0x83fe8014, 0, 0xffffffff, 'x', "SSI3 Interrupt Status Register" },
{ "SIER_3",	0x83fe8018, 0, 0xffffffff, 'x', "SSI3 Interrupt Enable Register" },
{ "STCR_3",	0x83fe801c, 0, 0xffffffff, 'x', "SSI3 Transmit Configuration Register" },
{ "SRCR_3",	0x83fe8020, 0, 0xffffffff, 'x', "SSI3 Receive Configuration Register" },
{ "STCCR_3",	0x83fe8024, 0, 0xffffffff, 'x', "SSI3 Transmit Clock Control Register" },
{ "SRCCR_3",	0x83fe8028, 0, 0xffffffff, 'x', "SSI3 Receive Clock Control Register" },
{ "SFCSR_3",	0x83fe802c, 0, 0xffffffff, 'x', "SSI3 FIFO Control/Status Register" },
{ "STR_3",	0x83fe8030, 0, 0xffffffff, 'x', "SSI3 Test Register1" },
{ "SOR_3",	0x83fe8034, 0, 0xffffffff, 'x', "SSI3 Option Register2" },
{ "SACNT_3",	0x83fe8038, 0, 0xffffffff, 'x', "SSI3 AC97 Control Register" },
{ "SACADD_3",	0x83fe803c, 0, 0xffffffff, 'x', "SSI3 AC97 Command Address Register" },
{ "SACDAT_3",	0x83fe8040, 0, 0xffffffff, 'x', "SSI3 AC97 Command Data Register" },
{ "SATAG_3",	0x83fe8044, 0, 0xffffffff, 'x', "SSI3 AC97 Tag Register" },
{ "STMSK_3",	0x83fe8048, 0, 0xffffffff, 'x', "SSI3 Transmit Time Slot Mask Register" },
{ "SRMSK_3",	0x83fe804c, 0, 0xffffffff, 'x', "SSI3 Receive Time Slot Mask Register" },
{ "SACCST_3",	0x83fe8050, 0, 0xffffffff, 'x', "SSI3 AC97 Channel Status Register" },
{ "SACCEN_3",	0x83fe8054, 0, 0xffffffff, 'x', "SSI3 AC97 Channel Enable Register" },
{ "SACCDIS_3",	0x83fe8058, 0, 0xffffffff, 'x', "SSI3 AC97 Channel Disable Register" },
};
