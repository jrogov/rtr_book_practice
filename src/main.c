#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <GL/glew.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include "init.h"
#include "camera.h"
#include "light.h"

#include "io/shader.h"
#include "io/texture.h"
#include "io/obj.h"
#include "io/log.h"
#include "model.h"
#include "define.h"
#include "color.h"

#include "config.h"

SDL_Window *window;
unsigned int window_height = HEIGHT;
unsigned int window_width = WIDTH;
unsigned int window_posx = POSX;
unsigned int window_posy = POSY;

uint8_t quicksilver = 0;


static int32_t handleEvents( movement_t* );
static void rotateCamera(GLuint programID, movement_t *player );

texparam_t water_tex_params[] = {
	TEXPARAM( GL_TEXTURE_MAG_FILTER, GL_LINEAR ),
	TEXPARAM( GL_TEXTURE_MIN_FILTER, GL_LINEAR ),
	TEXPARAM( GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER ),
	TEXPARAM( GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER ),
	TEXPARAM_END
};

texparam_t image_tex_params[] = {
	TEXPARAM( GL_TEXTURE_MAG_FILTER, GL_LINEAR ),
	TEXPARAM( GL_TEXTURE_MIN_FILTER, GL_LINEAR ),
	TEXPARAM( GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT ),
	TEXPARAM( GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT ),
	TEXPARAM_END
};

/*
GL_CLAMP_TO_EDGE
GL_CLAMP_TO_BORDER
GL_MIRRORED_REPEAT
GL_REPEAT
GL_MIRROR_CLAMP_TO_EDGE
           */


GLuint initMultisample(int msaa_lvl)
{
	enum e_framebuffer {FB_COLOR, FB_DEPTHSTENCIL};

	GLuint tex[2];
	glGenTextures( 2, tex );
	glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, tex[FB_COLOR] );
	glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, msaa_lvl, GL_RGBA8, WIDTH, HEIGHT, false );

	glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, tex[FB_DEPTHSTENCIL] );
	glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, msaa_lvl, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, false );


	GLuint fbo;
	glGenFramebuffers( 1, &fbo );
	glBindFramebuffer( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex[FB_COLOR], 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, tex[FB_DEPTHSTENCIL], 0);

	return fbo;
}

void blitMultisample( GLuint fbo )
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glBlitFramebuffer(0, 0, WIDTH, HEIGHT,
							0, 0, WIDTH, HEIGHT,
							GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
							GL_NEAREST);
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo );
}

