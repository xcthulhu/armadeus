/*
 * Xilinx FPGAs download support
 * Copyright (C) 2006-2009 Nicolas Colombain <nicolas.colombain@armadeus.com>
 *                         Armadeus Project / Armadeus systems
 *
 * Based on the implementation (U-Boot) of:
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


#define FPGA_DEBUG
//#undef FPGA_DEBUG

#ifdef FPGA_DEBUG
#define PRINTF(fmt,args...)	printk (fmt ,##args)
#else
#define PRINTF(fmt,args...)
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


typedef enum {
	min_xilinx_iface_type,		/* low range check value */
	slave_serial,			/* serial data and external clock */
	slave_parallel,			/* parallel data and external clock */
	max_xilinx_iface_type		/* insert all new types before this */
} Xilinx_iface;

typedef enum {
	min_xilinx_type,		/* low range check value */
	Xilinx_Spartan, 		/* Spartan-II Family */
	max_xilinx_type			/* insert all new types before this */
} Xilinx_Family;

struct fpga_desc {
	Xilinx_Family    family;	/* part type */
	Xilinx_iface     iface;		/* interface type */
	size_t           size;		/* bytes of data part can accept */
	void *           iface_fns;	/* interface function table */
};


/** pointer to target specific low level function */
typedef int (*Xilinx_pgm_fn)( int assert_pgm );
typedef int (*Xilinx_init_fn)(void);
typedef int (*Xilinx_done_fn)(void);
typedef int (*Xilinx_clk_fn)( int assert_clk );
typedef int (*Xilinx_wr_fn)( int assert_write );
typedef int (*Xilinx_pre_fn)(void);

typedef int (*Xilinx_cs_fn)( int assert_cs);
typedef int (*Xilinx_wdata_fn)( unsigned char data );
typedef int (*Xilinx_busy_fn)(void);
typedef int (*Xilinx_abort_fn)(void);
typedef int (*Xilinx_post_fn)(void);

/** struct of target specific low level functions */
typedef struct {
	Xilinx_pre_fn	pre;
	Xilinx_pgm_fn	pgm;
	Xilinx_clk_fn	clk;
	Xilinx_init_fn	init;
	Xilinx_done_fn	done;
	Xilinx_wr_fn	wr;
} Xilinx_Spartan_Slave_Serial_fns;

/* Slave Parallel Implementation function table */
typedef struct {
	Xilinx_pre_fn	pre;
	Xilinx_pgm_fn	pgm;
	Xilinx_clk_fn	clk;
	Xilinx_init_fn	init;
	Xilinx_done_fn	done;
	Xilinx_wr_fn	wr;
	Xilinx_cs_fn	cs;
	Xilinx_wdata_fn	wdata;
	Xilinx_busy_fn	busy;
	Xilinx_abort_fn	abort;
	Xilinx_post_fn	post;
} Xilinx_Spartan3_Slave_Parallel_fns;


/**
 *  program the FPGA.
 *  return 0 if success, >0 while programming, <0 if error detected
 */
size_t fpga_load(struct fpga_desc *desc, const char *buf, size_t bsize);

/**
 *  initialize the FPGA programming interface
 *  return 0 if success, <0 if error detected
 */
int fpga_init_load(struct fpga_desc *desc);

/* terminate FPGA loading */
int fpga_finish_load(struct fpga_desc *desc);

/**
 *  get some infos about FPGA download.
 *  return the number of char placed in the buffer
 */
int fpga_get_infos(struct fpga_desc *desc, char* buffer);

#endif // __XILINX_FPGA_H__
