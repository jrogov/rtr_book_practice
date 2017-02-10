#include "IO_funcs.h"

#include <errno.h>

#define CASE(a, b) case (a): return (b)

IO_stat_t
errno_to_iostat(int err)
{
	switch(err){
		CASE(ENOENT, 		 	IO_NO_FILE_ERROR);
		CASE(ENAMETOOLONG,	IO_NAME_LENGTH_ERROR);
		CASE(EFBIG, 			IO_FILE_SIZE_ERROR);
	}
	return IO_OK;
}

IO_stat_t
get_last_iostat()
{
	return errno_to_iostat(errno);
}

const char*
str_ioerror(IO_stat_t stat)
{
	switch(stat){
		CASE(IO_OK, "OK");
		CASE(IO_NO_FILE_ERROR, "File wasn't found");
		CASE(IO_NAME_LENGTH_ERROR, "Too long filename");
		CASE(IO_EMPTY_FILE_ERROR, "Empty file");
		CASE(IO_FILE_SIZE_ERROR, "File size error (for fixed buffers mostly)");
		CASE(IO_BAD_HEADER, "Bad header");
		CASE(IO_FORMAT_ERROR, "File format error");
		CASE(IO_PARAM_ERROR, "IO function params");
		CASE(IO_EXT_FUNC_ERROR,	 "External function (using in current function) error");
	}
	return "Wrong IO error ID";
}