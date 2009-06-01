/* Author: rebouxan
 * 26/05/2009 
 * MP v1.0
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation
 * 51 Franklin Street, Fifth Floor
 * Boston, MA  02110-1301, USA.
 */


#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#include <dirent.h>

/* The music directory that has to be loaded */
#define MUSIC_DIR "music"

/* Song sampling */
#define AUDIO_RATE 44100

/* MusicDir max files number */
#define NB_FILES_MAX 32

/* Functions' prototypes *****/
// in order of appearence ...
void initAll();
void cleanAll();
void exitr();
void handleKey(SDL_KeyboardEvent key);
void musicDone();
void openSong();
void playRep();
void openRep();
void timePlay();
void timeReset();
void displayBack();
void displayTime();
void displayVol();
void displaySong();
void displayRep();
void displayAll();

/* Global Vars **********/

  int done = 0;
  int vol=64;

  SDL_Surface   *screen = NULL,
		*background = NULL,
		*icoMusic = NULL,
		*icoPlay = NULL,
		*icoVol = NULL,
		*textVol = NULL,
		*textSong = NULL,
		*timer =NULL,
		*nameRep = NULL,
		*nbMusic = NULL;   

  SDL_Surface *playlist[NB_FILES_MAX];

  SDL_Event event;

  Mix_Music *music = NULL;

  /* Fonts */
  TTF_Font 	*fontSong = NULL,
	   	*fontTimer = NULL,
           	*fontVol = NULL,
	   	*fontRep = NULL;
  
  /* Font colors */
  SDL_Color     cBlack = {0, 0, 0},
		cWhite = {255, 255, 255},
		cSong = {252, 185, 15};

  const char *icons[] = {"data/play.bmp","data/pause.bmp","data/stop.bmp"};
  char *tracks[NB_FILES_MAX]; /* without filepath */
  char musicDir[64];

  int numIcon =2;
  int numSongPlay=0;
  int nbFile=0; //var counter nb files
  int currentTime = 0, previousTime = 0, counter = 0;
  int second = 0, minute = 0;
  int nbSongHigh = 0;
  int nb_song_display=0;
  int next=0,prev=0, yRep=0;

/* 
 * main
 *
 *********************************/
int main(int argc, char **argv)
{
	/* Take the arg if there is one */
	if (argc == 2) {
		strcpy(musicDir,argv[1]);
	}   
	else strcpy(musicDir,MUSIC_DIR);

	/* We initialise all we need */
	initAll();

	/* while done=0, the player keep on running */
	while (!done) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					done = 1; 
				break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_q)
						done=1;
					handleKey(event.key);
				break;
			}
		}
		/* TIMER */
		timePlay();

		/* Display function */
		displayAll();

		/* Limit the use of the CPU */
		SDL_Delay(50);
	}

	/* This is the cleaning up part */
	cleanAll();
	return 0;
} /***************End_main()*********/

/* 
 * Initialise all kind of things we need
 * */
void initAll()
{
	/* SDL init */
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	TTF_Init();

	/* Hide the mouse */  
	SDL_ShowCursor(0);

	/* Enable Key repeat */
	SDL_EnableKeyRepeat(100, 100);

	/* This is where we open up our audio device */
	if (Mix_OpenAudio(AUDIO_RATE, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096)) {
		printf("Unable to open audio! %s\n", Mix_GetError());
		exitr();
	}

	/* Set the video mode to LQ043 resolution */
	screen = SDL_SetVideoMode(480, 272, 0, SDL_ANYFORMAT);
	if (screen == NULL) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		exitr();;
	}

	/* Fonts opening */
	fontSong = TTF_OpenFont("data/font.ttf", 42);
	fontTimer = TTF_OpenFont("data/font.ttf", 30);    
	fontVol = TTF_OpenFont("data/font.ttf", 20);
	fontRep = TTF_OpenFont("data/font.ttf", 16);
	/* To avoid seg-error if problems with fonts */
	if (!fontSong||!fontTimer||!fontVol||!fontRep) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		exitr();
	}

	/* We open the dir where songs are ... */
	openRep();

} /**** End of Init ****/

/*
 * Close and clean all parts
 * */
