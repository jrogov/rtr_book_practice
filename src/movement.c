#include "movement.h"

#include <SDL2/SDL.h>

extern int window_width;
extern int window_height;

static float fclamp(float value, float min, float max)
{
	return fmax(min, fmax(max(value)))
}

int
handle_movement(movement_t mv, SDL_Event e)
{
	switch(e.type)
			case SDL_MOUSEMOTION:
				player->camX -= 
					player-> camX_sens * 
					((GLfloat) e.motion.xrel)/window_width;
				
				player->camY -= 
					player-> camY_sens * 
					((GLfloat) e.motion.yrel)/window_height;

				player->camY = fclamp(player->camY, -3.14, 3.14);
				// player->camY<3.14/2?player->camY:3.14/2;
				// player->camY = player->camY>-3.14/2?player->camY:-3.14/2;

				update = 1;
				break;
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym)
				{
					case SDLK_w:
						f = 1; break;
					case SDLK_s:
						b = 1; break;
					case SDLK_d:
						r = 1; break;
					case SDLK_a:
						l = 1; break;
					case SDLK_SPACE:
						u = 1; break;
					case SDLK_LCTRL:
						d = 1; break;
				}
				return;
				break;

			case SDL_KEYUP:
				switch(e.key.keysym.sym)
				{
					case SDLK_w:
						f = 0; break;
					case SDLK_s:
						b = 0; break;
					case SDLK_a:
						l = 0; break;
					case SDLK_d:
						r = 0; break;
					case SDLK_SPACE:
						u = 0; break;
					case SDLK_LCTRL:
						d = 0; break;
				}
				update |= 1;
				break;

	player->fb = fclamp(player->fb + player->accFB * 2 * (f-b), -player->speedFB, player->speedFB);// fmax(-player->speedFB, fmin(player->speedFB, player->fb + player->accFB * 2 * (f-b)));
	player->rl = fclamp(player->rl + player->accRL * 2 * (r-l), -player->speedRL, player->speedRL);// fmax(-player->speedRL, fmin(player->speedRL, player->rl + player->accRL * 2 * (r-l)));
	player->ud = fclamp(player->ud + player->accUD * 2 * (u-d), -player->speedUD, player->speedUD);// fmax(-player->speedUD, fmin(player->speedUD, player->ud + player->accUD * 2 * (u-d)));

}
