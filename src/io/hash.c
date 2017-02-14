#include "hash.h"

hash_t calc_shash(const char* s){
	hash_t h = 0;
	int i;
	for (i=0; s[i]!='\0'; i++)
		h = s[i]
				+ (h << ( (6 ^ i) & 7))
				+ (h << 16 )
				- (h>>2);
	return h;
}