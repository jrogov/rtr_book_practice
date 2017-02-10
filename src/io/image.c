#include "image.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

image_t* rotate90(image_t* image, ROTATE_DIRECTION dir){
	uint32_t x, y;
	uint32_t width, height;
	image_t *result;

	if (	NULL == image
		||	(dir!=CW && dir!=CCW) )
		return NULL;

	width 	= image->width;
	height 	= image->height;

	result = (image_t*) malloc(sizeof(*image));
	result->width 	= height;
	result->height	= width;
	result->depth 	= image->depth;
	result->pixels = (pixel_t*) malloc((width * height * image->depth) /8 );

	switch (dir){
		case CCW:	for (x=0; x<width; ++x)
							for (y=0; y<height; ++y)
								result->pixels[height*x + y] =
									image->pixels[ (height-1-y)*width + x ];
						break;
		case CW:		for (x=0; x<width; ++x)
							for (y=0; y<height; ++y)
								result->pixels[height*x + y] =
									image->pixels[ (y)*width + (width-1-x) ];
						break;
		default:		return NULL;
	}

	return result;
}

void print_image_ASCII(image_t* image){
	uint32_t i, q;
	uint32_t width = image->width;
	pixel_t *pixel;
	uint8_t luma;

	for (i=image->height-1; i>0; --i){
		for (q=0; q<width; ++q){
			pixel = &(image->pixels[i*width+q]);
			luma = (uint8_t)(	0.299 * pixel->R +
									0.587 * pixel->G +
									0.114 * pixel->B );

			if 		(luma 	<36)	printf("  ");
			else if	(luma 	<72)	printf("--");
			else if	(luma 	<108)	printf("~~");
			else if	(luma 	<144)	printf("++");
			else if	(luma 	<180)	printf("oo");
			else if	(luma 	<216)	printf("##");
			else 					printf("@@");
		}
		printf("\n");
	}
}

void free_image(image_t *image){
	if(NULL==image)
		return;
	free(image->pixels);
	free(image);
}

/*
CW:
	x` = y
	y` = height - 1 - x

	x = height - 1 - y`
	y = x`

CCW:
	x` = width 	- 1	- y
	y` = x

	x = y`
	y = width - 1 - x`

*/