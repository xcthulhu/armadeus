#ifndef H_BACKLIGHT_CONTROL_INCLUDE
#define H_BACKLIGHT_CONTROL_INCLUDE


// Initialize the Backlight control library
int initBacklightControl( void );

// Set brightness to given value (0-100 %)
void setBrightness( unsigned char );

// Get current brightness (0-100 %)
unsigned char getBrightness( void );

// Free ressources allocated by the Backlight control library
void releaseBacklightControl( void );


#endif // H_BACKLIGHT_CONTROL_INCLUDE
