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
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
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

    # Dictionary of gpio multiton classes
    __gpio = {}

    ################################################################
    class __impl:
        """ implementation
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

        def getPullupValue(self):
            """ Get pullup value
                \param return pin value
            """
            try:
                return wrapper.getPullupValue(self.__device)
            except Exception, e:
                raise AsGpioError(str(e))

        def setPullupValue(self, aValue):
            """ Set pullup value
                \param aValue value 0:no pullup 1:pullup
            """
            try:
                return wrapper.setPullupValue(self.__device, aValue)
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


    ################################################################

    @classmethod
    def getInstance(cls, aPort_letter, aPinNumber):
        if type(aPort_letter)!=str:
            raise Exception("port letter must be a character")
        if ((aPort_letter[0] < 'A') or (aPort_letter[0] > 'Z')):
            raise Exception("Port letter must be an UPPER case letter")
        if type(aPinNumber)!=int:
            raise Exception("Pin number must be a int")
        if aPinNumber < 0 or aPinNumber > 31:
            raise Exception("Pin number must be between 0 and 31")

        try:
            return AsGpio.__gpio["%s%d"%(aPort_letter[0],aPinNumber)]
        except KeyError:
            AsGpio.__gpio["%s%d"%(aPort_letter[0],aPinNumber)] =\
                                    cls.__impl(aPort_letter[0], aPinNumber)

        return AsGpio.__gpio["%s%d"%(aPort_letter[0],aPinNumber)]

    def  __init__(self):
        """ Initialize pwm
        """
        raise Exception("This constructor is private, to instanciate object do:"+\
                        "AsGpio.getInstance(port_letter, pin_number)")

if __name__ == "__main__":
   import os
   def pressEnterToContinue():
       print "Press enter to continue"
       raw_input()

