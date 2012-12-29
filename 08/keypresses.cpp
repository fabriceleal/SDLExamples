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

int main(int argc, char** argv)
{  
  SDL_Surface* screen = NULL;
  SDL_Surface* background = NULL;

  SDL_Surface* message = NULL;
  SDL_Surface* upMessage = NULL;
  SDL_Surface* downMessage = NULL;
  SDL_Surface* leftMessage = NULL;
  SDL_Surface* rightMessage = NULL;

  SDL_Event event;
  TTF_Font* font = NULL;
  SDL_Color textColor = { 255, 255, 255};

  bool quit = false;

  init(&screen, "Key Presses");

  background = load_image( "background.png" );
  font = load_font("DejaVuSans.ttf", 27);

  upMessage = TTF_RenderText_Solid(font, "Up was pressed.", textColor);
  if(upMessage == NULL) {
    FAIL_TTF("Error rendering up message.\n");
  }

  downMessage = TTF_RenderText_Solid(font, "Down was pressed.", textColor);
  if(downMessage == NULL) {
    FAIL_TTF("Error rendering down message.\n");
  }

  leftMessage = TTF_RenderText_Solid(font, "Left was pressed.", textColor);
  if(leftMessage == NULL) {
    FAIL_TTF("Error rendering left message.\n");
  }

  rightMessage = TTF_RenderText_Solid(font, "Right was pressed.", textColor);
  if(rightMessage == NULL) {
    FAIL_TTF("Error rendering right message.\n");
  }


  apply_surface(0,   0, background, screen);

  // update screen
  if(SDL_Flip( screen ) == -1)
    {
      FAIL_SDL("Error fliping screen.\n");
    }

  // wait for user exit
  while(quit == false) {
    while( SDL_PollEvent( &event ) ) {
      if( event.type == SDL_QUIT ) {
	quit = true;
      } else if( event.type == SDL_KEYDOWN ) {
	switch( event.key.keysym.sym ) {
	case SDLK_UP:
	  message = upMessage; break;
	case SDLK_DOWN:
	  message = downMessage; break;
	case SDLK_LEFT:
	  message = leftMessage; break;
	case SDLK_RIGHT:
	  message = rightMessage; break;
	}

	if(message != NULL) {
	  apply_surface(0, 0, background, screen);
	  apply_surface((SCREEN_WIDTH - message->w) / 2, 
			(SCREEN_HEIGHT - message->h) / 2,
			message, screen);
	  
	  message = NULL;
	}
	
	// update screen
	if(SDL_Flip( screen ) == -1)
	  {
	    FAIL_SDL("Error fliping screen.\n");
	  }

      }
    }
  }

  SDL_FreeSurface( upMessage );
  SDL_FreeSurface( downMessage );
  SDL_FreeSurface( leftMessage );
  SDL_FreeSurface( rightMessage );
  SDL_FreeSurface( background );

  TTF_CloseFont( font );
  
  TTF_Quit();
  
  SDL_Quit();

  return 0;
}
