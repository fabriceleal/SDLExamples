#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#define FAIL_SDL(msg)						\
  fprintf(stderr, msg "SDL Error: %s\n", SDL_GetError());	\
  exit(-1)

#define FAIL_IMG(msg)						\
  fprintf(stderr, msg "IMG Error: %s\n", IMG_GetError());	\
  exit(-1)

#define FAIL_TTF(msg)						\
  fprintf(stderr, msg "TTF Error: %s\n", TTF_GetError());	\
  exit(-1)

#define FAIL_MIX(msg)						\
  fprintf(stderr, msg, "Mix Error: %s\n", Mix_GetError());  \
  exit(-1)


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//using namespace std;

TTF_Font *load_font(std::string fontname, int size)
{
  TTF_Font* font = TTF_OpenFont(fontname.c_str(), size);
  if(font == NULL) {
    FAIL_TTF("Error loading font.\n");
  }
  return font;
}

SDL_Surface *load_image(std::string filename)
{
  SDL_Surface* loadedImage = NULL;
  SDL_Surface* optimizedImage = NULL;

  loadedImage = IMG_Load( filename.c_str() );

  if( loadedImage == NULL ) {
    FAIL_IMG("Error loading image.\n");
  }

  optimizedImage = SDL_DisplayFormat( loadedImage );
  if( optimizedImage == NULL) {
    FAIL_IMG("Error optimizing image\n");
  }
  SDL_FreeSurface( loadedImage );
  
  Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 200, 191, 231 );
  SDL_SetColorKey( optimizedImage , SDL_SRCCOLORKEY, colorkey );

  return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL)
{
  SDL_Rect offset;

  offset.x = x;
  offset.y = y;

  SDL_BlitSurface( source, clip, destination, &offset );
}

bool init(SDL_Surface** screen, std::string title)
{
  // Init SDL Stuff
  if(SDL_Init( SDL_INIT_EVERYTHING ) == -1) {
    FAIL_SDL("Error initializing SDL.\n");
  }

  // Setup screen
  *screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
  if(*screen == NULL) {
    FAIL_SDL("Error setting up SDL\n");
  }

  // Initialize TTF Stuff too
  if( TTF_Init() == -1 ){
    FAIL_TTF("Error setting up TTF\n");
  }

  if( Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) ) {
    FAIL_MIX("Error setting up Mix\n");
  }

  // Set window title
  SDL_WM_SetCaption(title.c_str(), NULL);
  
  return true;
}

int main(int argc, char** argv)
{  
  SDL_Surface* screen = NULL;
  SDL_Surface* background = NULL;

  Mix_Music *music = NULL;

  Mix_Chunk * scratch = NULL;
  Mix_Chunk * high = NULL;
  Mix_Chunk * med = NULL;
  Mix_Chunk * low = NULL;

  SDL_Event event;

  bool quit = false;

  init(&screen, "Sounds");

  background = load_image( "background.png" );
 
  music = Mix_LoadMUS( "music.wav" );
  if(music == NULL) {
    FAIL_MIX("Error loading music.\n");
  }

  scratch = Mix_LoadWAV( "scratch.wav" );
  if(scratch == NULL) {
    FAIL_MIX("Error loading scratch.\n");
  }

  high = Mix_LoadWAV( "high.wav" );
  if(high == NULL) {
    FAIL_MIX("Error loading high.\n");
  }

  med = Mix_LoadWAV( "med.wav" );
  if(med == NULL) {
    FAIL_MIX("Error loading med.\n");
  }

  low = Mix_LoadWAV( "low.wav" );
  if(low == NULL) {
    FAIL_MIX("Error loading low.\n");
  }

  apply_surface( 0, 0, background, screen );

  // update screen
  if(SDL_Flip( screen ) == -1) {
    FAIL_SDL("Error fliping screen.\n");
  }

  // wait for user exit
  while(quit == false) {
    while( SDL_PollEvent( &event ) ) {
      if( event.type == SDL_QUIT ) {
	quit = true;
      } else if( event.type == SDL_KEYDOWN ) {
	switch( event.key.keysym.sym ) {
	case SDLK_1:
	  if( Mix_PlayChannel(-1, scratch, 0 ) == -1 ) {
	    FAIL_MIX("Error playing scratch.\n");
	  }
	  break;
	case SDLK_2:
	  if( Mix_PlayChannel(-1, high, 0 ) == -1 ) {
	    FAIL_MIX("Error playing high.\n");
	  }
	  break;
	case SDLK_3:
	  if( Mix_PlayChannel(-1, med, 0 ) == -1 ) {
	    FAIL_MIX("Error playing med.\n");
	  }
	  break;
	case SDLK_4:
	  if( Mix_PlayChannel(-1, low, 0 ) == -1 ) {
	    FAIL_MIX("Error playing low\n");
	  }
	  break;
	case SDLK_9:
	  if( Mix_PlayingMusic() == 0) {
	    // Music is stopped

	    if( Mix_PlayMusic( music, -1 ) == -1 ) {
	      FAIL_MIX("Error playing music.\n");
	    }
	  } else {
	    // Music is not stopped, either paused or playing

	    if( Mix_PausedMusic() == 1) {
	      Mix_ResumeMusic();
	    } else {
	      Mix_PauseMusic();
	    }
 	  }
	  break;
	case SDLK_0:
	  Mix_HaltMusic();
	  break;
	} // switch(event.key.keysym.sym)
      } // if( eventtype == ..)
    } // while (poll event)
  } // while (not quit)

  SDL_FreeSurface( background );

  Mix_FreeChunk( scratch );
  Mix_FreeChunk( high );
  Mix_FreeChunk( med );
  Mix_FreeChunk( low );

  Mix_FreeMusic( music );

  Mix_CloseAudio();
  
  TTF_Quit();
  
  SDL_Quit();

  return 0;
}
