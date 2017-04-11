#define HEIGHT 1080
#define WIDTH 1080

#define POSX 0
#define POSY 0

#define MOUSE_SENSIVITY 2.0f
#define PLAYER_SPEED 0.1f
#define PLAYER_ACC 0.05f

#define LEVEL_SIZE 5.0f
#define CELL_HEIGHT ( 25.0f/2 )

#define FOV ( 70.0f / 180.0f * 3.14f )

#include "movement.h"

movement_t player = {

		// 0., 1., 0.,
		0., 0., -1.,
		// 0.f, 5.f, -80.f,

		0.0, 0.0, 0.0,

		PLAYER_SPEED, PLAYER_SPEED, PLAYER_SPEED,
		PLAYER_ACC,	PLAYER_ACC, PLAYER_ACC,

		0,
		3.14f / 180 * 10,
		MOUSE_SENSIVITY, MOUSE_SENSIVITY
	};

sprogram_info_t shader_info = {
	"shaders/exp.vs.glsl",
	"shaders/exp.fs.glsl",
	NULL,
	NULL, NULL
};

globlight_t light[] = {
								{
									0.f, 0.f, 0.f,
									50 * 1.0f, 50*1.0f, 50*1.0f,
									// 30, 40, 70,
									0,2,0,
									mut_func_rotation
								}

							};

const char* MODEL_PATH = "res/obj/pool.obj";
// const char* MODEL_PATH = "res/obj/suzscene_sm.obj";