void cleanAll()
{
	Mix_CloseAudio();

	TTF_CloseFont(fontSong);
	TTF_CloseFont(fontTimer);
	TTF_CloseFont(fontVol);
	TTF_CloseFont(fontRep);

	TTF_Quit();

	SDL_FreeSurface(textSong);
	SDL_FreeSurface(textVol);
	SDL_FreeSurface(nameRep);
	SDL_FreeSurface(nbMusic);
	SDL_FreeSurface(timer);
	SDL_FreeSurface(icoMusic);
	SDL_FreeSurface(icoVol);
	SDL_FreeSurface(icoPlay);
	SDL_FreeSurface(background);
	SDL_FreeSurface(screen);

	SDL_Quit();
}

/*
 * If troubles, clean all before exiting
 * */
void exitr()
{
	cleanAll();
	exit(0);
}

/*
 * Function that runs Keyboard events 
 * */
void handleKey(SDL_KeyboardEvent key)
{
	switch(key.keysym.sym) {

		case SDLK_o: // Play/stop
		if (key.state == SDL_PRESSED) {
			openSong();
		}
		break;

		case SDLK_p: // Pause/resume
			if (key.state == SDL_PRESSED) {
				if (music !=NULL) {
					if (Mix_PausedMusic()) {
						Mix_ResumeMusic();
						numIcon=0;
					} else {
						Mix_PauseMusic();
						numIcon=1;
					}
				}
				else openSong();
			}
		break;

		/* 32 volume levels */
		case SDLK_KP_MINUS: // volume down
			if (key.state == SDL_PRESSED)
				if (vol>0) {
					vol=vol-4;
					Mix_VolumeMusic(vol);
				}
		break;

		case SDLK_KP_PLUS: // volume up
			if (key.state == SDL_PRESSED)
				if (vol<128) {
					vol=vol+4;
					Mix_VolumeMusic(vol);
				} 
		break;

		case SDLK_v: // volume to 128 (max)
			if (key.state == SDL_PRESSED)
				vol=128;
			Mix_VolumeMusic(vol);
			break;
		
		case SDLK_LEFT: // previous song
			if( key.state == SDL_PRESSED) {
				if (numSongPlay>0) {
					if (music !=NULL) musicDone();
					numSongPlay=numSongPlay-1;
					openSong();
				}
			}
		break;

		case SDLK_RIGHT: // next song 
			if (key.state == SDL_PRESSED) {
				if (numSongPlay<nbFile-1) {
					if (music !=NULL) musicDone();
					numSongPlay=numSongPlay+1;
					openSong();
				}
			}
		break;

		/* Song cursor with highlighting ... */
		case SDLK_UP: // Up
			if (nbSongHigh>0) {
				nbSongHigh--;
				prev=1;
				next=0;
			}
		break;

		case SDLK_DOWN: // Down
			if (nbSongHigh<nbFile-1) {
				nbSongHigh++;
				prev=0;
				next=1;
			}
		break;

		case SDLK_RETURN: // Play highlighted song
			numSongPlay=nbSongHigh;
			if (music != 0) 
				musicDone();
			openSong();
		break;
	}
}

/* 
 * Unload and free the music
 * */
void musicDone() 
{
	if (Mix_PlayingMusic()) numIcon=2;
	Mix_HaltMusic();
	Mix_FreeMusic(music);
	music = NULL;
}

/* 
 * Load a track and play it
 * */
void openSong()
{
	/* if music=NULL, load one */
	if(music == NULL) {
		/* Add the path to the song */
		char wps[32] = ""; // char "music/song"
		strcat(wps,musicDir); 	/* wps="music" */
		strcat(wps,"/");	/* wps="music/" */
		strcat(wps,tracks[numSongPlay]);/* wps="music/song" */

		/* Loads up the music */
		music = Mix_LoadMUS(wps); 
		/* Set its volume */
		Mix_VolumeMusic(vol);
		/* Reset the timer */
		timeReset();
		/* And play it */
		Mix_PlayMusic(music, 0);
		/* Set play icon */
		numIcon=0;

		/* When the song's over, play the next one */
		Mix_HookMusicFinished(playRep);
	} else { /* music !=NULL */
		/* Stop the music from playing And free it */
		musicDone();
	}
}

/* Play the whole directory 
 * When a music is over, the next is played
 * */
void playRep()
{
	/* Free Music then load another */
	musicDone();
	if (numSongPlay<nbFile-1) {
		numSongPlay=numSongPlay+1;
		openSong();
	} else numIcon=2; // stop icon
}

