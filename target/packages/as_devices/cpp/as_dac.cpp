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

#include "as_dac.hpp"

AsDac::AsDac()
{

}

AsDac::~AsDac()
{
}

/*------------------------------------------------------------------------------*/

/** Connect dac to an object
 *
 * @param object to connect on dac
 * @param channel number
 * @return error
 */
AsDac::AsDac_errors 
AsDac::connect(void * aObject, char aChannel)
{
    int channel;

    channel = aChannel - 'A';
    if( channel >= mChannelNumber || channel < 0)
    {
        return AS_DAC_WRONGCHAN;
    }
    if (mChannelObject[channel] != NULL)
    {
        return AS_DAC_ALREADY_CONNECTED;
    }
    mChannelObject[channel] = aObject;
    return AS_DAC_OK;
}

/*------------------------------------------------------------------------------*/

/** Connect adc to an object
 *
 * @param channel number
 * @return error
 */
AsDac::AsDac_errors 
AsDac::disconnect(char aChannel)
{

    int channel;

    channel = aChannel - 'A';
    if(channel >= mChannelNumber || channel < 0)
    {
        return AS_DAC_WRONGCHAN;
    }
    if(mChannelObject[channel] == NULL)
    {
        return AS_DAC_NOCONNECTION;
    }
    mChannelObject[channel] = NULL;
    return AS_DAC_OK;
}
/*------------------------------------------------------------------------------*/

/** Connect adc to an object
 *
 * @param channel number
 * @return error
 */
AsDac::AsDac_errors 
AsDac::disconnect(void * aObject)
{
    return disconnect(getObjectChannel(aObject));
}

/*------------------------------------------------------------------------------*/

/** Return channel number of an object
 *
 * @param object pointer
 * @return channel number
 */
char
AsDac::getObjectChannel(void * aObject)
{
    for (int i=0; i< mChannelNumber;i++)
    {
        if(aObject == mChannelObject[i])
        {
            return i+'A';
        }
    }
    return (int)AS_DAC_NOCONNECTION;
}

