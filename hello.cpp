#include <SDL/SDL.h>

#define FAIL(msg)					\
  fprintf(stderr, msg "SDL Error: %s\n", SDL_GetError());	\
  return -1


int main(int argc, char** argv)
{
  SDL_Surface* hello = NULL;
  SDL_Surface* screen = NULL;
  SDL_Init( SDL_INIT_EVERYTHING );

  screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
  hello = SDL_LoadBMP("imgs/helloworld.bmp");

  if(hello == NULL) {
    FAIL("Result of LoadBMP is null!\n");
  }
  
  SDL_BlitSurface( hello, NULL, screen, NULL );

  SDL_Flip( screen );
  SDL_Delay( 2000 );

  SDL_FreeSurface( hello );

  SDL_Quit();
  return 0;
}
