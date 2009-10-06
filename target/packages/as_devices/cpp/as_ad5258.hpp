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
**
** TODO: use kernel driver instead of i2c bus :
** http://www.mail-archive.com/uclinux-dist-devel@blackfin.uclinux.org/msg02051.html
*/

#ifndef __ASAD5258_HPP__
#define __ASAD5258_HPP__

#include "as_i2c.hpp"

#define I2C_AD5258_ADDR (0x18)
#define I2C_DEV_PATH "/dev/i2c-"
#define I2C_BUS_NUM (0)

/** AsAd5258 description
 *
 */
class AsAd5258 {
public:
    AsAd5258(int aI2cNumber);

    virtual ~AsAd5258();

    void setRdac(int aValue);
    int  getRdac(void);
    void setEeprom(int aValue);
    int  getEeprom(void);
    void store(void);
    void restore(void);

private:
    int mI2cNumber;
    unsigned char mI2cAd5258Addr;
    
    AsI2c * mI2c;

    /** XXX: i2c methods
     */
    int mFHandlerI2c;
    int i2c_init(void);
    int i2c_readByte (unsigned char reg, unsigned char *buf );
    int i2c_writeByte(unsigned char reg, unsigned char value);

};

#endif /* __ASAD5258_HPP__ */


