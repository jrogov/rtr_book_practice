#include "camera.h"

static void rotateCamera(GLuint programID, movement_t *player , int32_t player_update ){

	//static float sublast_ticks = ((float) clock());
	//float current_ticks;
	//float cycle_time;
	//current_ticks = ((float) clock());
	//cycle_time = (current_ticks- last_ticks)/CLOCKS_PER_SEC;

	//static GLfloat mousespeed = 2.5f;
	static GLfloat angle = 0.0f;


	GLfloat verticalAngle, horizontalAngle;

	static GLfloat FoV = FOV;

	static glm::vec3 Position = glm::vec3( 0, 4, 5 );
	static glm::vec3 realUp = glm::vec3(0, 1, 0);

	glm::vec3 Up;
	glm::vec3 Direction;
	glm::vec3 Right;

	glm::mat4 Model, View, Projection;
	glm::mat4 MVP;


	angle+=0.03;

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

	player->fb = fabs(player->fb) < 0.01? 0: (player->fb + ( -2*(player->fb > 0) + 1 ) * 0. * player->accFB);
	player->rl = fabs(player->rl) < 0.01? 0: (player->rl + ( -2*(player->rl > 0) + 1 ) * 0. * player->accRL);
	player->ud = fabs(player->ud) < 0.01? 0: (player->ud + ( -2*(player->ud > 0) + 1 ) * 0. * player->accUD);

	 /* Swap Y and -Z for human-intuitive way */
	Model = glm::mat4(1.0);

	View = glm::lookAt(
							Position,
							Position+Direction,
							realUp
	);

	Projection = glm::perspective(FoV, ( (GLfloat) WIDTH)/HEIGHT, 0.1f, 100.0f);


	MVP = Projection * View * Model;

	glUseProgram(programID);

	GLint eye_loc = glGetUniformLocation( programID, "Eyecam");
	glUniform3fv(eye_loc, 1, &(Position+Direction)[0]);

	glm::mat4 ViewModel = View * Model;
	GLint m_loc = glGetUniformLocation( programID, "Model" );
	glUniformMatrix4fv(m_loc, 1, GL_FALSE, &(Model)[0][0]);
	GLint vm_loc = glGetUniformLocation( programID, "ViewModel" );
	glUniformMatrix4fv(vm_loc, 1, GL_FALSE, &(ViewModel)[0][0]);
	GLint mvp_loc = glGetUniformLocation( programID, "MVP" );
	glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &(Projection * ViewModel)[0][0]);

	GLint dir_loc = glGetUniformLocation(programID, "Dir");
	glUniform3f(dir_loc, 5*cos(angle), 5, 5*sin(angle));

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

