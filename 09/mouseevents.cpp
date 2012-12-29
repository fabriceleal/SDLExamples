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

class Button
{
private:
  // Attributes of the button
  SDL_Rect box;

  // Sheet
  SDL_Surface* buttonSheet;

  // Button clips
  SDL_Rect* clips;

  // Button sprite that will be shown
  SDL_Rect clip;

public:
  // Initialize vars
  Button(int x, int y, int w, int h, SDL_Surface* theButtonSheet, SDL_Rect* theClips) : clips(theClips) {
    clip = clips[ Button::CLIP_MOUSEOUT ];
    buttonSheet = theButtonSheet;
    box.x = x;
    box.y = y;
    box.w = w;
    box.h = h;
  }

  // Handles events and set the button's sprite region
  void handle_events(SDL_Event& event) {
    int x = 0, y = 0;
    if(event.type == SDL_MOUSEMOTION) {
      y = event.motion.y;
      x = event.motion.x;

      if( (x > box.x)  &&
	  (x < box.x + box.w) &&
	  (y > box.y) &&
	  (y < box.y + box.h) 
	  ) {
	clip = clips[ Button::CLIP_MOUSEOVER ];
      } else {
	clip = clips[ Button::CLIP_MOUSEOUT ];
      }

    } else if( event.type == SDL_MOUSEBUTTONDOWN ) {
      if( event.button.button == SDL_BUTTON_LEFT ) {
	
	x = event.button.x;
	y = event.button.y;

	if( (x > box.x)  &&
	  (x < box.x + box.w) &&
	  (y > box.y) &&
	  (y < box.y + box.h) 
	  ) {
	  clip = clips[ Button::CLIP_MOUSEDOWN ];
	} // if inside box
      } // if left button click

    } else if( event.type == SDL_MOUSEBUTTONUP ){
      if( event.button.button == SDL_BUTTON_LEFT ){
	
	x = event.button.x;
	y = event.button.y;

	if( (x > box.x)  &&
	  (x < box.x + box.w) &&
	  (y > box.y) &&
	  (y < box.y + box.h) 
	  ) {
	  clip = clips[ Button::CLIP_MOUSEUP ];
	} // if inside box
      }

    }

   }

  // Shows the button on the screen
  void show(SDL_Surface* screen) {
    apply_surface(box.x, box.y, buttonSheet, screen, &clip);
  }

  static const int CLIP_MOUSEOVER = 0;
  static const int CLIP_MOUSEOUT = 1;
  static const int CLIP_MOUSEDOWN = 2;
  static const int CLIP_MOUSEUP = 3;

};

int main(int argc, char** argv)
{  
  SDL_Surface* screen = NULL;
  SDL_Surface* stuff = NULL;
  SDL_Event event;
  SDL_Rect clips[4];

  bool quit = false;

  init( &screen, "Mouse events" );

  stuff = load_image( "button.png" );

  clips[ Button::CLIP_MOUSEOVER ].x = 0;
  clips[ Button::CLIP_MOUSEOVER ].y = 0;
  clips[ Button::CLIP_MOUSEOVER ].w = 320;
  clips[ Button::CLIP_MOUSEOVER ].h = 240;

  clips[ Button::CLIP_MOUSEOUT ].x = 320;
  clips[ Button::CLIP_MOUSEOUT ].y = 0;
  clips[ Button::CLIP_MOUSEOUT ].w = 320;
  clips[ Button::CLIP_MOUSEOUT ].h = 240;

  clips[ Button::CLIP_MOUSEDOWN ].x = 0;
  clips[ Button::CLIP_MOUSEDOWN ].y = 240;
  clips[ Button::CLIP_MOUSEDOWN ].w = 320;
  clips[ Button::CLIP_MOUSEDOWN ].h = 240;

  clips[ Button::CLIP_MOUSEUP ].x = 320;
  clips[ Button::CLIP_MOUSEUP ].y = 240;
  clips[ Button::CLIP_MOUSEUP ].w = 320;
  clips[ Button::CLIP_MOUSEUP ].h = 240;

  Button theButton(170, 120, 320, 240, stuff, clips);

  // Paint the screen - white
  SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));

  // Update button
  theButton.show( screen );

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
      } 

      // Handle event 
      theButton.handle_events( event );
     
      // Paint the screen - white
      SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));

      // Update button
      theButton.show( screen );

      // update screen
      if(SDL_Flip( screen ) == -1)
	{
	  FAIL_SDL("Error fliping screen.\n");
	}

    }
  }
    
  SDL_Quit();

  return 0;
}