int main( int argc, char* argv[] ){

	int32_t events_status;
	IO_stat_t io_status;

	GLuint programID;

	obj_t model;

	float water_color[4];

	init_log(NULL);

	#define REPORT(s, code) if ( io_status != IO_OK ) wflog_fatal_error( (s), code);

	window = initWindow(WIDTH, HEIGHT, MODEL_PATH, window_posx, window_posy);

	io_status = fload_program(&shader_info, &programID);
	REPORT("Program load failed: %u", ECODE_GL);

	io_status = loadOBJ(MODEL_PATH, &model);
	REPORT("Obj load failed: %u", ECODE_IO);

	glUseProgram( programID );

	// texparam_t test[] = {
		// TEXPARAM( GL_TEXTURE_WRAP_T, 1 ),
		// TEXPARAM( GL_TEXTURE_WRAP_T, (GLfloat) 1.0f )
	// }

	// printf("0x%X %d\n", test[0].type, test[0].value.i);
	// printf("0x%X %f\n", test[1].type, test[1].value.f);

	GLuint normID, texID;
	fload_BMP_texture("res/obj/pool_c.bmp", image_tex_params, &texID);
	fload_BMP_texture("res/obj/pool_n.bmp", image_tex_params, &normID);
	nUNIFORM1(i, programID, "textureSampler", 0);
	nUNIFORM1(i, programID, "normalSampler", 1);

	printf("ERROR CODE = %d\n", glGetError()); //exit(0);


	glClearColor(0.5, 0.5, 0.5, 1);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LESS);
	glEnable (GL_BLEND);

	// glEnable(GL_MULTISAMPLE);
	// #define MSAA_LVL 4
	// GLuint fbo = initMultisample(MSAA_LVL);



	/* bind framebuffer for water texture */
	GLuint water_framebuffer = 0;
	glGenFramebuffers(1, &water_framebuffer);

	/* init resources for pingpong rendering of water */
	GLuint water_tex[2];
	water_tex[0] = gen_plane_tex(GL_RGBA32F_ARB, 256, 256, GL_FLOAT, water_tex_params);
	water_tex[1] = gen_plane_tex(GL_RGBA32F_ARB, 256, 256, GL_FLOAT, water_tex_params);


	// current write buffer index
	size_t write_buffer_id = 0;

	/* attach texture to framebuffer */
	glBindFramebuffer(GL_FRAMEBUFFER, water_framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, water_tex[0], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, water_tex[1], 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	/* init simple plane for drawing texture */

	GLuint water_vao = gen_unitplane();

	GLuint water_shader, water_update_shader, water_update_norm_shader, water_drop_shader;
	fload_simple_program("shaders/exp.vs.glsl", "shaders/water.fs.glsl", &water_shader);
	fload_simple_program("shaders/simple.vs.glsl", "shaders/water_update.fs.glsl", &water_update_shader);
	fload_simple_program("shaders/simple.vs.glsl", "shaders/water_update_norm.fs.glsl", &water_update_norm_shader);
	fload_simple_program("shaders/simple.vs.glsl", "shaders/water_drop.fs.glsl", &water_drop_shader);


	glUseProgram(water_shader);
	hex2float("1e33aa88", water_color);
	nUNIFORM1( i, water_shader, "textureSampler", 0);
	nUNIFORMV( 4f, water_shader, "water_color", water_color );

	glUseProgram(water_update_shader);
	nUNIFORM1(i, water_update_shader, "texture", 0);
	nUNIFORM2(f, water_update_shader, "delta", 1.0f/256, 1.0f/256);

	glUseProgram(water_update_norm_shader);
	nUNIFORM1(i, water_update_norm_shader, "texture", 0);
	nUNIFORM2(f, water_update_norm_shader, "delta", 1.0f/256, 1.0f/256);

	glUseProgram(water_drop_shader);
	nUNIFORM1(i, water_drop_shader, "texture", 0);
	nUNIFORM1(f, water_drop_shader, "radius", 0.03 );
	nUNIFORM1(f, water_drop_shader, "strength", 0.005 );

	time_t prev, last;
	time(&prev);
	size_t counter=0;

	#define PINGPONG() 	glDrawBuffer( GL_COLOR_ATTACHMENT0 + write_buffer_id ); \
								glBindTexture(GL_TEXTURE_2D, water_tex[write_buffer_id^=1]); \
								set_tex_params( GL_TEXTURE_2D, water_tex_params);

	glm::mat4 rotPlane = glm::rotate(
			glm::translate(
				glm::mat4(1),
				glm::vec3(0,0.7,0)),
			3.14f/2, glm::vec3(1, 0, 0));

	while(1){

		/*
		================================
		CONTROL-DRIVEN EVENTS
		================================
		*/

		{
			/* WATER DROP EVENT */

			glBlendFunc (GL_ONE, GL_ZERO);

			/* set new drop at random location */
			glBindFramebuffer(GL_FRAMEBUFFER, water_framebuffer);


			glViewport(0, 0, 256,256);

			glDisable(GL_CULL_FACE);

			glActiveTexture(GL_TEXTURE0);
			if (counter == 0)
			{
				glUseProgram(water_drop_shader);
				PINGPONG();

				GLfloat dropcoord[] = {
					((GLfloat) ( random() & 255)) / 255.f,
					((GLfloat) ( random() & 255)) / 255.f
				};

				glUniform2f( glGetUniformLocation(water_drop_shader, "center"),
				dropcoord[0],
				dropcoord[1]
				);
				printf("%.3f : %.3f\n", dropcoord[0], dropcoord[1]);

				glBindVertexArray(water_vao);

				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			}

		}





		/*
		================================
		TIME-DRIVEN EVENTS
		================================
		*/

		if (!quicksilver)
		{
			PINGPONG();
			glUseProgram(water_update_shader);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			PINGPONG();
			glUseProgram(water_update_norm_shader);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			if(counter & 4) {
				GLfloat data[4*4];
				glBindFramebuffer(GL_READ_FRAMEBUFFER, water_framebuffer);
				glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
				glReadPixels(127, 127, 2, 2, GL_RGBA, GL_FLOAT, data);
				printf("DATA");
				for( int i=0; i<4; ++i)
					printf("%.1f %.1f %.1f %.1f\n", data[i*4 + 0], data[i*4 + 1], data[i*4 + 2], data[i*4 + 3]);
			}
		}
		/* player update */

		glEnable(GL_CULL_FACE);


		glViewport(0,0, WIDTH, HEIGHT);
		glUseProgram(programID);

		// glBindFramebuffer( GL_FRAMEBUFFER, fbo );
		glBindFramebuffer( GL_FRAMEBUFFER, 0);


		/*
		================================
		RENDERING
		================================
		*/

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		/* OBJECTS */

		/* PLAYER CONTROL EVENT */

		glUseProgram(programID);
		events_status = handleEvents( &player );
		if ( -1 ==  events_status)
			return 0;
		if (events_status & 2){
			glDeleteProgram(programID);
			fload_program(&shader_info, &programID);
			//__debug_print_shader_cache__(stdout);
			glUseProgram(programID);
			nUNIFORM1(i, programID, "textureSampler", 0);
			nUNIFORM1(i, programID, "normalSampler", 1);
		}

		rotateCamera( programID, &player );

		glBindVertexArray(model.VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normID);
		glDrawArrays(GL_TRIANGLES, 0, model.verts_cnt);



		/* WATER */

		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, water_tex[write_buffer_id]);
		// set_tex_params(GL_TEXTURE_2D, water_tex_params);

		glUseProgram(water_shader);
		glm::mat4 VM = (player.View*rotPlane);
		nUNIFORMM( 4f, water_shader, "ViewModel", &(VM)[0][0]);
		nUNIFORMM( 4f, water_shader, "View", &(player.View)[0][0]);
		nUNIFORMM( 4f, water_shader, "MVP", &(player.Projection * VM)[0][0]);

		glBindVertexArray(water_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, water_tex[write_buffer_id]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();

		// blitMultisample(fbo);

		SDL_GL_SwapWindow( window );

		/* fps counter */

		// SDL_Delay(32);
		counter++;
		/*time(&last);
		if(last - prev >=5){
			wflog("INFO", "FPS: %.2f", ((double) counter)/(last-prev));
			prev = last;
			counter = 0;
		}*/
	}

}

