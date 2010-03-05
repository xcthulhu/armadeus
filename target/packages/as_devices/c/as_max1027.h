/*
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Copyright (C) 2010 Fabien Marteau <fabien.marteau@armadeus.com> 
 *
 */

#ifndef AS_MAX1027_H_
#define AS_MAX1027_H_

/** @file
 * @brief Read/Write analog value with max1027 chip 
 *
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum  {
    AS_MAX1027_FAST,
    AS_MAX1027_SLOW
} AS_max1027_mode;

/**
 * Store max1027 parameters
 */
struct as_max1027_device {
    AS_max1027_mode mode;
};


/** @brief Open max1027
 *
 * @param aSpiNum spi bus number used 
 *
 * @return as_max1027_device structure pointer, NULL if error 
 */
struct as_max1027_device *as_max1027_open(int aSpiNum, 
                                          AS_max1027_mode mode);

//setAveraging();
//readTemperature_mC();
//readTemperature_C();
//
//getValueInMilliVolt();
//getValueInVolt();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AS_MAX1027_H_ */
