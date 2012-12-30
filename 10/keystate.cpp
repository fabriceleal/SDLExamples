#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <string>
#include <iostream>

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

  SDL_Surface* up = NULL;
  SDL_Surface* down = NULL;
  SDL_Surface* left = NULL;
  SDL_Surface* right = NULL;

  SDL_Event event;
  TTF_Font* font = NULL;
  SDL_Color textColor = { 255, 255, 255};

  bool quit = false;

  init(&screen, "Key States");

  font = load_font("DejaVuSans.ttf", 27);

  up = TTF_RenderText_Solid(font, "UP", textColor);
  if(up == NULL) {
    FAIL_TTF("Error rendering up message.\n");
  }

  down = TTF_RenderText_Solid(font, "DOWN", textColor);
  if(down == NULL) {
    FAIL_TTF("Error rendering down message.\n");
  }

  left = TTF_RenderText_Solid(font, "LEFT", textColor);
  if(left == NULL) {
    FAIL_TTF("Error rendering left message.\n");
  }

  right = TTF_RenderText_Solid(font, "RIGHT", textColor);
  if(right == NULL) {
    FAIL_TTF("Error rendering right message.\n");
  }


  SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

  // update screen
  if(SDL_Flip( screen ) == -1) {
    FAIL_SDL("Error fliping screen.\n");
  }

  // wait for user exit
  while(quit == false) {
    while( SDL_PollEvent( &event ) ) {
      if( event.type == SDL_QUIT ) {
	quit = true;
      } 
    
      // There's also SDL_JoystickGetAxis(), SDL_GetModState() and SDL_GetMouseState()
      int keys;
      Uint8* keystates = SDL_GetKeyState( &keys ); 
      
      //std::cout << "There are " << keys << " keys" << std::endl;

      SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

      if( keystates[ SDLK_UP ] ) {
	//std::cout << "Pressed UP" << std::endl;

	apply_surface((SCREEN_WIDTH - up->w) / 2,
		      (SCREEN_HEIGHT - up->h) / 2 - up->h,
		      up, screen);
      }

      if( keystates[ SDLK_DOWN ] ) {
	//std::cout << "Pressed DOWN" << std::endl;
	
	apply_surface((SCREEN_WIDTH - down->w) / 2,
		      (SCREEN_HEIGHT - down->h) / 2 + down->h,
		      down, screen);
      }

      if( keystates[ SDLK_LEFT ] ) {
	//std::cout << "Pressed LEFT" << std::endl;

	apply_surface((SCREEN_WIDTH - left->w) / 2 - left->w,
		      (SCREEN_HEIGHT - left->h) / 2,
		      left, screen);
      }

      if( keystates[ SDLK_RIGHT ] ) {
	//std::cout << "Pressed RIGHT" << std::endl;

	apply_surface((SCREEN_WIDTH - right->w) / 2 + right->w,
		      (SCREEN_HEIGHT - right->h) / 2,
		      right, screen);
      }    

      // update screen
      if(SDL_Flip( screen ) == -1) {
	FAIL_SDL("Error fliping screen.\n");        
      }
    }
  }

  SDL_FreeSurface( up );
  SDL_FreeSurface( down );
  SDL_FreeSurface( left );
  SDL_FreeSurface( right );

  TTF_CloseFont( font );
  
  TTF_Quit();
  
  SDL_Quit();

  return 0;
}
