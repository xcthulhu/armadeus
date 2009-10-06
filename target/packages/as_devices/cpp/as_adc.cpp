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
#include <stdlib.h>
#include <stdio.h>
#include "as_adc.hpp"

AsAdc::AsAdc(int aNumberOfChannels)
{
    mChannelNumber = aNumberOfChannels;
    mChannelHandlers = (int *)malloc(sizeof(int)*aNumberOfChannels);
    for(int i=0; i < aNumberOfChannels; i++)
    {
        mChannelHandlers[i] = -1;
    }
    mChannelObject = (void **)malloc(sizeof(void *)*aNumberOfChannels);
    for(int i=0; i < aNumberOfChannels; i++)
    {
        mChannelObject[i] = NULL;
    }
}

AsAdc::~AsAdc()
{
    free(mChannelHandlers);
    free(mChannelObject);
}

/*------------------------------------------------------------------------------*/

/** Connect adc to an object
 *
 * @param object to connect on adc
 * @param channel number
 * @return error
 */
AsAdc::AsAdc_errors 
AsAdc::connect(void * aObject, int aChannelNum)
{
    if( aChannelNum >= mChannelNumber || aChannelNum < 0)
    {
        return AS_ADC_WRONGCHAN;
    }
    if (mChannelObject[aChannelNum] != NULL)
    {
        return AS_ADC_ALREADY_CONNECTED;
    }
    mChannelObject[aChannelNum] = aObject;
    return AS_ADC_OK;
}

/*------------------------------------------------------------------------------*/

/** Connect adc to an object
 *
 * @param channel number
 * @return error
 */
AsAdc::AsAdc_errors 
AsAdc::disconnect(int aChannelNum)
{
    if(aChannelNum >= mChannelNumber || aChannelNum < 0)
    {
        return AS_ADC_WRONGCHAN;
    }
    if(mChannelObject[aChannelNum] == NULL)
    {
        return AS_ADC_NOCONNECTION;
    }
    mChannelObject[aChannelNum] = NULL;
    return AS_ADC_OK;
}
/*------------------------------------------------------------------------------*/

/** Connect adc to an object
 *
 * @param channel number
 * @return error
 */
AsAdc::AsAdc_errors 
AsAdc::disconnect(void * aObject)
{
    return disconnect(getObjectChannel(aObject));
}

/*------------------------------------------------------------------------------*/

/** Return channel number of an object
 *
 * @param object pointer
 * @return channel number
 */
int
AsAdc::getObjectChannel(void * aObject)
{
    for (int i=0; i< mChannelNumber;i++)
    {
        if(aObject == mChannelObject[i])
        {
            return i;
        }
    }
    printf("No connection for this object in adc\n");
    return (int)AS_ADC_NOCONNECTION;
}

