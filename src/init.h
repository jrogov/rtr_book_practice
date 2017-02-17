#ifndef _INIT_H_
#define _INIT_H_ 

#include <SDL2/SDL.h>
#include <stdint.h>

SDL_Window*
initWindow( unsigned int width, unsigned int height, 
				const char* name,
				unsigned int posx, unsigned int posy);

#endif /* _INIT_H_ */ 