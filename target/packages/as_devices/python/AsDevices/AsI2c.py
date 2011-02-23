#! /usr/bin/python
# -*- coding: utf-8 -*-
#-----------------------------------------------------------------------------
# Name:     AsI2c.py
# Purpose:  
# Author:   Fabien Marteau <fabien.marteau@armadeus.com>
# Created:  23/02/2011
#-----------------------------------------------------------------------------
#  Copyright (2011)  Armadeus Systems
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software 
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#-----------------------------------------------------------------------------

__doc__ = "This class drive ARMadeus Board I²C"
__version__ = "0.1"
__author__ = "Fabien Marteau <fabien.marteau@armadeus.com>"

import wrappers.AsI2c_wrap as wrapper

class AsI2cError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class AsI2c:
    """
    """

    def __init__(self, aBusNumber):
        try:
            self.__device = wrapper.i2c_open(aBusNumber)
        except Exception, e:
            raise AsI2cError("Can't open i2c bus number "+str(aBusNumber)+\
                                ": "+str(e))

    def __def__(self):
        try:
            wrappers.i2c_close(self.__device)
        except Exception, e:
            pass

    def setSlaveAddr(self, aAddr):
        """ Set address of slave
        """
        try:
            return wrapper.i2c_set_slave_addr(self.__device, aAddr)
        except Exception, e:
            raise AsI2cError(str(e))

    def getSlaveAddr(self):
        """ Get address of slave
        """
        try:
            return wrapper.i2c_get_slave_addr(self.__device)
        except Exception, e:
            raise AsI2cError(str(e))

    def read(self, aNumber):
        """ read aNumber of bytes on i2c bus
        """
        try:
           rString = wrapper.i2c_read(self.__device, aNumber)
        except Exception, e:
            raise AsI2cError(str(e))
        rList = []
        for character in rString:
            rList.append(ord(character))
        return rList

    def write(self, aList):
        """ read a list of bytes on i2c bus
        """
        aString = ''
        for character in aList:
            aString += chr(character)
        try:
            return wrapper.i2c_write(self.__device, aString)
        except Exception, e:
            raise AsI2cError(str(e))

    def readReg(self, aReg, aNumber):
        """ read a list of bytes at aReg register address
        """
        try:
           rString = wrapper.i2c_read_reg(self.__device, aReg, aNumber)
        except Exception, e:
            raise AsI2cError(str(e))
        rList = []
        for character in rString:
            rList.append(ord(character))
        return rList

    def writeReg(self, aReg, aList):
        """ write a list of bytes at aReg register address
        """
        aString = ''
        for character in aList:
            aString += chr(character)
        try:
            return wrapper.i2c_write_reg(self.__device, aReg, aString)
        except Exception, e:
            raise AsI2cError(str(e))

    def readRegByte(self, aReg):
        """ Read one byte at aReg register address
        """
        try:
            return wrapper.i2c_read_reg_byte(self.__device, aReg)
        except Exception, e:
            raise AsI2cError(str(e))

    def writeRegByte(self, aReg, aValue):
        """ write one byte at aReg register address
        """
        try:
            return wrapper.i2c_write_reg_byte(self.__device, aReg, aValue)
        except Exception, e:
            raise AsI2cError(str(e))

