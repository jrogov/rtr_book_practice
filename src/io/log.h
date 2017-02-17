#ifndef _SIMPLELOG_H_
#define _SIMPLELOG_H_

#include "error_codes.h"

#include <GL/glew.h>


/* NULL for stderr */
void
init_log(const char* filename);

void
close_log();

void
wlog(const char* tag, const char* message);

void
wflog(const char* tag, const char* msg_pattern, ...);

void
wlog_fatal_error(const char* msg);

void
wflog_fatal_error(const char* msg_pattern, error_code code, ...);

void
wlog_shader_infolog(GLuint shaderID, const char* filename);

void
wlog_sprogram_infolog(GLuint programID);

#endif /* "_SIMPLELOG_H_ */