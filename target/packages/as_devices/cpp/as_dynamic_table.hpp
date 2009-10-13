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

#ifndef __ASDYNAMICTABLE_HPP_
#define __ASDYNAMICTABLE_HPP_

class AsDynamicTable {
    public:
        AsDynamicTable()
        {
            int i;
            for(i=0; i< PWM_NUMBER; i++)
            {
                mInst[i] = NULL;
            }
        }

        void * getInstance(int aInstanceNum)
        {
            return mInst[aInstanceNum];
        }

        void setInstance(void * aInstance, int aInstanceNum)
        {
            mInst[aInstanceNum] = aInstance;
        }

       
        virtual ~AsDynamicTable()
        {
        }

    private:
        void * mInst[DYNAMIC_TABLE_SIZE];
};

#endif // __ASDYNAMICTABLE_HPP_
