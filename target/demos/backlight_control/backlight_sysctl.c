#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>


int initialized = 0;
FILE* fd_bright;
FILE* fd_max;
unsigned char current_brightness = 0;
int max_brightness = 255;

#ifdef TARGET
#define BRIGHTNESS_SYS_FILE     "/sys/class/backlight/imx-bl/brightness"
#define MAX_BRIGHTNESS_SYS_FILE "/sys/class/backlight/imx-bl/max_brightness"
#else
#define BRIGHTNESS_SYS_FILE     "./backlight.dat"
#define MAX_BRIGHTNESS_SYS_FILE "./backlight.dat"
#endif // TARGET

/*
 * Initialize the system
 */
int initBacklightControl()
{
    int result = -1;
    int size = 0;
    char buffer[6];

    if( !initialized )
    {
        fd_bright  = fopen( BRIGHTNESS_SYS_FILE, "w+" );
        fd_max = fopen( MAX_BRIGHTNESS_SYS_FILE, "r" );

        if( (fd_bright != NULL) && (fd_max != NULL) )
        {
            result = 0;
            initialized = 1;
#ifdef TARGET
            size = fread( buffer, 1, 6, fd_max );
            max_brightness = atoi(buffer);
            printf("Max: %i\n", max_brightness);
            size = fread( buffer, 1, 6, fd_bright );
            current_brightness = atoi(buffer);
            printf("Current: %i\n", current_brightness);
#else
            current_brightness = 50; // %    
#endif // TARGET
        } else {
            printf("Backlight: can't open %s interface\n", BRIGHTNESS_SYS_FILE);
        }
    }
    return( result );
}

/*
 * Free ressources allocated by the "library"
 */
void releaseBacklightControl()
{
    if( initialized )
    {
        fclose( fd_bright );
        fclose( fd_max );
        initialized = 0;
    }
}

/*
 * 0-100 %
 */
void setBrightness( unsigned char aValue )
{
    if( initialized )
    {
        char value[4]; int nchar;
        if( aValue > 100 ) aValue = 100;
        aValue = (aValue * max_brightness) / 100;

        // Send it to hardware
        nchar = sprintf( value, "%d", aValue);
        fwrite( value, 1, nchar, fd_bright );
        fflush( fd_bright );
        current_brightness = aValue;
    }
}

/*
 * 0-100 %
 */
unsigned char getBrightness( void )
{
    return( (current_brightness * 100) / max_brightness  );
}