/* 
 * Open a musics' directory
 * */
void openRep() 
{
	DIR *rep ;
	struct dirent *file ;

	/* open rep ****/
	rep = opendir (musicDir) ;
	int count=0;

	if (rep != NULL) { // if rep not null, we put filenames in a tab
		/* 1- Count how many files in musicDir */
		while (file = readdir(rep))
			// Don't display . and ..
			if (strcmp(file->d_name, "." ) == 0 || strcmp(file->d_name, ".." ) == 0) 
			continue;
			else nbFile++;
		
		/* 2- Go back to the begining of the rep */
		rewinddir(rep);

		/* 3- Put filename in a tab */
		while (file = readdir(rep)) {
			if (strcmp(file->d_name, "." ) == 0 || strcmp(file->d_name, ".." ) == 0)
				continue;
			else {
				tracks[count]= (char *) malloc(sizeof(file->d_name) + 1);
				strcpy(tracks[count++],file->d_name);
			} 
		}

		/* close rep ****/
		(void) closedir (rep) ;
	} else { // else program shutdown
		printf("Unable to load musicDir ! Check if your music repertory is called '%s'\n",musicDir); 
		exitr(); 
	}  

	/* init nb_song_display 
	* by default, songs are displayed by 9
	*/
	if (nbFile == 0) {
		printf("No file in %s directory ! Please put some.\n",musicDir);
		exitr();
	}

	/* Number of files to display */
	if (nbFile >= 10)
		nb_song_display=9;
	else 
		nb_song_display=nbFile;

	/* if too many files */
	if (nbFile > NB_FILES_MAX) {
		printf("Too many files in %s directory ! Please remove some.\n",musicDir);
		exitr();
	}
}

/* 
 * Increase timer each one second
 * */
void timePlay()
{
	/* If music is played ... */
	if (Mix_PlayingMusic() && !Mix_PausedMusic()) {
		/* ... we take the time */
		currentTime = SDL_GetTicks();
	}
	/* */
	if (currentTime - previousTime >= 1000) {
		counter += 1000; /* we add 1000ms to the time counter */
		previousTime = currentTime; /* Update previousTime */
	}
}

/* Reset the timer 
 * when a new song is played
 * */
void timeReset()
{
	previousTime = 0;
	counter = 0;
	minute=0;
}

/*
 * Display Backscreen
 * */
void displayBack()
{
	SDL_FreeSurface(background);
	background = SDL_LoadBMP("data/back.bmp");
	if (!background) {
		printf("%s\n", SDL_GetError());
		exitr();
	}
	SDL_BlitSurface(background, NULL, screen, 0);
}

/*
 * Display state of the player
 * */
void displayTime()
{
	char secondes[8] = "", minutes[8] = "00:00";
	SDL_Rect position;

	/* Display play/pause/stop icons */
	SDL_FreeSurface(icoPlay);
	icoPlay = SDL_LoadBMP(icons[numIcon]);
	if(!icoPlay) {
		printf("%s\n", SDL_GetError());
		exitr();
	}
	/* Set transparency */
	SDL_SetColorKey(icoPlay, SDL_SRCCOLORKEY, SDL_MapRGB(icoPlay->format, 255, 255, 255));
	SDL_SetAlpha(icoPlay, SDL_SRCALPHA, 128);
	position.x = 29;
	position.y = 150;
	SDL_BlitSurface(icoPlay, NULL, screen, &position);

	/* Display Timer */
	second=counter/1000;
	/* when 60s, pass to 1 min */
	if (second+1 > 60) {
		second=0;
		counter=0; 
		minute++;
	}
	
	/* if minute or second <10, we put a 0 above ... */
	if (minute < 10)
		sprintf(minutes, "0%d", minute);
	else
		sprintf(minutes, "%d", minute);
	if (second < 10)
		sprintf(secondes, "0%d", second);
	else
		sprintf(secondes, "%d", second);

	/* add minute and second */
	strcat(minutes,":"); /* minutes= 00: */
	strcat(minutes,secondes); /* minutes= 00:00 */

	/* the real Display part */        
	SDL_FreeSurface(timer);
	timer = TTF_RenderText_Blended(fontTimer, minutes, cBlack);
	position.x = 73;
	position.y = 150;
	SDL_BlitSurface(timer, NULL, screen, &position); 
}

