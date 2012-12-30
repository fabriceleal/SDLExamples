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


const int FRAMES_PER_SECOND = 20;
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

class Dot {
private:
  int x, y;
  int xVel, yVel;
public:
  Dot() {
    x = 0;
    y = 0;

    xVel = 0;
    yVel = 0;
  }

  void handle_input(SDL_Event& event) {
    //    std::cout << "handle input" << std::endl;
    if( event.type == SDL_KEYDOWN ) {
      switch( event.key.keysym.sym ) {
      case SDLK_UP:
	yVel -= Dot::DOT_HEIGHT / 2; break;
      case SDLK_DOWN:
	yVel += Dot::DOT_HEIGHT / 2; break;
      case SDLK_LEFT:
	xVel -= Dot::DOT_WIDTH / 2; break;
      case SDLK_RIGHT:
	xVel += Dot::DOT_WIDTH / 2; break;
      }
    } else if( event.type == SDL_KEYUP ) {
      switch( event.key.keysym.sym ) {
      case SDLK_UP:
	yVel += Dot::DOT_HEIGHT / 2; break;
      case SDLK_DOWN:
	yVel -= Dot::DOT_HEIGHT / 2; break;
      case SDLK_LEFT:
	xVel += Dot::DOT_WIDTH / 2; break;
      case SDLK_RIGHT:
	xVel -= Dot::DOT_WIDTH / 2; break;
      }
    }
  }

  void move() {
    x += xVel;
    if ( x < 0 || x + Dot::DOT_WIDTH > SCREEN_WIDTH ) {
      x -= xVel;
    }
    y += yVel;
    if( y < 0 || y + Dot::DOT_HEIGHT > SCREEN_HEIGHT ) {
      y -= yVel;
    }
  }

  void show(SDL_Surface* dot, SDL_Surface* screen) {
    apply_surface(x, y, dot, screen);
  }

  static const int DOT_HEIGHT = 36;
  static const int DOT_WIDTH = 37;
};

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
  SDL_Surface* dot = NULL;
  SDL_Event event;
  TTF_Font* font = NULL;
  SDL_Color textColor = { 255, 255, 255};

  bool quit = false;

  // Current frame
  //  int frame = 0;

  // The frame rate regulator
  Timer fps;

  // Timer used to update caption
  //Timer update;

  init( &screen, "Move the dot (up, left, down, right)" );

  font = load_font( "DejaVuSans.ttf", 27 );
  //message = TTF_RenderText_Solid( font, "Bla Bla", textColor );
  dot = load_image( "dot.png" );

  SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

  // update screen
  if(SDL_Flip( screen ) == -1) {
    FAIL_SDL("Error fliping screen.\n");
  }

  //  update.start();
  Dot theDot;


  // wait for user exit
  while(quit == false) {
    fps.start();

    while( SDL_PollEvent( &event ) ) {
      if( event.type == SDL_QUIT ) {
	quit = true;
      }

      theDot.handle_input( event );
    } // while(poll event)
    

    theDot.move();

    SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
    
    //apply_surface( (SCREEN_WIDTH / 2) - (message->w / 2), (SCREEN_HEIGHT / 2) - (message->h / 2), message, screen );
    theDot.show( dot, screen );
    
    // update screen
    if(SDL_Flip( screen ) == -1) {
      FAIL_SDL("Error fliping screen.\n");        
    }

    //    frame++;

    if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND ) {
      SDL_Delay( (1000 / FRAMES_PER_SECOND) - fps.get_ticks() );
    }

    /*if( update.get_ticks() > 1000 ) {
      std::stringstream caption;
      
      caption << "Average Frames Per Second " << frame / ( fps.get_ticks() / 1000.f );
      
      SDL_WM_SetCaption( caption.str().c_str(), NULL );
      
      update.start();
      }*/


  } // while(not quit)

  //  SDL_FreeSurface( <the_surface> );

  TTF_CloseFont( font );
  
  TTF_Quit();
  
  SDL_Quit();

  return 0;
}