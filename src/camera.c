#include "camera.h"

static void spectatorCamera(GLuint programID, movement_t *player ){

	static GLfloat angle[2] = {0.0f, sqrt(2)/2};

	GLfloat verticalAngle, horizontalAngle;

	static GLfloat FoV = FOV;

	static glm::vec3 Position = glm::vec3( 7, 5, 7 );
	static glm::vec3 realUp = glm::vec3(0, 1, 0);

	glm::vec3 Up;
	glm::vec3 Direction;
	glm::vec3 Right;

	glm::mat4 Model, View, Projection;
	glm::mat4 MVP;

	globlight_t light = {
									0.f, 0.f, 0.f,
									5 * 1.0f, 5*1.0f, 5*1.0f,
									10, 10, 10,
									mut_func_rotation
								};


	angle[0]+= 0.03;

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

	Up = glm::cross( Right, Direction );

	Position += glm::normalize(glm::cross(realUp, Right)) *	(player->fb);
	Position += Right * 		(player->rl);
	Position += realUp * 			(player->ud);

	 /* Swap Y and -Z for human-intuitive way */
	
	
	View = glm::lookAt(
							Position,
							Position+Direction,
							realUp
	);

	Projection = glm::perspective(FoV, ( (GLfloat) WIDTH)/HEIGHT, 0.1f, 100.0f);

	MVP = Projection * View * Model;
	glm::mat4 ViewModel = View * Model;

	glUseProgram(programID);



	#define SET_UMAT4(name, data)	glUniformMatrix4fv( glGetUniformLocation( programID, name ) , 1, GL_FALSE, &((data))[0][0]);
	#define SET_UVEC3(name, data) glUniform3fv( glGetUniformLocation( programID, name ) , 1, &(data)[0]);

	SET_UMAT4("View", View);
	SET_UMAT4("ViewModel", ViewModel)
	SET_UMAT4("MVP", MVP)

	light.mut_func(&light, angle);

	SET_UVEC3("W_Eye", Position+Direction);
	SET_UVEC3("W_LightDir", (const GLfloat*) &light.pos);

}


void 
__debug_print_movement__(FILE* f, movement_t *m)
{
	printf(
		"\ttCamX\tCamY\n"
		"\t%.2f\t%.2f\n"
		"\tF_B\tR_L\tU_D\n"
		"\t%.2f\t%.2f\t%.2f\n",
		m->camX, m->camY,
		m->fb, m->rl, m->ud
		);
}

