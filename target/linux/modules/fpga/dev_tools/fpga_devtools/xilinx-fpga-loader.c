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

#include "xilinx-fpga-loader.h"
#include "target-fpga-loader.h"
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
#include <asm/arch/imx-regs.h>
#else
#include <mach/imx-regs.h>
#endif
#include <asm/io.h>
#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/errno.h>

#define CONFIG_FPGA_DELAY()
#define CFG_FPGA_WAIT   4000  // uS

size_t bytecount = 0; // total bytes received
extern Xilinx_desc target_fpga_desc[]; // target descriptor table

/*
 * Timeout function
 */
static unsigned long get_timer(unsigned long initTime)
{
    struct timeval tv;
    do_gettimeofday(&tv);
    if( tv.tv_usec > initTime )  // avoid overflow pb
        return tv.tv_usec - initTime;
    else
        return initTime - tv.tv_usec;
}

/*
 * dump the given descriptor infos
 */
static int xilinx_dump_descriptor_info( Xilinx_desc *desc, char* buffer )
{
    int len;
    len  = sprintf( buffer, "%s %s %u\n", 
                    (desc->family == Xilinx_Spartan) ? "spartan":"unknown", 
                    (desc->iface == slave_serial) ? "slave serial":"unknown", 
                    desc->size );
    return len;
}

/**
 *  program the FPGA. 
 *  return 0 if success, >0 while programming, <0 if error detected
 */ 
static size_t spartan_serial_load (Xilinx_desc *desc, const char* buf, size_t bsize)
{
	Xilinx_Spartan_Slave_Serial_fns *fn = desc->iface_fns;
	if (fn) {
		unsigned long ts;		/* timestamp */

		/* Load the data */
        if( bytecount<=desc->size ) {
          	int i;
      	    unsigned char   val;
            size_t nbbyte = 0; // init local counter

		    while (nbbyte < bsize) {

			    /* Xilinx detects an error if INIT goes low (active)
			    while DONE is low (inactive) */
			    if ((*fn->done)() == 0 && (*fn->init)()) {
				    PRINTF ("** CRC error during FPGA load.\n");
    				return -ETIMEDOUT;
	    		}
		    	val = buf[nbbyte ++];
                bytecount++;
		    	i = 8;
		    	do {
			    	/* Deassert the clock */
			    	(*fn->clk)(0);
			    	CONFIG_FPGA_DELAY ();
			    	/* Write data */
			    	(*fn->wr)(val & 0x80);
			    	CONFIG_FPGA_DELAY ();
			    	/* Assert the clock */
			    	(*fn->clk)(1);
			    	CONFIG_FPGA_DELAY ();
			    	val <<= 1;
			    	i --;
			    } while (i > 0);
    	    }
        }
        if( bytecount>=desc->size ) 
        {
		    CONFIG_FPGA_DELAY ();

		    /* now check for done signal */
		    ts = get_timer(0);		/* get current time */
		    (*fn->wr)(1);

		    while (! (*fn->done)()) {
			    CONFIG_FPGA_DELAY ();
			    (*fn->clk)(0);	/* Deassert the clock pin */
			    CONFIG_FPGA_DELAY ();
			    (*fn->clk)(1);	/* Assert the clock pin */

			    if (get_timer (ts) > CFG_FPGA_WAIT) {	/* check the time */
				    PRINTF ("** Timeout waiting for DONE.\n");
				    return -ETIMEDOUT;
			    }
		    }
        }
        return bsize;
    }
	return -EINVAL;
}

/**
 *  initialize the FPGA programming interface. 
 *  return 0 if success, <0 if error detected
 */ 
static int spartan_serial_init (Xilinx_desc *desc)
{
	Xilinx_Spartan_Slave_Serial_fns *fn = desc->iface_fns;
	if (fn) {
		unsigned long ts;		/* timestamp */

        if(*fn->pre){
            (*fn->pre)(); //Run the pre configuration function if there is one.
        }

        /* Establish the initial state */
		(*fn->pgm)(1);	/* Assert the program, commit */

        /* Wait for INIT state (init low)                            */
		ts = get_timer(0);		/* get current time */
		do {
			    CONFIG_FPGA_DELAY ();
			    if (get_timer (ts) > CFG_FPGA_WAIT) {	/* check the time */
			    	PRINTF ("** Timeout waiting for INIT to start.\n");
				    return -ETIMEDOUT;
		    	}
		} while (!(*fn->init)());

		/* Get ready for the burn */
		CONFIG_FPGA_DELAY ();
		(*fn->pgm)(0);	/* Deassert the program, commit */

		ts = get_timer(0);		/* get current time */
		/* Now wait for INIT to go high */
        do {
			    CONFIG_FPGA_DELAY ();
			    if (get_timer (ts) > CFG_FPGA_WAIT) {	/* check the time */
				    PRINTF ("** Timeout waiting for INIT to clear.\n");
				return -ETIMEDOUT;
			    }
		} while ((*fn->init)());

        bytecount = 0; // reset byte count
        return 0; // success
    }
    return -EINVAL;
}

/**
 *  program the FPGA. 
 *  return 0 if success, >0 while programming, <0 if error detected
 */ 
size_t xilinx_load( Xilinx_desc *desc, const char *buf, size_t bsize )
{
    int ret = 0;

    if( desc ) {
        switch( desc->family ) // check family
        {
        case Xilinx_Spartan: 
            {
                switch( desc->iface ) // check donwload hardware interface
                {
                case slave_serial: ret = spartan_serial_load( desc, buf, bsize ); break;
                default: PRINTF("interface not supported!\n"); ret = -ENOSYS; break;
                }
            }
            break;
        default: PRINTF("family not supported!\n"); ret = -ENOSYS; break;
        }
    }
    else {
        PRINTF("invalid FPGA descriptor!\n");
        ret = -EINVAL;
    }
    return ret;
}

/**
 *  initialize the FPGA programming interface. 
 *  return 0 if success, <0 if error detected
 */ 
int xilinx_init_load( Xilinx_desc *desc )
{
    int ret = 0;

    if( desc ) {
        switch( desc->family ) // check family
        {
        case Xilinx_Spartan: 
            {
                switch( desc->iface ) // check donwload hardware interface
                {
                case slave_serial: ret = spartan_serial_init( desc ); break;
                default: PRINTF("interface not supported!\n"); ret = -ENOSYS; break;
                }
            }
            break;
        default: PRINTF("family not supported!\n"); ret = -ENOSYS; break;
        }
    }
    else {
        PRINTF("invalid FPGA descriptor!\n");
        ret = -EINVAL;
    }
    return ret;
}

/** 
 *  get the descriptor corresponding to desc_id
 *  return NULL if error
 */
Xilinx_desc * xilinx_get_descriptor( unsigned char desc_id )
{
    if( desc_id < NB_TARGET_DESC ){
        return  &(target_fpga_desc[desc_id]);
    }
    return NULL;
}

/** 
 *  get the descriptor infos
 */
int xilinx_get_descriptor_info( int desc_id, char* buffer )
{
    int len = 0;

    if( desc_id >= 0 ) {
        len  = xilinx_dump_descriptor_info( &target_fpga_desc[desc_id], buffer );
    }
    else {  // all desc requested
        unsigned char i;
        for( i=0; i<NB_TARGET_DESC; i++ ) {
            len  += xilinx_dump_descriptor_info( &target_fpga_desc[i], buffer+len );
        }
    }

    return len;
}



