#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <string>
#include <cstdarg>

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



/*bool cleanup(int count, ...)
{
  va_list stuff_to_free;
  va_start(stuff_to_free, count);

  for(SDL_Surface* free_me : stuff_to_free) {
    SDL_FreeSurface( free_me );
  }
  int j;
  for(j = 0; j < count; ++j) {
    SDL_Surface* to_free = va_arg(stuff_to_free, SDL_Surface*);
    SDL_FreeSurface(to_free);
  }

  va_end(stuff_to_free);

  SDL_Quit();
  }*/

int main(int argc, char** argv)
{  
  SDL_Surface* screen = NULL;
  SDL_Surface* background = NULL;
  SDL_Surface* message = NULL;
  SDL_Event event;
  TTF_Font* font = NULL;
  SDL_Color textColor = { 255, 255, 255};

  bool quit = false;

  init(&screen, "True Types Fonts");

  background = load_image( "background.png" );
  font = load_font("DejaVuSans.ttf", 27);

  message = TTF_RenderText_Solid(font, "The quick brown foz jumps over the lazy dog.", textColor);
  if(message == NULL) {
    FAIL_TTF("Error rendering text.\n");
  }

  apply_surface(0,   0, background, screen);
  apply_surface(0, 150, message, screen);

  if(SDL_Flip( screen ) == -1)
    {
      FAIL_SDL("Error fliping screen.\n");
    }

  // wait for user exit
  while(quit == false) {
    while( SDL_PollEvent( &event ) ) {
      if(event.type == SDL_QUIT) {
	quit = true;
      }
    }
  }

  SDL_FreeSurface( background );
  SDL_FreeSurface( message );

  TTF_CloseFont( font );
  
  TTF_Quit();
  
  SDL_Quit();

  return 0;
}
