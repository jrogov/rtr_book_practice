#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include <time.h>
#include <sys/stat.h>

#include <GL/glew.h>

#include "shader.h"

/*
NOTE: ONLY ASCII-chars in shader files
*/

/*
TODO:
create_program(strings) - for built-in-code shaders
fcreate_program(filenames) - for standalone shaders
*/

/* 128K for shader file currently */
#define BUFSIZE (128 * 1024)


typedef uint64_t hash_t;

struct shader_file_t{
	hash_t hash;
	time_t mtime;
	GLuint shader;
	GLenum type;
	struct shader_file_t* next;
};

static struct shader_file_t* sf_head = NULL;


static hash_t calc_shash(const char* s){
	hash_t h = 0;
	int i;
	for (i=0; s[i]!='\0'; i++)
		h = s[i]
				+ (h << ( (6 ^ i) & 7))
				+ (h << 16 )
				- (h>>2);
	return h;
}



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
			/* else reload it */
			else
				break;
		}
		shader_file = shader_file -> next;
	}

	/* load shader if not found/reload if changed*/
	io_status = load_shader_source(filename, &compiled_shader, type);

	if (io_status!=IO_OK)
		return io_status;

	/* if shader entry NOT found / first entry */
	if (shader_file == NULL ){
		shader_file = (struct shader_file_t *) malloc(sizeof(*shader_file));
		shader_file -> next	= sf_head;
		sf_head = shader_file;
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

IO_stat_t fload_program(
		const char* vertex_sfile,
		const char* fragment_sfile,
		const char* geometry_sfile,
		const char* tess_control_sfile,
		const char* tess_evaluation_sfile,
		GLuint *program
		){

	GLuint programID;
	GLuint vertexShader, geometryShader, fragmentShader, tessControlShader, tessEvalShader;
	GLint status;

	IO_stat_t io_status;

	programID = glCreateProgram();

	#define REPORT() if(io_status != IO_OK) return io_status;

	if(NULL != vertex_sfile)
		io_status = load_shader(vertex_sfile, &vertexShader, GL_VERTEX_SHADER);
	REPORT();
	glAttachShader( programID, vertexShader );


	if(NULL != fragment_sfile)
		io_status = load_shader(fragment_sfile, &fragmentShader, GL_FRAGMENT_SHADER);
	REPORT();
	glAttachShader( programID, fragmentShader );

	if(NULL != geometry_sfile)
		io_status = load_shader(geometry_sfile, &geometryShader, GL_GEOMETRY_SHADER);
	REPORT();
	glAttachShader( programID, geometryShader );

	if(NULL != tess_control_sfile)
		io_status = load_shader(tess_control_sfile, &tessControlShader, GL_TESS_CONTROL_SHADER);
	REPORT();
	glAttachShader( programID, tessControlShader);

	if(NULL != tess_evaluation_sfile)
		io_status = load_shader(tess_evaluation_sfile, &tessEvalShader, GL_TESS_EVALUATION_SHADER);
	REPORT();
	glAttachShader( programID, tessEvalShader);

	glLinkProgram( programID );
	glGetProgramiv( programID, GL_LINK_STATUS, &status);
	if ( status != GL_TRUE ) {
    *program = programID;
    return IO_EXT_FUNC_ERROR;
  	}

	/* clear this mess */
  /*
	if (NULL != vertex_sfile) {
		glDetachShader(programID, vertexShader);
		glDeleteShader(vertexShader);
	}

	if (NULL != geometry_sfile) {
		glDetachShader(programID, fragmentShader);
		glDeleteShader(fragmentShader);
	}

	if (NULL != fragment_sfile) {
		glDetachShader(programID, geometryShader);
		glDeleteShader(geometryShader);
	}

	if (NULL != tess_control_sfile) {
		glDetachShader(programID, tessEvalShader);
		glDeleteShader(tessEvalShader);
	}

	if (NULL != tess_evaluation_sfile) {
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
	GLchar *log;

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
		/*perror("Shader loading error: ");*/
		switch(errno){
			case ENOENT: return IO_NO_FILE_ERROR;
			case ENAMETOOLONG: return IO_NAME_LENGTH_ERROR;
		}
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

	/* DEBUG INFO! DELETE! ??? */
	log = buffer;
	glGetShaderInfoLog(s, 1000, NULL, log);
	if( status != GL_TRUE ){
		printf("%s\n", log);
		return IO_FORMAT_ERROR;
	}

	*shader = s;
	return IO_OK;
}