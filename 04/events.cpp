#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <string>
#include <cstdarg>

#define FAIL_SDL(msg)						\
  fprintf(stderr, msg "SDL Error: %s\n", SDL_GetError());	\
  exit(-1)

#define FAIL_IMG(msg)						\
  fprintf(stderr, msg "IMG Error: %s\n", IMG_GetError());	\
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
    FAIL_IMG("Error loading image.\n");
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

  // Set window title
  SDL_WM_SetCaption(title.c_str(), NULL);
  
  return true;
}

bool cleanup(int count, ...)
{
  va_list stuff_to_free;
  va_start(stuff_to_free, count);

  /*for(SDL_Surface* free_me : stuff_to_free) {
    SDL_FreeSurface( free_me );
  }*/
  int j;
  for(j = 0; j < count; ++j) {
    SDL_Surface* to_free = va_arg(stuff_to_free, SDL_Surface*);
    SDL_FreeSurface(to_free);
  }

  va_end(stuff_to_free);

  SDL_Quit();
}

int main(int argc, char** argv)
{  
  SDL_Surface* screen = NULL;
  SDL_Surface* image = NULL;
  SDL_Event event;
  bool quit = false;

  init(&screen, "Event driven programming");

  image = load_image( "image.png" );
 
  apply_surface( 0, 0, image, screen);

  if(SDL_Flip( screen ) == -1)
    {
      FAIL_SDL("Error fliping screen");
    }

  // wait for user exit
  while(quit == false) {
    while( SDL_PollEvent( &event ) ) {
      if(event.type == SDL_QUIT) {
	quit = true;
      }
    }
  }

  cleanup(1, screen);

  return 0;
}