float
fclamp(float value, float min, float max)
{
	return fmax(min, fmin(max, value));
}

/* -1 - quit, 0 - no updates, 1 - player update, 2 - reload shader */
static int32_t handleEvents( movement_t* player ){



	SDL_Event e;
	GLuint update = 0;

	static GLint
			f = 0, b = 0,
			r = 0, l = 0,
			u = 0, d = 0,
			shift = 0;
	static GLint
			focus = 1;
	static uint32_t fullscreen = 0;

	GLint xrel = 0,
			yrel = 0;

	#define BRAKE_PLAYER(ax, acc) \
			player->ax = fabs(player->ax) < 0.01 ? \
									0: \
									(player->ax + ( -2*(player->ax > 0) + 1 ) * 0.3 * player->acc)

	BRAKE_PLAYER(fb, accFB);
	BRAKE_PLAYER(rl, accRL);
	BRAKE_PLAYER(ud, accUD);

	while( SDL_PollEvent(&e) !=0 ){
		switch((e.type)){
			case SDL_QUIT:
				return -1;
			case SDL_MOUSEMOTION:
				if(focus)
				{
					xrel += e.motion.xrel;
					yrel += e.motion.yrel;
				}
				update = 1;
				break;
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym){
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
					case SDLK_LSHIFT:
						shift = 1;
					case SDLK_F1:
						update |= 2;
						break;
					case SDLK_F11:
						fullscreen ^= SDL_WINDOW_FULLSCREEN_DESKTOP;
						SDL_SetWindowFullscreen(window, fullscreen);
						break;
					case SDLK_TAB:
						quicksilver^=1;
				}
				update |= 1;
				break;

			case SDL_KEYUP:
				switch(e.key.keysym.sym){
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
					case SDLK_LSHIFT:
						shift = 0;
				}
				update |= 1;
				break;
			case SDL_WINDOWEVENT:
				switch (e.window.event)
				{
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						focus = 1;
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						focus = 0;
						break;

				}
		}
	}


	player->camX -= player-> camX_sens * ((GLfloat) xrel)/WIDTH;
	player->camY -= player-> camY_sens * ((GLfloat) yrel)/HEIGHT;
	player->camY = fclamp(player->camY, -3.14/2, 3.14/2);

	player->fb = fclamp( player->fb + (shift+1) * player->accFB * 2 * (f-b), (shift+1) * -player->speedFB, (shift+1) * player->speedFB );
	player->rl = fclamp( player->rl + (shift+1) * player->accRL * 2 * (r-l), (shift+1) * -player->speedRL, (shift+1) * player->speedRL );
	player->ud = fclamp( player->ud + (shift+1) * player->accUD * 2 * (u-d), (shift+1) * -player->speedUD, (shift+1) * player->speedUD );

	return update;
}


