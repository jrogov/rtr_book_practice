#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <GL/glew.h>

#include "log.h"
#include "obj.h"
#include "IO_funcs.h"
#include "shader.h"



/*

	OBJ legend

	usemtl					| MTL files
	mtllib					| -||-
	# ...						| comment
	v f1 f2 f3 				| vertex (3 floats)
	vt f1 f2					| vertex texture (uv) coordinate (2 floats)
	vn f1 f2 f3				| vertex normal (3 floats)
	f a/b/c d/e/f g/h/i 	| face aka plane (intgers vertexnum/uvnum/normalnum)

*/

#define MAX_ELEMENTS 40000
#define MAX_FACES 40000

#define VERT_LENGTH 3
#define UV_LENGTH 2
#define NORM_LENGTH 3
#define FACE_LENGTH 9

IO_stat_t
loadOBJ(const char* filename, obj_t* model)
{

	GLfloat verts[VERT_LENGTH * MAX_ELEMENTS];
	GLfloat uvs[UV_LENGTH * MAX_ELEMENTS];
	GLfloat norms[NORM_LENGTH * MAX_ELEMENTS];

	/* 
		structure as in f: v1/u1/n1 v2/u2/n2 v3/u3/n3 
		if no indice, then -1
	*/
	GLuint indices[FACE_LENGTH * MAX_FACES];
	
	FILE* f;
	size_t v_cnt=0, u_cnt=0, n_cnt=0; /* vertices, uvs, and normals */
	size_t ind_cnt=0;

	/* for buffer object size */
	size_t uv_ind_cnt=0, norm_ind_cnt=0; /* their indices. NOT an index for inserting */

	size_t i;
	char header[24];
	char buffer[1000];
	char newline[2];
	char *prevendp, *endp;


	f = fopen(filename, "r");
	if (f == NULL){
		return get_last_iostat();
	}

	for(;;)
	{

		#define FILLFLOAT(where, counter, num) 									\
				prevendp = buffer;														\
				for(i=0; i<(num); ++i){													\
					(where)[(counter)++] = (float) strtod(prevendp, &endp);	\
					if(prevendp==endp) { fclose(f); return IO_FORMAT_ERROR;}	\
					prevendp = endp;														\
				}																				\

		fscanf(f, "%23s%*[\t ]%999[^\n]%1[\n]", header, buffer, newline);
		if (feof(f))
			break;
		if (header[0] == '\0' && buffer[0] == '\0')
			continue;
		if (newline[0] == '\0')
			return IO_FORMAT_ERROR;
	
		// printf("Reading a string @%ld: %s || %s\n", ftell(f), header, buffer);
		#define ISHEAD(a) !strcmp(header, (a))

		if ( ISHEAD ("usemtl") )
			{
				useMTL(buffer);
				continue;
			}

		if ( ISHEAD ("mtllib") )
			{
				loadMTLlib(buffer);
				continue;
			}

		if (ISHEAD ("#"))
				continue;

		if ( ISHEAD ("v") && verts != NULL )
			{
				FILLFLOAT(verts, v_cnt, 3);
				continue;
			}

		if ( ISHEAD ("vt") && uvs != NULL )
			{
				FILLFLOAT(uvs, u_cnt, 2);
				continue;
			}

		if ( ISHEAD ("vn") && norms != NULL )
			{
				FILLFLOAT(norms, n_cnt, 3);
				continue;
			}
		if ( ISHEAD ("f") )
			{
				#define READUINT() {indices[ind_cnt++] = -1 + strtoul(prevendp, &endp, 10);\
												if (endp==prevendp) return IO_FORMAT_ERROR; }
				prevendp = buffer;
				for (i=0; i<3; ++i)
				{
					READUINT();
					if (*endp == ' ')
						continue;
					prevendp = ++endp;

					if(*endp != '/')
						{
							READUINT();
							uv_ind_cnt++;
						}
					else indices[ind_cnt++] = (GLuint) -1;
					prevendp = ++endp;
					
					if(!isspace(*endp) && *endp!='\n')
						{
							READUINT();
							norm_ind_cnt++;
						}
					else indices[ind_cnt++] = (GLuint) -1;
					prevendp = ++endp;

				}
			}
	}

	
	GLuint VAO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( 	GL_ARRAY_BUFFER, 
					 	sizeof(verts[0]) * (3 * ind_cnt + 2 * uv_ind_cnt + 3 * norm_ind_cnt),
					 	NULL,
					 	GL_STATIC_DRAW);

	wflog("INFO", "Allocation glBuffer of size %lu x %lu + %lu x %lu + %lu x %lu bytes", 
		ind_cnt/3, sizeof(verts[0]), 
		uv_ind_cnt, sizeof(uvs[0]), 
		norm_ind_cnt, sizeof(norms[0]));

	GLfloat* glBuffer = (GLfloat*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	size_t offset = 0;
	for (i = 0 ; i < (ind_cnt) ; )
	{
		GLuint vert_ind = VERT_LENGTH * indices[i++];
		GLuint uv_ind = indices[i++];
		GLuint norm_ind = indices[i++];

		glBuffer[offset++] = verts[vert_ind++];
		glBuffer[offset++] = verts[vert_ind++];
		glBuffer[offset++] = verts[vert_ind];


		if(uv_ind != (GLuint) -1)
		{
			uv_ind *= UV_LENGTH;
			glBuffer[offset++] = uvs[uv_ind++];
			glBuffer[offset++] = uvs[uv_ind];
		}
		if(norm_ind != (GLuint) -1)
		{
			norm_ind *= NORM_LENGTH;
			glBuffer[offset++] = norms[norm_ind++];
			glBuffer[offset++] = norms[norm_ind++];
			glBuffer[offset++] = norms[norm_ind];
		}
	}

	if (GL_FALSE == glUnmapBuffer(GL_ARRAY_BUFFER)){
		wflog("ERROR", "Failed buffer unmapping during loading %s", filename);
		return IO_EXT_FUNC_ERROR;
	}

	model-> VAO = VAO;
	model-> verts_cnt = ind_cnt 
								- uv_ind_cnt 
								- norm_ind_cnt;

	size_t 	vert_size = VERT_LENGTH * sizeof(verts[0]),
				uv_size = ( uv_ind_cnt != 0 ) ? UV_LENGTH * sizeof(uvs[0]) : 0,
				norm_size = ( norm_ind_cnt != 0 ) ? NORM_LENGTH * sizeof(norms[0]) : 0;

	model-> uv_offset = ( uv_size != 0 ) ? vert_size : -1;
	model-> normal_offset = ( norm_size != 0 ) ? vert_size + uv_size : -1;


	size_t stride = (vert_size + uv_size + norm_size);

	glEnableVertexAttribArray(LOC_POSITION);
	glVertexAttribPointer( LOC_POSITION, 
		3, GL_FLOAT, GL_FALSE, 
		stride, 
		(GLvoid*) 0 );	

	if(uv_ind_cnt!=0)
	{
		glEnableVertexAttribArray(LOC_UV);
		glVertexAttribPointer( LOC_UV, 
			3, GL_FLOAT, GL_FALSE, 
			stride, 
			(GLvoid*) model->uv_offset );	
	}

	if(norm_ind_cnt != 0)
	{
		glEnableVertexAttribArray(LOC_NORMAL);
		glVertexAttribPointer( LOC_NORMAL, 
			3, GL_FLOAT, GL_FALSE, 
			stride, 
			(GLvoid*) model->normal_offset );	
	}

	wflog("INFO", "Read %s with %u nodes and %u faces", 
				filename, 
				v_cnt, 
				(ind_cnt - uv_ind_cnt - norm_ind_cnt)/3);

	return IO_OK;

}



void
__debug_print_model_info(obj_t* model)
{
	printf("BufObject GLID = %u || count[VERT] = %lu | uv_offset = %ld | normal_offset = %ld\n",
				model-> VAO,
				model-> verts_cnt,
				(long int) model-> uv_offset,
				(long int) model-> normal_offset
	);
}

void
loadMTLlib(const char* name)
{
	/* placeholder*/
}

void
useMTL(const char* name)
{
	/* placeholder */
}


/* !!!!

for (i = 0;i < v_cnt; i+=3)
		printf("v %.6f %.6f %.6f\n", verts[i], verts[i+1], verts[i+2]);

	for (i = 0; i<n_cnt; i+=3)
		printf("vn %.6f %.6f %.6f\n", norms[i], norms[i+1], norms[i+2]);

	for (i=0; i<iv_cnt; i+=3)
		printf("f %u//%u %u//%u %u//%u\n", 
			1+vert_inds[i],
			1+norm_inds[i],
			1+vert_inds[i+1], 
			1+norm_inds[i+1], 
			1+vert_inds[i+2],
			1+norm_inds[i+2]);


*/