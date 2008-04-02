/*
 ***********************************************************************
 *
 * (c) Copyright 2006    Armadeus project
 * Julien Boibessot <julien.boibessot@armadeus.com>
 * Nicolas Colombain <nicolas.colombain@armadeus.com>
 * Generic Xilinx FPGA loader 
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
 **********************************************************************
 */

#ifndef __FPGA_LOADER_H__
#define __FPGA_LOADER_H__

#define FPGA_PROC_DIRNAME   "driver/fpga"
#define FPGA_PROC_FILENAME  FPGA_PROC_DIRNAME "/loader"

#define FPGA_DRIVER_VERSION "v0.0.6"
#define FPGA_DRIVER_NAME    "fpgaloader"

// By default, we use dynamic allocation of major numbers
#define FPGA_MAJOR 0
#define FPGA_MAX_MINOR 254

#define FPGA_IOCTL 0x10000000   // !! TO BE BETTER DEFINED !!

#endif // __FPGA_LOADER_H__
