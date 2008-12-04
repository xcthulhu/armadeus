/*
 * definitions.h - imxregs common definitions
 *
 * Copyright (C) 2008 armadeus systems
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

typedef unsigned int u32;

struct reg_info {
   char *name;
   u32 addr;
   int shift;
   u32 mask;
   char type;
   char *desc;
};

// Size of mmapping:
#define MAP_SIZE 4096
#define MAP_MASK ( MAP_SIZE - 1 )
