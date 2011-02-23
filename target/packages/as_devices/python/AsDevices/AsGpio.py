#! /usr/bin/python
# -*- coding: utf-8 -*-
#-----------------------------------------------------------------------------
# Name:     AsGpio.py
# Purpose:  
# Author:   Fabien Marteau <fabien.marteau@armadeus.com>
# Created:  23/03/2010
#-----------------------------------------------------------------------------
#  Copyright (2010)  Armadeus Systems
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software 
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#-----------------------------------------------------------------------------

__doc__ = "This class drive ARMadeus board GPIO"
__version__ = "1.0.0"
__versionTime__ = "23/03/2010"
__author__ = "Fabien Marteau <fabien.marteau@armadeus.com>"

import wrappers.AsGpio_wrap as wrapper

class AsGpioError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class AsGpio:
    """ Drive GPIO
    """
    def __init__(self, aPortLetter, aPinNum):
        try:
            self.__device = wrapper.gpio_open(aPortLetter, aPinNum)
        except Exception, e:
            raise AsGpioError("Can't open GPIO port "+str(aPortLetter)+\
                                " pin "+str(aPinNum)+": "+str(e))

    def __del__(self):
        try:
            wrapper.gpio_close(self.__device)
        except Exception, e:
            pass

    def setPinDirection(self, aDirection):
        """ Set period in us
            \param aDirection pin direction 0:input 1:output
        """
        try:
            wrapper.setPinDirection(self.__device, aDirection)
        except Exception, e:
            raise AsGpioError(str(e))

    def getPinDirection(self):
        """ Set period in us
            \param aDirection pin direction 0:input 1:output
        """
        try:
            return wrapper.getPinDirection(self.__device)
        except Exception, e:
            raise AsGpioError(str(e))

    def setPinValue(self, aValue):
        """ Set pin value
            \param aValue value to write
        """
        try:
            wrapper.setPinValue(self.__device, aValue)
        except Exception, e:
            raise AsGpioError(str(e))

    def getPinValue(self):
        """ Get pin value
            \param return pin value
        """
        try:
            return wrapper.getPinValue(self.__device)
        except Exception, e:
            raise AsGpioError(str(e))

    def blockingGetPinValue(self, aDelay_s, aDelay_us=0):
        """ Get pin value (blocking mode)
            \param return pin value
        """
        try:
            return wrapper.blockingGetPinValue(self.__device,
                                            aDelay_s,
                                            aDelay_us)
        except Exception, e:
            raise AsGpioError(str(e))

    def setIrqMode(self, aMode):
        """ Set irq Mode
            \param aMode.
                1:MODE_NOINT
                2:MODE_RISING
                3:MODE_FALLING
                4:MODE_BOTH
        """
        try:
            return wrapper.setIrqMode(self.__device, aMode)
        except Exception, e:
            raise AsGpioError(str(e))

    def getIrqMode(self):
        """ get irq mode
        """
        try:
            return wrapper.getIrqMode(self.__device)
        except Exception, e:
            raise AsGpioError(str(e))


    def getPinNumber(self):
        """ Get pin number
            \param return pin number
        """
        try:
            return wrapper.getPinNumber(self.__device)
        except Exception, e:
            raise AsGpioError(str(e))

    def getPortLetter(self):
        """ Get pin letter
            \param return pin letter
        """
        try:
            return wrapper.getPortLetter(self.__device)
        except Exception, e:
            raise AsGpioError(str(e))

