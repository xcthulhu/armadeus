#! /usr/bin/python
# -*- coding: utf-8 -*-
#-----------------------------------------------------------------------------
# Name:     AsDac.py
# Purpose:  
# Author:   Fabien Marteau <fabien.marteau@armadeus.com>
# Created:  21/02/2011
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
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software 
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#-----------------------------------------------------------------------------

__doc__ = "This class drive ARMadeus boards DACs"
__version__ = "0.1"
__author__ = "Fabien Marteau <fabien.marteau@armadeus.com>"

import wrappers.AsDac_wrap as wrapper

class AsDacError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class AsDac:
    """ Drive dac
    """

    # Dictionary of dac multiton classes
    __dac = {}

    ################################################################
    class __impl:
        """ implementation
        """
        def __init__(self, aDacType, aBusNumber, aAddress, aVRef):
            try:
                self.__device = wrapper.dac_open(aDacType, aBusNumber, aAddress, aVRef)
            except Exception, e:
                raise AsDacError("Can't open dac port type "+str(aDacType)+\
                                    " num "+str(aBusNumber))

        def __del__(self):
            try:
                wrapper.dac_close(self.__device)
            except Exception, e:
                pass

        def setValueInMillivolts(self, aChannel, aValue):
            """ Get value in millivolts
            """
            try:
                return wrapper.dac_setValueInMillivolts(self.__device, aChannel, aValue)
            except Exception, e:
                raise AsDacError(str(e))

    ################################################################

    @classmethod
    def getInstance(cls, aDacType, aBusNumber, aAddress, aVRef):
        try:
            return AsDac.__dac["%s:%d-0x%02x"%(aDacType, aBusNumber, aAddress)]
        except KeyError:
            AsDac.__dac["%s:%d-0x%02x"%(aDacType, aBusNumber, aAddress)] =\
                                    cls.__impl(aDacType, aBusNumber, aAddress, aVRef)

        return AsDac.__dac["%s:%d-0x%02x"%(aDacType, aDeviceNum, aAddress)]

    def  __init__(self):
        """ Initialize dac
        """
        raise Exception("This constructor is private, to instanciate object do:"+\
                        "AsDac.getInstance(dactype, busnumber, address, vref)")

if __name__ == "__main__":
   import os
   def pressEnterToContinue():
       print "Press enter to continue"
       raw_input()

