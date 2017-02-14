#ifndef _HASH_H_
#define _HASH_H_

#include "stdint.h"


typedef uint64_t hash_t;

hash_t calc_shash(const char* s);

#endif /* _HASH_H_ */

