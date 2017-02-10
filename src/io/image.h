#ifndef _IMAGE_H_
	#define _IMAGE_H_

#include <stdint.h>
#include "IO_funcs.h"

typedef struct pixel24 {
	uint8_t R,G,B;
} pixel_t;

typedef struct {
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	pixel_t *pixels;
} image_t;

typedef enum {
	CW = 0,
	CCW
} ROTATE_DIRECTION;

image_t* rotate90(image_t*, ROTATE_DIRECTION);
void print_image_ASCII(image_t*);
void free_image(image_t*);
#endif 		/* _IMAGE_H_ */