#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include <time.h>
#include <sys/stat.h>

#include <GL/glew.h>

#include "shader.h"
#include "hash.h"
#include "log.h"


/*
TODO:
create_program(strings) - for built-in-code shaders
fcreate_program(filenames) - for standalone shaders
*/

/* 128K for shader file currently */
#define BUFSIZE (128 * 1024)


struct shader_file_t{
	hash_t hash;
	time_t mtime;
	GLuint shader;
	GLenum type;
	struct shader_file_t* next;
};

static struct shader_file_t* sf_head = NULL;


static IO_stat_t load_shader_source(const char* filename, GLuint *shader, GLenum type);


IO_stat_t load_shader(const char* filename, GLuint *shader, GLenum type){
	struct stat buf;

	hash_t hash;
	time_t mtime;
	struct shader_file_t* shader_file;


	GLuint compiled_shader;
	IO_stat_t io_status;

	stat(filename, &buf);
	mtime = buf.st_mtime;
	hash = calc_shash(filename);

	shader_file = sf_head;

	while(shader_file != NULL){
		if ( shader_file->hash == hash ){
			/* return OK(0) if file's not changed/of the same type*/
			if ( shader_file->mtime == mtime && shader_file->type == type ){
				*shader = shader_file->shader;
				return IO_OK;
			}
			/* else re-/load it */
			else
				break;
		}
		shader_file = shader_file -> next;
	}

	/* load shader if not found/reload if changed */
	io_status = load_shader_source(filename, &compiled_shader, type);


	/* Attention: even though returned IO_OK, shader isn't reloaded: returned prev loaded one */
	if (io_status!=IO_OK)
	{
		if (shader_file != NULL){
			*shader = shader_file-> shader;
			return IO_OK;
		}
		return io_status;
	}

	/* if shader entry NOT found / INITIAL entry */
	if (shader_file == NULL ){
		shader_file = (struct shader_file_t *) malloc(sizeof(*shader_file));
		shader_file -> next	= sf_head;
		sf_head = shader_file;
	}
	/* else delete shader at shader_file */
	else
	{
		glDeleteShader(shader_file-> shader);
	}
	shader_file -> hash 	= hash;
	shader_file -> mtime = mtime;
	shader_file -> shader= compiled_shader;
	shader_file -> type	= type;
	*shader = compiled_shader;
	return IO_OK;
}



void __debug_print_shader_cache__(FILE* f){
	struct shader_file_t* current = sf_head;
	const char* sshader_type;

	printf("  StructAddr ||             Hash |   mtime  | GLID | Type | Next Address \n");
	while( current!=NULL ){


		switch(current->type){
			case GL_COMPUTE_SHADER: 			sshader_type = "CMP"; break;
			case GL_VERTEX_SHADER: 				sshader_type = "VRT"; break;
			case GL_TESS_CONTROL_SHADER: 		sshader_type = "TSC"; break;
			case GL_TESS_EVALUATION_SHADER: 	sshader_type = "TSE"; break;
			case GL_GEOMETRY_SHADER: 			sshader_type = "GEO"; break;
			case GL_FRAGMENT_SHADER: 			sshader_type = "FRG"; break;
			default: 								sshader_type = "NaS"; break; /* not a shader get it */
		}

		#define TIME_HMS(a) (((a)%86400)/3600),(((a)%3600)/60),((a)%60)
		fprintf(f, "   %08lx  || %016lx | %02ld:%02ld:%02ld | %4u |  %s | %08lx\n",
			(long unsigned)	current,
									current->hash,
						TIME_HMS(current->mtime),
									current->shader,
									sshader_type,
			(long unsigned) 	current->next);

		current = current->next;
	}
}

IO_stat_t 
fload_program( sprogram_info_t* info, GLuint *program)
{
	GLuint programID;
	GLint status;

	/* size for compile-error in case if sprogram_info_t changed */
	GLenum shader_types[sizeof(sprogram_info_t)/sizeof(const char*)] = {
		GL_VERTEX_SHADER,
		GL_FRAGMENT_SHADER,
		GL_GEOMETRY_SHADER,
		GL_TESS_CONTROL_SHADER,
		GL_TESS_EVALUATION_SHADER
	};

	/* corresponding to types above */
	GLuint shaders[sizeof(shader_types)/sizeof(shader_types[0])];

	IO_stat_t io_status;
	const char** info_name_p;
	size_t i; 

	programID = glCreateProgram();

	info_name_p = (const char**) info;
	for( i = 0; i < sizeof(shader_types)/sizeof(shader_types[0]); ++i)
	{

		if(NULL == *info_name_p)
			continue;
			
		io_status = load_shader(*info_name_p, &shaders[i], shader_types[i]);
		if(io_status != IO_OK) 
		{
			return io_status;
		}		
		
		glAttachShader( programID, shaders[i] );
		++info_name_p;
	}

		glLinkProgram( programID );
	glGetProgramiv( programID, GL_LINK_STATUS, &status);
	if ( status != GL_TRUE ) {
   	wlog_sprogram_infolog(programID);
   	return IO_EXT_FUNC_ERROR;
  	}


	*program = programID;
	return IO_OK;

	/* clear this mess */
  /*
	if (NULL != info->vertex_shader_file) {
		glDetachShader(programID, vertexShader);
		glDeleteShader(vertexShader);
	}

	if (NULL != info->geometry_shader_file) {
		glDetachShader(programID, fragmentShader);
		glDeleteShader(fragmentShader);
	}

	if (NULL != info->fragment_shader_file) {
		glDetachShader(programID, geometryShader);
		glDeleteShader(geometryShader);
	}

	if (NULL != info->tess_control_shader_file) {
		glDetachShader(programID, tessEvalShader);
		glDeleteShader(tessEvalShader);
	}

	if (NULL != info->tess_evaluation_shader_file) {
		glDetachShader(programID, tessControlShader);
		glDeleteShader(tessControlShader);
	}

*/

	*program = programID;
	return IO_OK;
}

static IO_stat_t load_shader_source(const char* filename, GLuint *shader, GLenum type){

	GLchar buffer[BUFSIZE];
	GLchar const *handle[1];

	FILE* f;
	GLuint s;
	GLint filesize;
	GLint status;

	switch(type){
		case GL_COMPUTE_SHADER:
		case GL_VERTEX_SHADER:
		case GL_TESS_CONTROL_SHADER:
		case GL_TESS_EVALUATION_SHADER:
		case GL_GEOMETRY_SHADER:
		case GL_FRAGMENT_SHADER:
			break;
		default:
			return IO_PARAM_ERROR;
	}


	f = fopen(filename, "r");
	if( NULL == f ) {
		
		wflog( "Failed to load shaderfile %s: %s", filename, str_ioerror(get_last_iostat()) ); 
		return get_last_iostat();
	}

	filesize = fread(buffer, sizeof(*buffer), sizeof(buffer), f);
	if (0==filesize) return IO_EMPTY_FILE_ERROR;
	if ( !feof(f) ) return IO_FILE_SIZE_ERROR;
	fclose(f);

	s = glCreateShader( type );
	handle[0] = buffer;
	glShaderSource( s, 1, handle, &filesize);

	glCompileShader( s );
	glGetShaderiv( s, GL_COMPILE_STATUS, &status );
	
	if( status != GL_TRUE ){
		wlog_shader_infolog(s, filename);
		return IO_FORMAT_ERROR;
	}
	*shader = s;
	return IO_OK;
}