/*
**    The ARMadeus Project
**
**    Copyright (C) 2011  The armadeus systems team
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

#ifndef __ASAS1531_H__
#define __ASAS1531_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "as_adc.h"

struct as_adc_device *as_adc_open_as1531(int aDeviceNum, int aVRef);

int32_t as_adc_get_value_in_millivolts_as1531(struct as_adc_device *aDev, int aChannel);

int32_t as_adc_close_as1531(struct as_adc_device *aDev);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __ASAS1531_H__ */
