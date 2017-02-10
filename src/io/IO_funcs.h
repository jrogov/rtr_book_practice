#ifndef IO_FUNCS_H
#define IO_FUNCS_H

typedef enum {
  IO_OK=0,
  IO_NO_FILE_ERROR,     /* File wasn't found */
  IO_NAME_LENGTH_ERROR, /* Too long filename */
  IO_EMPTY_FILE_ERROR,  /* Empty file */
  IO_FILE_SIZE_ERROR,	/* File size error (for fixed buffers mostly) */
  IO_BAD_HEADER,
  IO_FORMAT_ERROR, 		/* File format error */
  IO_PARAM_ERROR,   		/* IO function params error*/
  IO_EXT_FUNC_ERROR		/* External function (using in current function) error */
} IO_stat_t;

IO_stat_t
errno_to_iostat(int err);

IO_stat_t
get_last_iostat();

const char*
str_ioerror(IO_stat_t stat);

#endif /* IO_FUNCS_H */