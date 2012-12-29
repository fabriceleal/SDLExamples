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
  SDL_Surface* dots = NULL;
  SDL_Event event;
  SDL_Rect clip[4];

  bool quit = false;

  init(&screen, "Sprites");

  dots = load_image( "dots.png" );

  clip[0].x = 0;
  clip[0].y = 0;
  clip[0].w = 100;
  clip[0].h = 100;

  clip[1].x = 100;
  clip[1].y = 0;
  clip[1].w = 100;
  clip[1].h = 100;
 
  clip[2].x = 0;
  clip[2].y = 100;
  clip[2].w = 100;
  clip[2].h = 100;

  clip[3].x = 100;
  clip[3].y = 100;
  clip[3].w = 100;
  clip[3].h = 100;

  // Paint the screen - white
  SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));

  apply_surface(   0,   0, dots, screen, &clip[0] );
  apply_surface( 540,   0, dots, screen, &clip[1] );
  apply_surface(   0, 380, dots, screen, &clip[2] );
  apply_surface( 540, 380, dots, screen, &clip[3] );

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

  cleanup(2, dots, screen);

  return 0;
}
