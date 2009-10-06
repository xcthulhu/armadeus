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

#ifdef __cplusplus
# include "test_cpp.h"
#else
# include "test_c.h"
#endif

int main(int argc, char ** argv)
{
    char buffer[20];
    buffer[0] = '0';

    while(buffer[0] != 'q')
    {
        system("clear");
        printf("*******************************************************\n");
        printf("*   Testing program for as_devices library            *\n");
        printf("*******************************************************\n");
        printf("Choose a test ('q' to quit):\n");
        printf(" 1) Testing pwm\n");
        printf("> ");
        scanf("%s",buffer);
        
        switch(buffer[0])
        {
            case '1' : test_pwm();
                       break;
            default : break;
        }
    }
    return 0;
}
