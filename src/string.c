#include "string.h"

#include <string.h>



/// @brief Iterates over an UTF8 string.
/// @param text The string on which iterate.
/// @note After the call to this function, the adress in *text will be 
/// incremented to correspond to the next UTF8 character.
/// @return The current character in the string.
int utf8_iterate(const char **text) {
	unsigned char c = **text;
	if (!c) return 0;
	if (c < 128) {
		++*text;
		return c;
	} else {
		++*text;
		unsigned char c2 = **text;
		++*text;
		return (c+(c2<<8))&0x0000FFFF;
	}
}

/// @brief Returns the len of an UTF8 string.
/// @param text The string to get the len from.
/// @return The len of the string.
int utf8_len(const char *text) {
	int n = 0;
	unsigned char c = 0;
	while ((c = *text)) {
		if (c < 128) ++text;
		else text += 2;
		n++;
	}
	return n;
}
