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
# it under the terms of the GNU General Public License as published by
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
# Revision list :
#
# Date       By        Changes
#
#-----------------------------------------------------------------------------

__doc__ = "This class drive pwm under apf27. It's implemented as a multiton"
__version__ = "1.0.0"
__versionTime__ = "08/10/2009"
__author__ = "Fabien Marteau <fabien.marteau@armadeus.com>"

import wrappers

NUMBER_OF_PWM = 2

class Apf27PwmError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class Apf27Pwm:
    """ Drive pwm on apf27
    """

    __pwm = [None for i in range(NUMBER_OF_PWM)]

    ################################################################$
    class __impl:
        """ implementation
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


    ################################################################

    @classmethod
    def getInstance(cls, pwm_num):
        if type(pwm_num)!=int:
            raise Exception("pwm_num argunent must be 'int'")
        if pwm_num < 0:
            raise Exception("pwm_num must be positive")
        if pwm_num >= NUMBER_OF_PWM:
            raise Exception("There is no pwm number "+\
                    str(pwm_num)+", max is "+str(NUMBER_OF_PWM-1))
        if Apf27Pwm.__pwm[pwm_num] is None:
            Apf27Pwm.__pwm[pwm_num] = cls.__impl(pwm_num)

        return Apf27Pwm.__pwm[pwm_num]

    def  __init__(self):
        """ Initialize pwm
        """
        raise Exception("This constructor is private")

if __name__ == "__main__":
    import os
    def pressEnterToContinue():
        print "Press enter to continue"
        raw_input()

    print "Apf27Pwm class test\n"
    print Apf27Pwm.__doc__

    pwm0 = Apf27Pwm.getInstance(0)

    keyInput = "0"

    while keyInput is not "q":
        os.system("clear")
        print "***********************************************"
        print "*       Testing pwm python class              *"
        print "***********************************************"
        print "Choose ('q' to quit):"
        print " 1) set Frequency"
        print " 2) get Frequency"
        print " 3) set Period"
        print " 4) get Period"
        print " 5) set Duty"
        print " 6) get Duty"
        print " 7) activate/desactivate"
        print " 8) get state"
        print "> ",

        keyInput = raw_input()

        if   keyInput == "1" :
            print "Give frequency :",
            freq = raw_input()
            pwm0.setFrequency(int(freq,10))
            pressEnterToContinue()
        elif keyInput == "2" :
            print "Current pwm frequency is "+str(pwm0.getFrequency())
            pressEnterToContinue()
        elif keyInput == "3" :
            print "Give period :",
            period = raw_input()
            pwm0.setFrequency(int(period,10))
            pressEnterToContinue()
        elif keyInput == "4" :
            print "Current period is "+str(pwm0.getPeriod())
            pressEnterToContinue()
        elif keyInput == "5" :
            print "Give duty :",
            duty = raw_input()
            pwm0.setDuty(int(duty,10))
            pressEnterToContinue()
        elif keyInput == "6" :
            print "Current duty is "+str(pwm0.getDuty())
            pressEnterToContinue()
        elif keyInput == "7" :
            print "activate 'a' or desactivat 'd' ?",
            response = raw_input()
            if response == 'a':
                pwm0.activate(True)
            else:
                pwm0.activate(False)
            pressEnterToContinue()
        elif keyInput == "8" :
            if pwm0.getState():
                print "pwm is active"
            else:
                print "pwm is inactive"
            pressEnterToContinue() 
