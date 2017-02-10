#include <stdint.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "IO_funcs.h"
#include "bmp.h"
#include "image.h"


#define ROW_GARBAGE_LENGTH(d, w) (4 - (((d * w) >> 3) & 3))&3

IO_stat_t
load_BMP24(FILE *f, image_t** image, bmp_header_t** header){
	bmp_header_t *h;
	image_t *i;
	uint32_t k;
	uint32_t count;

	uint32_t width, height, depth;
	uint32_t garbage_len;

	if (NULL==f)
		return IO_PARAM_ERROR;

	h = (bmp_header_t*) malloc(sizeof(*h));
	i = (image_t*) malloc(sizeof(*i));
	if (	NULL ==h
		||	NULL == i)
		errx(1, "[ERROR] Malloc error");
	if (1!=(fread(h, sizeof(*h), 1, f))){
		return IO_FORMAT_ERROR;
	}
	if (feof(f)){
		warnx("Unexpected end of file");
		return IO_FORMAT_ERROR;
	}

	if(h->bfType != BMP_SIGNATURE)
		return IO_BAD_HEADER;

	if (	(width 	= i->width 	= h->biWidth)		== 0
		||	(height 	= i->height = h->biHeight)   	== 0
		||	(depth 	= i->depth 	= h->biBitCount) 	== 0
		)

			return IO_BAD_HEADER;

	garbage_len = ROW_GARBAGE_LENGTH(depth, width); /*(4 - (((depth * width) >> 3) & 3))&3;*/

	i->pixels = (pixel_t*) malloc(sizeof(*i->pixels) * width * height);
	fseek(f, h->bOffBits, SEEK_SET);

	if (0 == garbage_len){
		count = width*height;
		if (count != (fread(i->pixels , sizeof(*i->pixels), count, f))){
			free(i);
			return IO_FORMAT_ERROR;
		}
	}
	else{
		for(k=0; k<height; ++k){
			count = width;
			if(count != fread(&i->pixels[k*width], sizeof(*i->pixels), count, f)){
				free(i);
				return IO_FORMAT_ERROR;
			}
			fseek(f, garbage_len, SEEK_CUR);
		}
	}

	if (NULL != header){
		*header = h;
	}
	else
		free(h);
	if (NULL != image)
		*image = i;
	else
		free(i);

	return IO_OK;
}

IO_stat_t
save_BMP24(image_t* image, FILE *f){
	bmp_header_t header;
	uint32_t garbage_len;
	uint32_t count;
	uint32_t retcount;
	uint32_t rawdata_size;

	if(	NULL == image
		|| 0 == image->width
		|| 0 == image->height
		|| NULL == f)
		return IO_FORMAT_ERROR;

	garbage_len = ROW_GARBAGE_LENGTH(image->depth, image->width);
	printf("\nGarbLen: %u\n", garbage_len);
	rawdata_size = ((image->depth * image->width)/8 + garbage_len) * image->height;

	header.bfType 			= BMP_SIGNATURE;
	header.bfileSize 		= rawdata_size	+ sizeof(header);
	header.bfReserved 		= 0x6777726d;
	header.bOffBits 			= sizeof(header);
	header.biSize 			= sizeof(header) - DIB_SIZE;
	header.biWidth 			= image->width;
	header.biHeight 			= image->height;
	header.biPlanes 			= 1;
	header.biBitCount 		= image->depth;
	header.biCompression	= BI_RGB;
	header.biSizeImage 		= rawdata_size;
	header.biXPelsPerMeter	= 3780;
	header.biYPelsPerMeter	= 3780;
	header.biClrUsed			= 0;
	header.biClrImportant	= 0;

	printf("Dest header:\n");
	print_header_info(stdout, &header);
	printf("\n");

	if(0 == garbage_len)

		retcount = fwrite(&header, sizeof(header), 1, f);

		/*if (1 != retcount){
			printf("%u\n", retcount);
			perror(NULL);
			warnx("IO Error");
			return FORMAT_IO_ERROR;
		}*/

		printf("CurPos: %lu\n",ftell(f));

		count = image->width * image->height;
		retcount = fwrite(image->pixels, sizeof(*image->pixels), count, f);
		if (count != retcount){
			warnx("IO Error");
			return IO_FORMAT_ERROR;
		}

	return IO_OK;
}


void print_header_info(FILE *f, bmp_header_t *header){
	fprintf(f, "bfType: %u\n",
	header->bfType);

	fprintf(f, "bfileSize: %u\n",
	header->bfileSize);

	fprintf(f, "bfReserved: %u\n",
	header->bfReserved);

	fprintf(f, "bOffBits: %u\n",
	header->bOffBits);

	fprintf(f, "biSize: %u\n",
	header->biSize);

	fprintf(f, "biWidth: %u\n",
	header->biWidth);

	fprintf(f, "biHeight: %u\n",
	header->biHeight);

	fprintf(f, "biPlanes: %u\n",
	header->biPlanes);

	fprintf(f, "biBitCount: %u\n",
	header->biBitCount);

	fprintf(f, "biCompression: %u\n",
	header->biCompression);

	fprintf(f, "biSizeImage: %u\n",
	header->biSizeImage);

	fprintf(f, "biXPelsPerMeter: %u\n",
	header->biXPelsPerMeter);

	fprintf(f, "biYPelsPerMeter: %u\n",
	header->biYPelsPerMeter);

	fprintf(f, "biClrUsed: %u\n",
	header->biClrUsed);

	fprintf(f, "biClrImportant: %u\n",
	header->biClrImportant);
}

