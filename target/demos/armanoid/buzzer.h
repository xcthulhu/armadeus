#ifndef H_BUZZER_INCLUDE
#define H_BUZZER_INCLUDE


// Initialize the buzzer "sound" system
int initBuzzer( void );
// Play a sound on buzzer at given frequency (Hz) and during given time (ms)
void playSound( int aFreq, int aTime );
// Free ressources allocated by the sound system
void releaseBuzzer( void );


#endif //H_BUZZER_INCLUDE
