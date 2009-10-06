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

#ifndef __ASDAC_HPP__
#define __ASDAC_HPP__

using namespace std;

/** AsDac description
 *
 */
class AsDac{
    public:
        enum AsDac_errors 
        {
            AS_DAC_NOCONNECTION = -7,
            AS_DAC_ALREADY_CONNECTED = -6,
            AS_DAC_IOCTL = -6,
            AS_DAC_WRONGCHAN = -5,
            AS_DAC_LSEEK = -4,
            AS_DAC_READFILE = -3,
            AS_DAC_WRITEFILE = -2,
            AS_DAC_OPENFILE = -1,
            AS_DAC_OK = 0
        };
    
        AsDac();
    
        virtual ~AsDac();
    
        virtual AsDac_errors init(int aI2cNum)=0;
        virtual AsDac_errors setDac(void * aObject, int aValue)=0;
        virtual AsDac_errors setDacPercent(void * aObject, float aValueInPercent)=0;
    
        /* */
        virtual AsDac_errors powerDown(void * aObject)=0;
    
        /* DAC connections */
        AsDac_errors connect(void * aObject, char aChannel);
        AsDac_errors disconnect(char aChannel);
        AsDac_errors disconnect(void * aObject);
    
    protected:
        char getObjectChannel(void * aObject);
    
        void ** mChannelObject; /**< object for each channel 'A' -> channel 0,
                                  'B' -> channel 1*/
        int mChannelNumber; /**< Channel number in component */
};

#endif /* __ASDAC_HPP__ */


