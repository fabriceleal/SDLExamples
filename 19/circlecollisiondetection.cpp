#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

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


struct Circle {
  int x, y;
  int r;
};

double distance( int x1, int y1, int x2, int y2 )
{
  return sqrt( pow( x2 - x1, 2 ) + pow( y2 - y1, 2 ) );
}

bool check_collision( Circle& a, Circle& b)
{
  if( distance( a.x, a.y, b.x, b.y ) < (a.r + b.r) ) {
    return true;
  }
  return false;
}

bool check_collision( Circle& a, std::vector<SDL_Rect>& b ) {
  int cx, cy;

  for(int bBox = 0; bBox < b.size(); ++bBox ) {
    if( a.x < b[ bBox ].x ) {
      cx = b[ bBox ].x;
    } else if( a.x > b[ bBox ].x + b[ bBox ].w ) {
      cx = b[ bBox ].x + b[ bBox ].w;
    } else {
      cx = a.x;
    }

    if( a.y < b[ bBox ].y ) {
      cy = b[ bBox ].y;
    } else if( a.y > b[ bBox ].y + b[ bBox ].h ) {
      cy = b[ bBox ].y + b[ bBox ].h;
    } else {
      cy = a.y;
    }
    
    if( distance( a.x, a.y, cx, cy ) < a.r ) {
      return true;
    }    
  }

  return false;
}

bool check_collision( std::vector<SDL_Rect>& a, std::vector<SDL_Rect>& b ) {
  // The sides of the rectangles
  int left_a, left_b;
  int top_a, top_b;
  int right_a, right_b;
  int bottom_a, bottom_b;

  for( int aBox = 0; aBox < a.size(); ++aBox ) {

    // Calc sides a
    left_a = a[ aBox ].x;
    right_a = a[ aBox ].x + a[ aBox ].w;
    top_a = a[ aBox ].y; 
    bottom_a = a[ aBox ].y + a[ aBox ].h;  

    for( int bBox = 0; bBox < b.size(); ++bBox ) {

      // Calc sides b
      left_b = b[ bBox ].x;
      right_b = b[ bBox ].x + b[ bBox ].w;
      top_b = b[ bBox ].y; 
      bottom_b = b[ bBox ].y + b[ bBox ].h;

      if( (bottom_a <= top_b || 
	   top_a >= bottom_b || 
	   right_a <= left_b || 
	   left_a >= right_b) == false ) {
	return true;
      }
    }
  }
  
  return false;
}

class Dot {
private:
  Circle c;

  int xVel, yVel;

public:
  static const int DOT_WIDTH = 20;
  static const int DOT_HEIGHT = 20;

  Dot() {
    c.x = c.y = 10;

    xVel = yVel = 0;
  }

  void handle_input(SDL_Event& event) {
    switch( event.type ) {
    case SDL_KEYDOWN:
      switch( event.key.keysym.sym ) {
      case SDLK_UP:
	yVel -= 1;
	break;
      case SDLK_DOWN:
	yVel += 1;
	break;
      case SDLK_LEFT:
	xVel -= 1;
	break;
      case SDLK_RIGHT:
	xVel += 1;
	break;
      }
      break;
    case SDL_KEYUP:
      switch( event.key.keysym.sym ) {
      case SDLK_UP:
	yVel += 1;
	break;
      case SDLK_DOWN:
	yVel -= 1;
	break;
      case SDLK_LEFT:
	xVel += 1;
	break;
      case SDLK_RIGHT:
	xVel -= 1;
	break;
      }
      break;
    }
  }

  // Move the dot
  void move( std::vector<SDL_Rect>& rects, Circle& circle ) {
    c.x += xVel;

    if( c.x < 0 || c.x + Dot::DOT_WIDTH > SCREEN_WIDTH || 
	check_collision( circle, box )) {
      c.x -= xVel;
    }

    c.y += yVel;

    if( c.y < 0 || c.y + Dot::DOT_HEIGHT > SCREEN_HEIGHT || 
	check_collision( circle, box )) {
      c.y -= yVel;
    }

  }

  // Show dot on the screen
  void show(SDL_Surface* dot, SDL_Surface* screen) {
    apply_surface( x, y, dot, screen );
  }
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

const int DOT_WIDTH = 20;

int main(int argc, char** argv)
{  
  SDL_Surface* screen = NULL; 
  SDL_Surface* dot = NULL;
  SDL_Event event;
  SDL_Color textColor = { 255, 255, 255};

  bool quit = false;

  Dot theDot;
  std::vector<SDL_Rect> box(1);
  Circle otherDot;
  
  box[ 0 ].x = 60;
  box[ 0 ].y = 60;
  box[ 0 ].w = 40;
  box[ 0 ].h = 40;

  otherDot.x = 30;
  otherDot.y = 30;
  otherDot.r = DOT_WIDTH / 2;

  // The frame rate regulator
  Timer fps;

  init( &screen, "Move the dot (with circle collision detection) (up, left, down, right)" );

  SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

  // update screen
  if(SDL_Flip( screen ) == -1) {
    FAIL_SDL("Error fliping screen.\n");
  }

  dot = load_image( "dot.png" );

  // wait for user exit
  while(quit == false) {
    fps.start();

    while( SDL_PollEvent( &event ) ) {
      if( event.type == SDL_QUIT ) {
	quit = true;
      }

      theDot.handle_input(event);

    } // while(poll event)
    

    theDot.move( otherDot.get_rects()  );

    SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

    SDL_FillRect( screen, &box[0], SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF) );

    apply_surface( otherDot.x - otherDot.r , otherDot.y - otherDot.r, dot, screen );
    
    theDot.show(dot, screen);

    otherDot.show(dot, screen);
    theDot.show(dot, screen);

    // update screen
    if(SDL_Flip( screen ) == -1) {
      FAIL_SDL("Error fliping screen.\n");        
    }

    if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND ) {
      SDL_Delay( (1000 / FRAMES_PER_SECOND) - fps.get_ticks() );
    }

  } // while(not quit)
  
  TTF_Quit();
  
  SDL_Quit();

  return 0;
}
