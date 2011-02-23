#! /usr/bin/python
# -*- coding: utf-8 -*-
#-----------------------------------------------------------------------------
# Name:     Apf27Pwm.py
# Purpose:  
# Author:   Fabien Marteau <fabien.marteau@armadeus.com>
# Created:  08/10/2009
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
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software 
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#-----------------------------------------------------------------------------
# Revision list :
#
# Date       By        Changes
#
#-----------------------------------------------------------------------------

__doc__ = "This class drive pwm under apf27."
__version__ = "1.0.0"
__versionTime__ = "08/10/2009"
__author__ = "Fabien Marteau <fabien.marteau@armadeus.com>"

import wrappers

class Apf27PwmError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class Apf27Pwm:
    """ Drive pwm on apf27
    """
    def __init__(self, pwm_num):
        self.__pwm_num = pwm_num
        try:
            self.wrapper = wrappers.Apf27Pwm_wrap
            self.wrapper.init(self.__pwm_num)
        except Exception, e:
            raise Apf27PwmError("Can't init pwm "+str(self.__pwm_num)+\
                                ": "+str(e))

    def __del__(self):
        try:
            self.wrapper.pwm_close(self.pwm_num)
        except Exception, e:
            raise Apf27PwmError(str(e))

    def setFrequency(self, aFrequency):
        """ Set frequency of pwm
            \param aFrequency frequency in Hz
        """
        try:
            self.wrapper.setFrequency(self.__pwm_num, aFrequency)
        except Exception, e:
            raise Apf27PwmError(str(e))

    def getFrequency(self):
        """ Get frequency of pwm
            \return frequency in Hz
        """
        try:
            return self.wrapper.getFrequency(self.__pwm_num)
        except Exception, e:
            raise Apf27PwmError(str(e))

    def setPeriod(self, aPeriod):
        """ Set period in us
            \param aPeriod period in us
        """
        try:
            self.wrapper.setPeriod(self.__pwm_num, aPeriod)
        except Exception, e:
            raise Apf27PwmError(str(e))

    def getPeriod(self):
        """ Get period in us
            \return period in us
        """
        try:
            return self.wrapper.getPeriod(self.__pwm_num)
        except Exception, e:
            raise Apf27PwmError(str(e))

    def setDuty(self, aDuty):
        """ Set duty in 1/1000
            \param aDuty in 1/1000
        """
        try:
            self.wrapper.setDuty(self.__pwm_num, aDuty)
        except Exception, e:
            raise Apf27PwmError(str(e))

    def getDuty(self):
        """ Get duty in 1/1000
            \return duty in 1/1000
        """
        try:
            return self.wrapper.getDuty(self.__pwm_num)
        except Exception,e:
            raise Apf27PwmError(str(e))

    def activate(self, aEnable):
        """ Activate pwm
            \param aEnable if True enable, else disable
            \return aEnable
        """
        try:
            if aEnable:
                self.wrapper.activate(self.__pwm_num, 1)
                return True
            else:
                self.wrapper.activate(self.__pwm_num, 0)
                return False
        except Exception, e:
            raise Apf27PwmError(str(e))

    def getState(self):
        """ Get state of pwm
            \return boolean
        """
        try:
            if self.wrapper.getState(self.__pwm_num) == 1:
                return True
            else:
                return False
        except Exception, e:
            raise Apf27PwmError(str(e))