static void rotateCamera(GLuint programID, movement_t *player ){

	static GLfloat light_param[2] = {0.0f, sqrt(2)/2};
	static glm::vec3 Up = glm::vec3(0, 1, 0);

	static struct timeval last;

	GLfloat verticalAngle, horizontalAngle;

	glm::vec3 *Position;
	glm::vec3 Direction;
	glm::vec3 Right;
	glm::mat4 Model, View, Projection;

	glm::mat4 ViewModel, MVP;


	#define MAX_UTIME_VALUE (1e9-1)
	#define UTIME(t) ( (GLfloat) ( t.tv_sec % 1000000 * 10000 )  + (GLfloat (t.tv_usec / 100) ) )

	/* calculate data */
	GLfloat unow = - (UTIME(last));
	// printf("%f\n", unow);
	gettimeofday(&last, NULL);
	unow += ( UTIME(last) );

	// GLfloat mult = 1e-8;
	light_param[0] += 0.5 * ((GLfloat) unow) * 3.14 * 2 * 1e5 / MAX_UTIME_VALUE;
	// printf("%f\n", light_param[0]);

	horizontalAngle = player->camX ;
	verticalAngle 	= 	player->camY ;
	Direction = glm::vec3(
								cos(verticalAngle) * sin(horizontalAngle),
								sin(verticalAngle),
								cos(verticalAngle) * cos(horizontalAngle)
	);

	Right = glm::vec3(
					sin(horizontalAngle - 3.14f/2.0f),
					0,
					cos(horizontalAngle - 3.14f/2.0f)
	);

	Position = (glm::vec3*)(player);

	*Position += glm::normalize(glm::cross(Up, Right)) *	(player->fb)
				 + Right * (player->rl)
				 + Up * (player->ud);

	View = glm::lookAt(
							*Position,
							*Position+Direction,
							Up
	);


	Projection = glm::perspective(FOV, ( (GLfloat) window_width)/window_height, 0.1f, 1000.0f);


	ViewModel = View * Model;
	MVP = Projection * ViewModel;

	/* send data to server */

	player->Model = Model;
	player->View = View;
	player->Projection = Projection;

	glUseProgram(programID);

	nUNIFORMM( 4f, programID, "View", &View[0][0]);
	nUNIFORMM( 4f, programID, "ViewModel", &ViewModel[0][0]);
	nUNIFORMM( 4f, programID, "MVP", &MVP[0][0]);
	nUNIFORMM( 4f, programID, "Model", &Model[0][0]);

	light[0].mut_func(&light[0], light_param);

	// UVEC3("W_Eye", Position+Direction);
	// glGetUnifomBlockIndex(programID, "Light");


	nUNIFORMV( 3f, programID, "W_LightDir",  &light[0].pos[0]);

}

