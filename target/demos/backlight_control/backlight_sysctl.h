#ifndef H_BACKLIGHT_CONTROL_INCLUDE
#define H_BACKLIGHT_CONTROL_INCLUDE


// Initialize the Backlight control library
int initBacklightControl( void );

// Play a sound on buzzer at given frequency (Hz) and during given time (ms)
void setBacklight( unsigned char aValue );

// Free ressources allocated by the Backlight control library
void releaseBacklightControl( void );


#endif // H_BACKLIGHT_CONTROL_INCLUDE
