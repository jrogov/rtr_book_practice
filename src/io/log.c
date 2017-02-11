#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

static FILE* output;

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

#define MSG_HEADER "%20lu [%s]: "

void 
wlog(const char* tag, const char* message)
{
	time_t t;
	time(&t);
	fprintf(output, MSG_HEADER "%s\n", t, tag, message);
}

void
wflog(const char* tag, const char* mes_pattern, ...)
{
	va_list args;
	time_t t;

	time(&t);
	fprintf(output, MSG_HEADER, t, tag);
	
	va_start(args, mes_pattern);
	vfprintf(output, mes_pattern, args);
	fputc('\n', output);
	va_end(args);
}
