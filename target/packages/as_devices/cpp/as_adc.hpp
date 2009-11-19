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

#ifndef __ASADC_HPP__
#define __ASADC_HPP__

using namespace std;

/** AsAdc description
 *
 */
class AsAdc{
    public:
        enum AsAdc_errors 
        {
            AS_ADC_NOCONNECTION = -7,
            AS_ADC_ALREADY_CONNECTED = -6,
            AS_ADC_WRONGCHAN = -5,
            AS_ADC_LSEEK = -4,
            AS_ADC_READFILE = -3,
            AS_ADC_WRITEFILE = -2,
            AS_ADC_OPENFILE = -1,
            AS_ADC_OK = 0
        };
     
        AsAdc(int aNumberOfChannels);
    
        virtual ~AsAdc();

        /* ADC connection */
        AsAdc_errors connect(void * aObject, int aChannelNum);
        AsAdc_errors disconnect(int aChannelNum);
        AsAdc_errors disconnect(void * aObject);

        /* ADC access */
        virtual AsAdc_errors readTemperature_mC(void)=0;
        virtual float readTemperature_C(void)=0;
    
        virtual AsAdc_errors getValueInMiliVolt(void * aObject)=0;
        virtual AsAdc_errors getValueInMiliVolt(int aChannelNumber)=0;
        virtual float getValueInVolt (void * aObject)=0;

    protected:
        int getObjectChannel(void * aObject);
        int * mChannelHandlers; /**< file handler for each channel */
        void ** mChannelObject; /**< object for each channel */
        int mChannelNumber;

};

#endif /* __ASADC_HPP__ */

