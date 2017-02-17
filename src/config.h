#define HEIGHT 540
#define WIDTH 960

#define POSX 1080
#define POSY 0

#define MOUSE_SENSIVITY 2.0f
#define PLAYER_SPEED 0.5f
#define PLAYER_ACC 0.05f

#define LEVEL_SIZE 5.0f
#define CELL_HEIGHT ( 25.0f/2 )

#define FOV ( 70.0f / 180.0f * 3.14f )


static const GLfloat testLevelVertex[] = {
											1.0, -1.0, -1.0,
											1.0, -1.0, 1.0,
											-1.0, -1.0, 1.0,
											-1.0, -1.0, -1.0,
											1.0, 1.0, -1.0,
											1.0, 1.0, 1.0,
											-1.0, 1.0, 1.0,
											-1.0, 1.0, -1.0
													};
static const GLuint testLevelIndex[] = {
											5-1, 1-1, 4-1,
											5-1, 4-1, 8-1,

											3-1, 7-1, 8-1,
											3-1, 8-1, 4-1,

											2-1, 6-1, 3-1,
											6-1, 7-1, 3-1,

											1-1, 5-1, 2-1,
											5-1, 6-1, 2-1,

											5-1, 8-1, 6-1,
											8-1, 7-1, 6-1,

											1-1, 2-1, 3-1,
											1-1, 3-1, 4-1
													};


movement_t player = {
		1.25*3.14f,
		-0.4f,
		0.0,
		0.0,
		0.0,
		PLAYER_SPEED,
		PLAYER_SPEED,
		PLAYER_SPEED,
		MOUSE_SENSIVITY,
		MOUSE_SENSIVITY,
		PLAYER_ACC,
		PLAYER_ACC,
		PLAYER_ACC
	};

sprogram_info_t shader_info = { 
	"shaders/simple.vs.glsl",
	"shaders/simple.fs.glsl",
	NULL,
	NULL, NULL
};
