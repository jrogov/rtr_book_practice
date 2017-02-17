#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>


#define FATAL_TAG "FATAL"
#define GL_LOG_BUFSIZE 1000


static FILE* output;

FILE*
get_output()
{
	return output;
}

void
init_log(const char* filename){

	if (output != NULL && output != stderr)
		close_log();

	if (filename != NULL)
	{
		output = fopen(filename, "a");
		if (output == NULL)
		{
			output = stderr;
			wflog("ERROR", "Log file \"%s\" couldn't be opened. (errno: %d)", filename, errno);
		}
	}
	else
		output = stderr;

	return;
}

void
close_log()
{
	if ( output != NULL && output != stderr)
		fclose(output);
}

#define MSG_HEADER " %lu [%s]: "

void
wlog(const char* tag, const char* message)
{
	time_t t;
	time(&t);
	fprintf(output, MSG_HEADER "%s\n", t, tag, message);
}

void
wflog(const char* tag, const char* msg_pattern, ...)
{
	va_list args;
	time_t t;

	time(&t);
	fprintf(output, MSG_HEADER, t, tag);

	va_start(args, msg_pattern);
	vfprintf(output, msg_pattern, args);
	fputc('\n', output);
	va_end(args);
}

void
wlog_fatal_error(const char* msg)
{
	init_log(NULL);
	wlog(FATAL_TAG, msg);
	exit(1);
}


/* a bit of overkill - copying entire wflog - but it's more convenient */


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvarargs"

/* I should actually kill myself for this, but error_code is used for both exit code and message, so... */

void
wflog_fatal_error(const char* msg_pattern, error_code code, ...)
{
	va_list args;
	time_t t;

	time(&t);
	fprintf(output, MSG_HEADER, t, FATAL_TAG);

	/* Attention: args actually start with error_code */
	va_start(args, msg_pattern);
	vfprintf(output, msg_pattern, args);
	fputc('\n', output);
	va_end(args);

	exit(code);
}

#pragma GCC diagnostic pop

void
wlog_shader_infolog(GLuint shaderID, const char* filename)
{
	char log_buffer[GL_LOG_BUFSIZE];
	glGetShaderInfoLog( shaderID, sizeof(log_buffer)/sizeof(log_buffer[0]), NULL, log_buffer);
	wflog("ERROR", "Failed loading shader %s.\n\n[OpenGL LOG]:\n%s\n", filename, log_buffer);
}

void
wlog_sprogram_infolog(GLuint programID)
{
	char log_buffer[GL_LOG_BUFSIZE];
	glGetProgramInfoLog( programID, sizeof(log_buffer)/sizeof(log_buffer[0]), NULL, log_buffer);
	wflog("ERROR", "Failed linking shader program. \n\n[OpenGL LOG]:\n%s\n", log_buffer);	
}