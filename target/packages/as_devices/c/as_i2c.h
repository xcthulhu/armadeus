/*
**    THE ARMadeus Systems
** 
**    Copyright (C) 2009  The armadeus systems team 
**    Fabien Marteau <fabien.marteau@armadeus.com>
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __ASI2C_H__
#define __ASI2C_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define NUMBER_OF_I2C 2

int as_i2c_init(int aBusNumber);

int as_read_byte_data(int aBusNumber,
                      unsigned char aChipAddr,
                      unsigned char aReg,
                      unsigned char *aBuf);

int as_write_byte_data(int aBusNumber,
                       unsigned char aChipAddr,
                       unsigned char aReg,
                       unsigned char aValue);

int as_write_byte(int aBusNumber,
                  unsigned char aChipAddr,
                  unsigned char aValue);

int as_write_multiple_bytes(int aBusNumber,
                            unsigned char aChipAddr,
                            unsigned char *aBuff,
                            int aSize);

int as_read_multiple_bytes(int aBusNumber,
                           unsigned char aChipAddr,
                           unsigned char *aBuff,
                           int aSize);

int as_close(int aBusNumber);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ASI2C_H__

