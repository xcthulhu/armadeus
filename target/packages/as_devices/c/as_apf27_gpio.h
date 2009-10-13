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

#ifndef __ASAPF27GPIO_H__
#define __ASAPF27GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define NUMBER_OF_PORTS 6

int as_apf27_gpio_init(char aPortChar);

int as_apf27_gpio_set_pin_direction(char aPortChar,
                                    int aPinNum,
                                    int aDirection);

int as_apf27_gpio_set_pin_value(char aPortChar,
                                int aPinNum,
                                int aValue);

int as_apf27_gpio_get_pin_value(char aPortChar,
                                int aPinNum);

int as_apf27_gpio_close(char aPortChar);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ASAPF27GPIO_H__
