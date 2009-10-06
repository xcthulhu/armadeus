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
*/

#ifndef __ASMAX1027_HPP__
#define __ASMAX1027_HPP__

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>  /* for open()   */
#include <errno.h>  /* for perror() */
#include <unistd.h> /* for write()  */
#include "as_adc.hpp"

using namespace std;

/** Manage Analog to Digital Converter chip Max1027
 *
 *  @author Fabien Marteau <fabien.marteau@armadeus.com>
 */
class AsMax1027 : public AsAdc{
    public:

        /* Public singleton constructor */
        static AsMax1027 *getInstance(int aSpiNum);

        /* destructor */
        virtual ~AsMax1027();

        /* ADC access */
        virtual AsAdc_errors readTemperature_mC(void);
        virtual float readTemperature_C(void);
    
        virtual AsAdc_errors getValueInMiliVolt(void * aObject);
        virtual AsAdc_errors getValueInMiliVolt(int aChannelNumber);
        virtual float getValueInVolt (void * aObject);
        virtual float getValueInVolt (int aChannelNumber);

    protected:
        AsAdc_errors init (int aSpiNumber);

    private:
        static const char * SETUP_NAME;      /**< setup file name */
        static const char * CONVERSION_NAME; /**< conversion file name */
        static const char * TEMPERATURE_NAME;/**< temperature file name*/
        static const char * INPUT_PRE_NAME;  /**< analog channel name pre*/
        static const char * INPUT_POST_NAME; /**< analog channel name post*/
        static const char * SYS_PATH;        /**< /sys path */
        static const int CHANNELS_NUM = 7;   /**< number of channels */

        int mSpiNumber;
        string mSysPath;

        /* Driver files */
        int mFSetup;
        int mFTemp;
        int mFConversion;

        /* Singleton */
        static AsMax1027 *mMax1027;

        AsMax1027(int aSpiNum); /* Private constructor */

        /* somes usefull functions */
        string setIntValueInString(int aValue);
        AsAdc_errors writeBuffer(int aFile_handler, string aValueWrite);
        AsAdc_errors readBuffer(int aFile_handler, string *aValueRead);
    
        /* accessors */
        void setSpiNumber(int aSpiNumber){ mSpiNumber = aSpiNumber;};
        int getSpiNumber(void){return mSpiNumber;};

        void setSysPath(string aSyspath){ mSysPath = aSyspath;};
        string getSysPath(void){return mSysPath;};

        AsAdc_errors setChannelHandler(int aFchannel,int aChannel_num);
        AsAdc_errors getChannelHandler(int aChannel_num);
};
#endif /* __ASMAX1027_HPP__ */

