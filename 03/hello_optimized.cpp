#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <string>

#define FAIL(msg)					\
  fprintf(stderr, msg "SDL Error: %s\n", SDL_GetError());	\
  exit(-1)

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//using namespace std;

SDL_Surface *load_image(std::string filename)
{
  SDL_Surface* loadedImage = NULL;
  SDL_Surface* optimizedImage = NULL;

  loadedImage = IMG_Load( filename.c_str() );

  if( loadedImage == NULL ) {
    FAIL("Error loading image.\n");
  }

  optimizedImage = SDL_DisplayFormat( loadedImage );
  SDL_FreeSurface( loadedImage );

  return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination)
{
  SDL_Rect offset;
  offset.x = x;
  offset.y = y;

  SDL_BlitSurface( source, NULL, destination, &offset );
  
}

int main(int argc, char** argv)
{
  SDL_Surface* message = NULL;
  SDL_Surface* background = NULL;
  SDL_Surface* screen = NULL;
  
  if(SDL_Init( SDL_INIT_EVERYTHING ) == -1)
    {
      FAIL("Error initializing SDL.\n");
    }

  screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
  
  if(screen == NULL)
    {
      FAIL("Error setting up SDL\n");
    }
  
  SDL_WM_SetCaption("Hello World Optimized Image Loading and Blitting", NULL);

  message = load_image( "message.png" );
  background = load_image( "background.png" );

  apply_surface(   0,   0, background, screen );
  apply_surface( 320,   0, background, screen );
  apply_surface(   0, 240, background, screen );
  apply_surface( 320, 240, background, screen );

  apply_surface( 180, 140, message, screen);

  if(SDL_Flip( screen ) == -1)
    {
      FAIL("Error fliping screen");
    }

  SDL_FreeSurface( message );
  SDL_FreeSurface( background );

  SDL_Delay( 2000 );

  SDL_Quit();

  return 0;
}
