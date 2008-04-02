/*
 * (c) Copyright 2006    Armadeus project
 *  Nicolas Colombain <nicolas.colombain@armadeus.com>
 *  Xilinx FPGA support
 *
 * Based on the implementation(uBoot) of: 
 * Rich Ireland, Enterasys Networks, rireland@enterasys.com.
 * Keith Outwater, keith_outwater@mvis.com
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
 */
#ifndef __XILINX_FPGA_H__
#define __XILINX_FPGA_H__

#include <linux/types.h>

//#define FPGA_DEBUG
#undef FPGA_DEBUG

#ifdef FPGA_DEBUG
#define	PRINTF(fmt,args...)	printk (fmt ,##args)
#else
#define	PRINTF(fmt,args...)
#endif



/* Spartan-III */
#define XILINX_XC3S50_SIZE  	439264/8
#define XILINX_XC3S200_SIZE  	1047616/8
#define XILINX_XC3S400_SIZE  	1699136/8
#define XILINX_XC3S1000_SIZE 	3223488/8
#define XILINX_XC3S1500_SIZE 	5214784/8
#define XILINX_XC3S2000_SIZE 	7673024/8
#define XILINX_XC3S4000_SIZE 	11316864/8
#define XILINX_XC3S5000_SIZE 	13271936/8


typedef enum {					/* typedef Xilinx_iface	*/
	min_xilinx_iface_type,		/* low range check value */
    slave_serial,				/* serial data and external clock */
    max_xilinx_iface_type		/* insert all new types before this */
} Xilinx_iface;					/* end, typedef Xilinx_iface */

typedef enum {					/* typedef Xilinx_Family */
	min_xilinx_type,			/* low range check value */
    Xilinx_Spartan, 			/* Spartan-II Family */
    max_xilinx_type				/* insert all new types before this */
} Xilinx_Family;				/* end, typedef Xilinx_Family */

typedef struct {				/* typedef Xilinx_desc */
    Xilinx_Family    family;	/* part type */
    Xilinx_iface     iface;		/* interface type */
    size_t           size;		/* bytes of data part can accept */
	void *           iface_fns;	/* interface function table */
} Xilinx_desc;					/* end, typedef Xilinx_desc */


/** pointer to target specific low level function */
typedef int (*Xilinx_pgm_fn)( int assert_pgm );
typedef int (*Xilinx_init_fn)(void);
typedef int (*Xilinx_done_fn)(void);
typedef int (*Xilinx_clk_fn)( int assert_clk );
typedef int (*Xilinx_wr_fn)( int assert_write );
typedef int (*Xilinx_pre_fn)(void);

/** struct of target specific low level functions */
typedef struct {
	Xilinx_pre_fn	pre;
	Xilinx_pgm_fn	pgm;
	Xilinx_clk_fn	clk;
	Xilinx_init_fn	init;
	Xilinx_done_fn	done;
	Xilinx_wr_fn	wr;
} Xilinx_Spartan_Slave_Serial_fns;

/**
 *  program the FPGA. 
 *  return 0 if success, >0 while programming, <0 if error detected
 */ 
size_t xilinx_load( Xilinx_desc *desc, const char *buf, size_t bsize );

/**
 *  initialize the FPGA programming interface. 
 *  return 0 if success, <0 if error detected
 */ 
int xilinx_init_load( Xilinx_desc *desc );

/** 
 *  get the descriptor infos, return the number of char placed in the buffer
 */
int xilinx_get_descriptor_info( int desc_id, char* buffer);

/** 
 *  get the descriptor corresponding to desc_id
 *  return NULL if error
 */
Xilinx_desc * xilinx_get_descriptor( unsigned char desc_id );

#endif // __XILINX_FPGA_H__
