#include <stdint.h>

#include <stdio.h>

int tohex(const char c){
	if( c > 'F' )
		return c - 0x57;
	else if( c > '9' )
		return c - 0x37;
	else return c - '0';
}

void hex2float(const char* s, float* data)
{
	if ( '#' == s[0] ) ++s;

	char c;

	#define C2F(s) ( ( (float) ( (tohex(s[0]) << 4) + tohex(s[1])) ) / 255.0 )

	data[0] = C2F(s); s+=2;
	data[1] = C2F(s); s+=2;
	data[2] = C2F(s); s+=2;
	if( *s == '\0' ) return;
	data[3] = C2F(s);
}