/* 
 * Display a volum icon and Volume
 * */
void displayVol()
{
	char volume[32] = ""; /* char "Volume: vol" */ //!!!
	SDL_Rect position;

	/* Display icoVol */
	SDL_FreeSurface(icoVol);
	icoVol = SDL_LoadBMP("data/vol.bmp");
	if (!icoVol) {
		printf("%s\n", SDL_GetError());
		exitr();
	}
	/* Set transparency */
	SDL_SetColorKey(icoVol, SDL_SRCCOLORKEY, SDL_MapRGB(icoVol->format, 255, 255, 255));
	SDL_SetAlpha(icoVol, SDL_SRCALPHA, 128);
	position.x = 35;
	position.y = 200;
	SDL_BlitSurface(icoVol, NULL, screen, &position);

	/* Display the Volume */
	int vol4 = vol/4;
	sprintf(volume, "Volume: %d ", vol4);
	SDL_FreeSurface(textVol);
	textVol = TTF_RenderText_Blended(fontVol, volume, cBlack);
	position.x = 71;
	position.y = 203;
	SDL_BlitSurface(textVol, NULL, screen, &position); 
}

/* 
 * Display an icon and the name of the song on the screen
 * */
void displaySong()
{
	SDL_Rect position;

	/* Display icoMusic */
	SDL_FreeSurface(icoMusic);
	icoMusic = SDL_LoadBMP("data/mus.bmp");
	if (!icoMusic) {
		printf("%s\n", SDL_GetError());
		exitr();
	}
	/* Set transparency */
	SDL_SetColorKey(icoMusic, SDL_SRCCOLORKEY, SDL_MapRGB(icoMusic->format, 255, 255, 255));
	SDL_SetAlpha(icoMusic, SDL_SRCALPHA, 128);
	position.x = 18;
	position.y = 80;
	SDL_BlitSurface(icoMusic, NULL, screen, &position);

	/* Display song name next to icoMusic */
	SDL_FreeSurface(textSong);
	textSong = TTF_RenderText_Blended(fontSong, tracks[numSongPlay], cBlack);
	position.x = 75;
	position.y = 85;
	SDL_BlitSurface(textSong, NULL, screen, &position); 
}

/*
 * Display music rep
 * */
void displayRep()
{
	/* Cleaning */
	int j;
	for (j=yRep;j<nb_song_display+yRep;j++) 
		SDL_FreeSurface(playlist[j]);

	SDL_Rect position;
	/* Writing musicDir name */  
	SDL_FreeSurface(nameRep);
	nameRep = TTF_RenderText_Blended(fontVol, musicDir, cBlack);
	position.x = 305;
	position.y = 68;
	SDL_BlitSurface(nameRep, NULL, screen, &position);

	/* Display the number of files in musicDir */
	SDL_FreeSurface(nbMusic);
	char files[32];
	sprintf(files, "(%d)", nbFile);
	nbMusic = TTF_RenderText_Blended(fontVol, files, cBlack);
	position.x = 430;
	position.y = 68;
	SDL_BlitSurface(nbMusic, NULL, screen, &position);

	/* */
	int i;int pos=0;
	if (nbSongHigh >= nb_song_display && next) if (yRep<(nbFile-nb_song_display)) {
							yRep++;
							next=0;
						}
	if (nbSongHigh <= nb_song_display && prev) if (yRep>0) {
							yRep--;
							prev=0;
						}

	for (i=yRep;i<nb_song_display+yRep;i++) {
	/* Highlight song selected by the cursor */ 
		if (i != nbSongHigh) 
			playlist[i] = TTF_RenderText_Shaded(fontRep, tracks[i], cBlack, cWhite);
		else 
			playlist[i] = TTF_RenderText_Shaded(fontRep, tracks[i], cBlack, cSong);
		position.x = 355;
		position.y = 95+pos*19;
		SDL_BlitSurface(playlist[i], NULL, screen, &position);
		pos++;
	}
}

/*
 * Display all the elements
 * */
void displayAll()
{
	/* Call the different display functions */
	displayBack();
	displayTime();
	displaySong();
	displayVol();
	displayRep();
	/* Refresh the Screen */
	SDL_Flip(screen);
}
