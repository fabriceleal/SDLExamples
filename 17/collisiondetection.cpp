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

bool check_collision( SDL_Rect a, SDL_Rect b ) {
  // The sides of the rectangles
  int left_a, left_b;
  int top_a, top_b;
  int right_a, right_b;
  int bottom_a, bottom_b;

  // Calc sides a
  left_a = a.x;
  right_a = a.x + a.w;
  top_a = a.y; 
  bottom_a = a.y + a.h;

  // Calc sides b
  left_b = b.x;
  right_b = b.x + b.w;
  top_b = b.y; 
  bottom_b = b.y + b.h;

  if( bottom_a <= top_b ) {
    return false;
  }

  if( top_a >= bottom_b ) {
    return false;
  }

  if( right_a <= left_b ) {
    return false;
  }

  if( left_a >= right_b ) {
    return false;
  }
  
  return true;
}

class Square {
private:
  SDL_Rect box;
  SDL_Rect* wall;
  int xVel, yVel;
  
public:
  Square(SDL_Rect* theWall) {
    box.x = box.y = 0;
    
    box.w = Square::SQUARE_WIDTH;
    box.h = Square::SQUARE_HEIGHT;
    
    wall = theWall;

    yVel = xVel = 0;    
  }

  static const int SQUARE_HEIGHT = 20;
  static const int SQUARE_WIDTH = 50;

  void handle_input(SDL_Event& event) {
    if( event.type == SDL_KEYDOWN ) {
      switch( event.key.keysym.sym ) {
      case SDLK_UP:
	yVel -= 10; break;
      case SDLK_DOWN:
	yVel += 10; break;
      case SDLK_LEFT:
	xVel -= 10; break;
      case SDLK_RIGHT:
	xVel += 10; break;
      }
    } else if( event.type == SDL_KEYUP ) {
      switch( event.key.keysym.sym ) {
      case SDLK_UP:
	yVel += 10; break;
      case SDLK_DOWN:
	yVel -= 10; break;
      case SDLK_LEFT:
	xVel += 10; break;
      case SDLK_RIGHT:
	xVel -= 10; break;
      }
    }   
  }

  void move() {
    box.x += xVel;

    if( box.x < 0 || box.x + Square::SQUARE_WIDTH > SCREEN_WIDTH || check_collision( box , *wall ) ) {
      box.x -= xVel;
    }

    box.y += yVel;

    if( box.y < 0 || box.y + Square::SQUARE_HEIGHT > SCREEN_HEIGHT || check_collision( box , *wall ) ) {
      box.y -= yVel;
    }

  }

  void show(SDL_Surface* screen) {
    SDL_FillRect( screen, &box, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
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

int main(int argc, char** argv)
{  
  SDL_Surface* screen = NULL; 
  SDL_Event event;
  SDL_Color textColor = { 255, 255, 255};

  SDL_Rect wall;
  wall.x = 300;
  wall.y = 40;
  wall.w = 40;
  wall.h = 400;

  Square theSquare(&wall);

  bool quit = false;

  // The frame rate regulator
  Timer fps;

  init( &screen, "Move the square (with collision detection) (up, left, down, right)" );

  SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

  // update screen
  if(SDL_Flip( screen ) == -1) {
    FAIL_SDL("Error fliping screen.\n");
  }

  // wait for user exit
  while(quit == false) {
    fps.start();

    while( SDL_PollEvent( &event ) ) {
      if( event.type == SDL_QUIT ) {
	quit = true;
      }

      theSquare.handle_input( event );
    } // while(poll event)
    

    theSquare.move();

    SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

    SDL_FillRect( screen, &wall, SDL_MapRGB(screen->format, 0x77, 0x77, 0x77));
    
    theSquare.show( screen );

    // update screen
    if(SDL_Flip( screen ) == -1) {
      FAIL_SDL("Error fliping screen.\n");        
    }

    if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND ) {
      SDL_Delay( (1000 / FRAMES_PER_SECOND) - fps.get_ticks() );
    }

  } // while(not quit)

  // SDL_FreeSurface( <the_surface> );

  // TTF_CloseFont( <the_font> );
  
  TTF_Quit();
  
  SDL_Quit();

  return 0;
}
