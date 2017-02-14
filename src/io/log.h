#ifndef _SIMPLELOG_H_
#define _SIMPLELOG_H_

/* NULL for stderr */

void
init_log(const char* filename);

void
close_log();

void
wlog(const char* tag, const char* message);

void
wflog(const char* tag, const char* mes_pattern, ...);




#endif /* "_SIMPLELOG_H_ */