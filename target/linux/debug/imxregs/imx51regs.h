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
};
