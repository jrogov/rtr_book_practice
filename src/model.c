#include <GL/glew.h>
#include <GL/glu.h>

#include "io/shader.h"

GLfloat PLANE[] = {
	-1.f, -1.f, 0.f, 0.f, 0.f,
	-1.f, 1.f, 0.f, 0.f, 1.f,
	1.f, 1.f, 0.f, 1.f, 1.f,
	1.f, -1.f, 0.f, 1.f, 0.f
};

#define VERT_OFFSET (GLvoid*) (0*sizeof(PLANE[0]))
#define UV_OFFSET (GLvoid*) (3*sizeof(PLANE[0]))
#define ATTRIB_SIZE (GLsizei)(5*sizeof(PLANE[0]))

/*
	Generates unit XY plane
	Convenient for textures generation
*/

GLuint gen_unitplane()
{
	GLuint vao;
	GLuint attrib;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &attrib);
	glBindBuffer(GL_ARRAY_BUFFER, attrib);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PLANE), PLANE, GL_STATIC_DRAW);

	glEnableVertexAttribArray(LOC_POSITION);
	glVertexAttribPointer(LOC_POSITION, 3, GL_FLOAT, GL_FALSE, ATTRIB_SIZE, VERT_OFFSET );

	glEnableVertexAttribArray(LOC_UV);
	glVertexAttribPointer(LOC_UV, 2, GL_FLOAT, GL_FALSE, ATTRIB_SIZE, UV_OFFSET);

	return vao;
}