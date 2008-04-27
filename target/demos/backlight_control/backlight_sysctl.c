#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>

#include "backlight_sysctl.h"

int playing, initialized = 0;
FILE* fd;
FILE* fda;
unsigned char currentBacklight = 0;

#ifdef TARGET
#define FREQ_SYS_FILE   "/sys/class/pwm/pwm0/frequency"
#define ACTIVE_SYS_FILE "/sys/class/pwm/pwm0/active"
#else
#define FREQ_SYS_FILE   "./backlight.dat"
#define ACTIVE_SYS_FILE "./active"
#endif // TARGET

SDL_TimerID myTimerID;

/*
 * Initialize the system
 *
 */
int initBacklightControl()
{
    int result = -1;

    if( !initialized )
    {
        playing = 0;
        fd  = fopen( FREQ_SYS_FILE, "w+" );
        fda = fopen( ACTIVE_SYS_FILE, "w" );

        if( (fd != NULL) && (fda != NULL) )
        {
            result = 0;
            initialized = 1;
#ifdef TARGET

#else
            currentBacklight = 50; // %    
#endif // TARGET
        } else
            printf("Buzzer: can't open /sys interface\n");
    }
    return( result );
}

/*
 * Free ressources allocated by the "library"
 *
 */
void releaseBacklightControl()
{
    if( initialized )
    {
        fclose( fd );
        fclose( fda );
        initialized = 0;
    }
}

/*
 * 0-100 %
 */
void setBacklight( unsigned char aValue )
{
    if( initialized )
    {
        char value[4]; int nchar;

        // Program PWM with given frequency
        nchar = sprintf( value, "%d", aValue);
        fwrite( value, 1, nchar, fd );
        fflush( fd );
        currentBacklight = aValue;
    }
}

/*
 * 0-100 %
 */
unsigned char getBacklight( void )
{
    return( currentBacklight );
}

