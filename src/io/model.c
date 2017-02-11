#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <GL/glew.h>

#include "log.h"
#include "model.h"
#include "IO_funcs.h"



/* 

	OBJ legend 

	usemtl					| MTL files 
	mtllib					| -||-
	# ...						| comment
	v f1 f2 f3 				| vertex (3 floats)
	vt f1 f2					| vertex texture (uv) coordinate (2 floats)
	vn f1 f2 f3				| vertex normal (3 floats)
	f a/b/c d/e/f g/h/i 	| face aka plane (intgers vertexnum/uvnum/normalnum)

*/


IO_stat_t
loadOBJ(const char* filename, obj_t* object)
{
	FILE* f;
	size_t vi=0, ui=0, ni=0; /* vertex, uv, and normal arrays indices*/
	size_t ivi=0, iui=0, ini=0;
	size_t i;
	char header[24];
	char buffer[1000];
	char newline[2];
	char *prevendp, *endp;


	f = fopen(filename, "r");
	if (f == NULL){
		return get_last_iostat();	
	}

	while( !feof(f) )
	{

		#define FILLFLOAT(where, counter, num) 									\
				prevendp = buffer;														\
				for(i=0; i<(num); ++i){													\
					(where)[(counter)++] = (float) strtod(prevendp, &endp);	\
					if(prevendp==endp) { fclose(f); return IO_FORMAT_ERROR;}	\
					prevendp = endp;														\
				}																				\

	fscanf(f, "%23s%*[\t ]%999[^\n]%1[\n]", header, buffer, newline);
	if (header[0] == '\0' && buffer[0] == '\0')
		continue;
	if (newline[0] == '\0')
		return IO_FORMAT_ERROR;

		// printf("Reading a string: %s || %s\n", header, buffer);
	#define ISHEAD(a) !strcmp(header, (a))

		if ( ISHEAD ("usemtl") )
			{
				useMTL(buffer);
				continue;
			}

		if ( ISHEAD ("mtllib") )
			{
				loadMTLlib(buffer);
				continue;
			}
		
		if (ISHEAD ("#"))
				continue;
		
		if ( ISHEAD ("v") && object->verts != NULL )
			{
				FILLFLOAT(object->verts, vi, 3);
				continue;
			}

		if ( ISHEAD ("vt") && object->uvs	!= NULL )
			{
				FILLFLOAT(object->uvs, ui, 2);
				continue;
			}

		if ( ISHEAD ("vn") && object->norms != NULL )
			{
				FILLFLOAT(object->norms, ni, 3);
				continue;
			}
		if ( ISHEAD ("f") )
			{
				#define READUINT(buf, index) 	(buf)[index++] = -1 + strtoul(prevendp, &endp, 10);\
														if (endp==prevendp) return IO_FORMAT_ERROR;
				prevendp = buffer;
				for (i=0; i<3; ++i)
				{
					/* TODO CORRECTLY*/
					READUINT(object->vert_inds, ivi);
					if (*endp == ' ')
						continue;
					prevendp = ++endp;
					// printf("<%s>\n", endp);
					if(*endp != '/')
						{
							READUINT(object->uv_inds, iui);
						}
					prevendp = ++endp;
					if(!isspace(*endp) && *endp!='\n')
						{
							READUINT(object->norm_inds, ini);
						}
					prevendp = ++endp;
					// printf("f: %u %u %u\n", object->vert_inds[ivi-1], object->uv_inds[iui-1], object->norm_inds[ini-1]);
					// scanf("%*c");
				}
				// printf("%lu %lu %lu\n", ivi, iui, ini);
				// exit(0);
			}
	}

	object->verts_cnt 	=	vi;
	object->uvs_cnt 		=	ui;
	object->norms_cnt 	= 	ni;

	object->vert_ind_cnt	= ivi;
	object->uv_ind_cnt 		= iui;
	object->norm_ind_cnt	= ini;

	wflog("INFO", "Read %s with %u nodes and %u faces", filename, vi, (ivi==0)?vi/3:ivi/3);

	/*printf("Indices:\n");
	for(i=0; i<ivi; i+=3)
		printf("%lu: %u/%u/%u %u/%u/%u %u/%u/%u\n", 
					i, object->vert_inds[i+0], object->uv_inds[i+0], object->norm_inds[i+0],
					object->vert_inds[i+1], object->uv_inds[i+1], object->norm_inds[i+1],
					object->vert_inds[i+2], object->uv_inds[i+2], object->norm_inds[i+2]
					);
	
	printf("\nVertices:\n");
	for(i=0; i<vi; i+=3)
		printf("%3.1f %3.1f %3.1f\n", vertices[i], vertices[i+1], vertices[i+2]);
	*/

	return IO_OK;

}

void
loadMTLlib(const char* name)
{
	/* placeholder*/
}

void
useMTL(const char* name)
{
	/* placeholder */
}