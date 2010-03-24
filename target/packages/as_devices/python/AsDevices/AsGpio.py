#! /usr/bin/python
# -*- coding: utf-8 -*-
#-----------------------------------------------------------------------------
# Name:     AsGpio.py
# Purpose:  
# Author:   Fabien Marteau <fabien.marteau@armadeus.com>
# Created:  23/03/2010
#-----------------------------------------------------------------------------
#  Copyright (2008)  Armadeus Systems
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

    ################################################################$
    class __impl:
        """ implementation
        """
        def __init__(self, port_letter):
            self.__port_letter = port_letter
            try:
                self.__device = wrapper.gpio_open(self.__port_letter)
            except Exception, e:
                raise AsGpioError("Can't open GPIO "+str(self.__port_letter)+\
                                    ": "+str(e))

        def __del__(self):
            try:
                wrapper.gpio_close(self.__device)
            except Exception, e:
                pass

        def setPinDirection(self, aPinNum, aDirection):
            """ Set period in us
                \param aPinNum pin number
                \param aDirection pin direction 0:input 1:output
            """
            try:
                wrapper.setPinDirection(self.__device, aPinNum, aDirection)
            except Exception, e:
                raise AsGpioError(str(e))

        def setPinValue(self, aPinNum, aValue):
            """ Set pin value
                \param aPinNum pin number
                \param aValue value to write
            """
            try:
                wrapper.setPinValue(self.__device, aPinNum, aValue)
            except Exception, e:
                raise AsGpioError(str(e))

        def getPinValue(self, aPinNum):
            """ Get pin value
                \param aPinNum pin number
                \return pin value
            """
            try:
                return wrapper.getPinValue(self.__device, aPinNum)
            except Exception, e:
                raise AsGpioError(str(e))


    ################################################################

    @classmethod
    def getInstance(cls, aPort_letter):
        if type(aPort_letter)!=str:
            raise Exception("port letter must be a character")
        if ((aPort_letter[0] < 'A') or (aPort_letter[0] > 'Z')):
            raise Exception("Port letter must be an UPPER case letter")

        try:
            return AsGpio.__gpio[aPort_letter[0]]
        except KeyError:
            AsGpio.__gpio[aPort_letter[0]] = cls.__impl(aPort_letter[0])

        return AsGpio.__gpio[aPort_letter[0]]

    def  __init__(self):
        """ Initialize pwm
        """
        raise Exception("This constructor is private")

if __name__ == "__main__":
   import os
   def pressEnterToContinue():
       print "Press enter to continue"
       raw_input()
   print "AsGpio class test\n"
   print AsGpio.__doc__
   
   portf = AsGpio.getInstance('F')
   print str(portf)

