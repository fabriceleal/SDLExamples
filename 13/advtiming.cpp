#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>

#define FAIL_SDL(msg)						\
  fprintf(stderr, msg "SDL Error: %s\n", SDL_GetError());	\
  exit(-1)

#define FAIL_IMG(msg)						\
  fprintf(stderr, msg "IMG Error: %s\n", IMG_GetError());	\
  exit(-1)

#define FAIL_TTF(msg)						\
  fprintf(stderr, msg "TTF Error: %s\n", TTF_GetError());	\
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
  if(SDL_Init( SDL_INIT_EVERYTHING ) == -1)
    {
      FAIL_SDL("Error initializing SDL.\n");
    }

  // Setup screen
  *screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
  if(*screen == NULL)
    {
      FAIL_SDL("Error setting up SDL\n");
    }

  // Initialize TTF Stuff too
  if( TTF_Init() == -1 ){
    FAIL_TTF("Error setting up TTF\n");
  }

  // Set window title
  SDL_WM_SetCaption(title.c_str(), NULL);
  
  return true;
}

class Timer {
private:
  int startTicks;
  int pausedTicks;

  bool paused;
  bool started;

public:
  // Init
  Timer() {
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
  }

  void start() {
    started = true;
    paused = false;
    startTicks = SDL_GetTicks();
  }

  void stop() {
    started = false;
    paused = false;
  }

  void pause() {
    if ( started && !paused ) {
      paused = true;
      pausedTicks = SDL_GetTicks();
    }
  }

  void unpause() {
    if ( paused ) {
      paused = false;
      startTicks = SDL_GetTicks() - pausedTicks;
      pausedTicks = 0;
    }
  }

  int get_ticks() {
    if ( started ){
      if ( paused ) {
	return pausedTicks;
      } else {
	return SDL_GetTicks() - startTicks;
      }
    }

    return 0;
  }

  bool is_started() {
    return started;
  }

  bool is_paused() {
    return paused;
  }
};

int main(int argc, char** argv)
{  
  SDL_Surface* screen = NULL;
  
  SDL_Surface* seconds = NULL;
  SDL_Surface* startStop = NULL;
  SDL_Surface* pauseMessage = NULL;

  SDL_Event event;
  TTF_Font* font = NULL;
  SDL_Color textColor = { 255, 255, 255};

  bool quit = false;
  
  init(&screen, "Advanced Timing");

  font = load_font("DejaVuSans.ttf", 27);

  // <surface> = TTF_RenderText_Solid(font, <text>, textColor);

  startStop = TTF_RenderText_Solid( font, "Press S to start or stop the timer", textColor );
  pauseMessage = TTF_RenderText_Solid( font, "Press P to pause or unpause the timer", textColor );

  SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

  // update screen
  if(SDL_Flip( screen ) == -1) {
    FAIL_SDL("Error fliping screen.\n");
  }


  Timer theTimer;
  theTimer.start();

  // wait for user exit
  while(quit == false) {
    while( SDL_PollEvent( &event ) ) {
      if( event.type == SDL_QUIT ) {
	quit = true;
      } else if( event.type == SDL_KEYDOWN ) {
	if( event.key.keysym.sym == SDLK_s ) {
	  if( theTimer.is_started() ) {
	    theTimer.stop();
	  } else {
	    theTimer.start();
	  } // theTimer.is_started()
	  
	} else if( event.key.keysym.sym == SDLK_p ) {
	  if( theTimer.is_paused() ) {
	    theTimer.unpause();
	  } else {
	    theTimer.pause();
	  } // theTimer.is_started()
	  
	} // if pressed ...
      } // if event.type == ...
    } // while(poll event)

    if( theTimer.is_started() ) {
      std::stringstream time;
      time << "Timer: "<< theTimer.get_ticks() / 1000.f;

      seconds = TTF_RenderText_Solid( font, time.str().c_str(), textColor );
      
      SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

      apply_surface( (SCREEN_WIDTH - startStop->w) / 2, 
		     50, startStop, screen);
      apply_surface( (SCREEN_WIDTH - pauseMessage->w) / 2, 
		     80, pauseMessage, screen);

      apply_surface( (SCREEN_WIDTH - seconds->w) / 2, 
		     0, seconds, screen);

      SDL_FreeSurface( seconds );
      seconds = NULL;
    }
    
    // update screen
    if(SDL_Flip( screen ) == -1) {
      FAIL_SDL("Error fliping screen.\n");        
    }

  } // while(not quit)

  //  SDL_FreeSurface( <the_surface> );

  TTF_CloseFont( font );
  
  TTF_Quit();
  
  SDL_Quit();

  return 0;
}
