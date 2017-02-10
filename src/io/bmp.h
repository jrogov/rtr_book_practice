#ifndef  _BMP_H_
	#define _BMP_H_

#include <stdint.h>
#include <stdio.h>
#include "image.h"
#include "IO_funcs.h"

/*
1,4,8 bits per color - tables
16,32 - RGBA with 4/8 bits per pixel
24 - RGB with 8 bits per pixel
*/

typedef enum{
BI_RGB = 0,		/* No Compression */
BI_RLE8,		/* RLE 8-bit/pixel 					(Can be used only with 8-bit/pixel bitmaps) */
BI_RLE4,		/* RLE 4-bit/pixel 					(Can be used only with 4-bit/pixel bitmaps) */
BI_BITFIELDS,	/* OS22XBITMAPHEADER: Huffman 1D, BITMAPV2INFOHEADER: RGB bit field masks, BITMAPV3INFOHEADER+: RGBA*/
BI_JPEG,		/* OS22XBITMAPHEADER: RLE-24, BITMAPV4INFOHEADER+: JPEG image for printing) */
BI_PNG,			/* BITMAPV4INFOHEADER+: PNG image for printing */
BI_ABITFIELDS,	/* RGBA bit field masks 			(only Windows CE 5.0 with .NET 4.0 or later) */
BI_CMYK,		/* No Compression					(only Windows Metafile CMYK) */
BI_CMYKRLE8,	/* RLE-8 							(only Windows Metafile CMYK) */
BI_CMYKRLE4		/* RLE-4 							(only Windows Metafile CMYK) */
} bmp_compression;


#pragma pack(push, 2)
typedef struct bmp_header{
	uint16_t bfType; 			/* Signature = 0x42 0x4D*/
	uint32_t bfileSize;			/* BMP file size */
	uint32_t bfReserved;		/* 2 16bit reserved fields*/
	uint32_t bOffBits;			/* Offset of pixel array */
	uint32_t biSize;			/* Size of this header (40 bytes) */
	uint32_t biWidth;			/* Image width */
	uint32_t biHeight;			/* Image height */
	uint16_t biPlanes;			/* The number of color planes (must be 1) */
	uint16_t biBitCount;		/* Depth = Bits/pixel (1/4/8/16/24/32) */
	uint32_t biCompression; 	/* Compression */
	uint32_t biSizeImage;		/* Size of actual image data (pixels) */
	uint32_t biXPelsPerMeter; 	/* horizontal resolution, pixel/meter */
	uint32_t biYPelsPerMeter;	/* vertical resolution, pixel/meter */
	uint32_t biClrUsed;			/* number of palette colors (0<c<2^n???)*/
	uint32_t biClrImportant;	/* number of important colors (0 for every color)*/
} bmp_header_t;
#pragma pack(pop)

#define BMP_SIGNATURE 0x4D42
#define DIB_SIZE 14

IO_stat_t
load_BMP24(FILE*, image_t**, bmp_header_t**);

IO_stat_t
save_BMP24(image_t* image, FILE *f);

void print_header_info(FILE *f, bmp_header_t *header);
#endif		/* _BMP_H_ */
