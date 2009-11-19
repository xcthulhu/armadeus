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

#ifndef __ASAD5258_H__
#define __ASAD5258_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* number of AD5258 under system */
#define NUMBER_OF_AD5258 1

int as_ad5258_init(int aAdNumber);

int as_ad5258_set_rdac(int aAdNumber, int aValue);
int as_ad5258_get_rdac(int aAdNumber);
int as_ad5258_set_eeprom(int aAdNumber, int aValue);
int as_ad5258_get_eeprom(int aAdNumber);
int as_ad5258_store(int aAdNumber);
int as_ad5258_restore(int aAdNumber);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ASAD5258_H__
