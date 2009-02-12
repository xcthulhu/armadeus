/*
**	THE ARMADEUS PROJECT
**
**  Copyright (C) year  The source forge armadeus project team
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
**	File : ad9889ctrl.h 
**
**	authors: thom25@users.sourceforge.net, jorasse@armadeus.org
*/ 
 

// I2C slave Adresses

#define AD9889_I2C_SLAVE_ADDR	0x39


// main registers address
#define CHIP_REVISION		0x00

#define EDID_ID			0x43

#define SECONDARY_DEVICE	0xCF

// secondary device address
#define GMP_HB0			0x00

#define GMP_PB27		0x1E



#define DEVICE_ID_CMD       0x00
#define REVISION_ID_CMD     0x01
#define PAGE_SELECT_CMD     0X02
#define RESET_CMD           0x03
#define POWER_STATE_CMD     0x04            
#define TV_HUE_CMD          0x05
#define TV_SAT_CMD          0x06
#define TV_CONTRAST_CMD     0x07
#define TV_BRIGHTNESS_CMD   0x08
#define TV_SHARPNESS_CMD    0x09
#define VIDEO_OUTPUT_FORMAT_CMD     0x0A
#define CRYSTAL_CMD                 0x0B
#define INPUT_DATA_FORMAT1_CMD      0x0C
#define INPUT_DATA_FORMAT2_CMD      0x0D
#define SYNC_CMD                    0x0E
#define TV_FILTER1_CMD              0x0F
#define TV_FILTER2_CMD              0x10
#define INPUT_TIMING1_CMD           0x11
#define INPUT_TIMING2_CMD           0x12
#define INPUT_TIMING3_CMD           0x13
#define INPUT_TIMING4_CMD           0x14
#define INPUT_TIMING5_CMD           0x15
#define INPUT_TIMING6_CMD           0x16
#define INPUT_TIMING7_CMD           0x17
#define INPUT_TIMING8_CMD           0x18
#define INPUT_TIMING9_CMD           0x19
#define INPUT_TIMING10_CMD          0x1A
#define INPUT_TIMING11_CMD          0x1B
#define BURST_AMPLITUDE_CMD         0x1C
#define CLOCK_TREE_CMD              0x1D
#define OUTPUT_TIMING1_CMD          0x1E
#define OUTPUT_TIMING2_CMD          0x1F
#define TV_VERTICAL_POS1_CMD        0x20
#define TV_VERTICAL_POS2_CMD        0x21
#define TV_HORIZONTAL_POS1_CMD      0x22
#define TV_HORIZONTAL_POS2_CMD      0x23
#define PCLK_CLOCK_DIVIDER1_CMD     0x24
#define PCLK_CLOCK_DIVIDER2_CMD     0x25
#define PCLK_CLOCK_DIVIDER3_CMD     0x26
#define PCLK_CLOCK_DIVIDER4_CMD     0x27
#define CLOCK_DIVIDER_NUM1_CMD      0x28
#define CLOCK_DIVIDER_NUM2_CMD      0x29
#define CLOCK_DIVIDER_NUM3_CMD      0x2A
#define CLOCK_DIVIDER_DEN1_CMD      0x2B
#define CLOCK_DIVIDER_DEN2_CMD      0x2C
#define CLOCK_DIVIDER_DEN3_CMD      0x2D
#define CLOCK_DIVIDER_INT_CMD       0x2E
#define PLL_RATIO1_CMD              0x2F
#define PLL_RATIO2_CMD              0x30
#define PLL_RATIO3_CMD              0x31
#define FSCI_ADJUSTMENT1_CMD        0x32
#define FSCI_ADJUSTMENT2_CMD        0x33
#define SUB_CARRIER_FREQ1_CMD       0x34
#define SUB_CARRIER_FREQ2_CMD       0x35
#define SUB_CARRIER_FREQ3_CMD       0x36
#define SUB_CARRIER_FREQ4_CMD       0x37
#define DAC_TRIMMING_CMD            0x62
#define DATA_IO_CMD                 0x63
#define ATTACHED_DISPLAY            0x7E
#define CBCR_INPUT_SWITCH_CMD       0x10   // second page


#define AD9889_MAX_REV		0x00
#define PAGE_SELECT_0               0
#define PAGE_SELECT_1               1
#define DAC_OUTPUT_OFF              (0<<4)
#define DAC_OUTPUT_CVBS             (1<<4)
#define DAC_OUTPUT_SVIDEO           (2<<4)
#define OUTPUT_FORMAT__NTSC_M       0
#define OUTPUT_FORMAT__NTSC_J       1
#define OUTPUT_FORMAT__NTSC_443     2
#define OUTPUT_FORMAT__PAL          3
#define OUTPUT_FORMAT__PAL_M        4
#define OUTPUT_FORMAT__PAL_N        5
#define OUTPUT_FORMAT__PAL_NC       6
#define OUTPUT_FORMAT__PAL_60       7





typedef struct 
{
    unsigned char resetib   : 1;
    unsigned char resetdb   : 1;
    unsigned char spare     : 8;
} reset_reg;

typedef struct 
{
    unsigned char fdp       : 1;
    unsigned char reserved  : 1;
    unsigned char ppdac     : 2;    
    unsigned char spare     : 8;
} powerState_reg;

typedef struct 
{
    unsigned char vos       : 4;
    unsigned char dacsw     : 2;
    unsigned char svideo    : 1;    
    unsigned char tv_bp     : 1;
} videoOutputFormat_reg;

typedef struct 
{
    unsigned char xtal      : 4;
    unsigned char spare     : 3;
    unsigned char xtalsel   : 1;
} crystal_reg;

typedef struct 
{
    unsigned char multi     : 1;
    unsigned char spare     : 5;
    unsigned char dcksel    : 1;
    unsigned char ducvbs    : 1;
} inputDataFormat1_reg;

typedef struct 
{
    unsigned char idf       : 3;
    unsigned char swap      : 3;
    unsigned char reverse   : 1;
    unsigned char high      : 1;
} inputDataFormat2_reg;

typedef struct 
{
    unsigned char diffen     : 1;
    unsigned char syo        : 1;
    unsigned char vpo        : 1;
    unsigned char hpo        : 1;
    unsigned char flds       : 1;
    unsigned char fldsen     : 1;
    unsigned char des        : 1;
    unsigned char pouten     : 1;
} sync_reg;

typedef struct 
{
    unsigned char ycv        : 2;
    unsigned char ysv        : 2;
    unsigned char cbw        : 1;
    unsigned char cfbp       : 1;
    unsigned char cfrb       : 1;
    unsigned char xch        : 1;
} tvFiler1_reg;

typedef struct 
{
    unsigned char aff        : 3;
    unsigned char spare      : 5;
} tvFiler2_reg;

typedef struct 
{
    unsigned char hai        : 3;
    unsigned char hti        : 3;
    unsigned char reserved   : 1;
    unsigned char hvauto     : 1;
} inputTiming1_reg;

typedef struct 
{
    unsigned char ho        : 3;
    unsigned char hw        : 2;
    unsigned char spare     : 3;
} inputTiming4_reg;

typedef struct 
{
    unsigned char ho        : 2;
    unsigned char vti       : 2;
    unsigned char vo        : 2;
    unsigned char spare     : 2;
} inputTiming7_reg;

typedef struct 
{
    unsigned char dotcr13   : 1;
    unsigned char bsdadj    : 3;
    unsigned char aciv      : 1;
    unsigned char spare     : 3;
} burstAmplitude_reg;



