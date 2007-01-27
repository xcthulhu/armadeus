#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>

#include "buzzer.h"

int playing, initialized = 0;
FILE* fd;
FILE* fda;

#ifdef TARGET
#define FREQ_SYS_FILE   "/sys/class/pwm/pwm0/frequency"
#define ACTIVE_SYS_FILE "/sys/class/pwm/pwm0/active"
#else
#define FREQ_SYS_FILE   "./frequency"
#define ACTIVE_SYS_FILE "./active"
#endif // TARGET

SDL_TimerID myTimerID;

/*
 * Initialize the buzzer "sound" system
 *
 */
int initBuzzer()
{
    int result = -1;

    if( !initialized )
    {
        playing = 0;
        fd  = fopen( FREQ_SYS_FILE, "w" );
        fda = fopen( ACTIVE_SYS_FILE, "w" );

        if( (fd != NULL) && (fda != NULL) )
        {
            result = 0;
            initialized = 1;
        } else
            printf("Buzzer: can't open /sys interface\n");
    }
    return( result );
}

static Uint32 timerExpired(Uint32 interval, void* param)
{
    playing = 0;
    // Stop PWM
    fwrite( "0", 1, 1, fda );
    fflush( fda );
    //printf("Timer expired\n");

    return(0);
}

/*
 * Free ressources allocated by the sound system
 *
 */
void releaseBuzzer()
{
    if( initialized )
    {
        playing = 0;
        SDL_RemoveTimer( myTimerID );
        timerExpired(0, 0);
        fclose( fd );
        fclose( fda );
        initialized = 0;
    }
}

/*
 * Play a sound at given frequency during given time
 *
 * Frequency should be in Hertz 
 * Time in milliseconds
 */
void playSound( int aFreq, int aTime )
{
    if( (!playing) && (initialized) )
    {
        char freq[16]; int nchar;

        // Limit frequency to [50,15k] Herz
        if( aFreq < 50 ) aFreq = 50;
        if( aFreq > 15000 ) aFreq = 15000;
        // Launch PWM
        fwrite( "1", 1, 1, fda );
        fflush( fda );
        // Program PWM with given frequency
        nchar = sprintf( freq, "%d", aFreq);
        fwrite( freq, nchar, nchar, fd );
        fflush( fd );
        // Launch a timer that will stop sound when desired value is reached
        myTimerID = SDL_AddTimer( (Uint32) aTime, timerExpired , 0);
        playing = 1;
    }
}

