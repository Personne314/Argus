#include "string.h"

#include <string.h>



/// @brief Iterates over an UTF8 string.
/// @param text The string on which iterate.
/// @note After the call to this function, the adress in *text will be 
/// incremented to correspond to the next UTF8 character.
/// @return The current character in the string.
uint32_t utf8_iterate(const char **text) {
	unsigned char c = *((*text)++);
	if (!c) return 0;
	if (c < 192) {
		return c;
	} else if (c < 224) {
		unsigned char c2 = *((*text)++);
		return (c2+(c<<8))&0x0000FFFF;
	} else {
		unsigned char c2 = *((*text)++);
		unsigned char c3 = *((*text)++);
		return (c3+(c2<<8)+(c<<16))&0x00FFFFFF;
	}
}


/// @brief Returns the len of an UTF8 string.
/// @param text The string to get the len from.
/// @return The len of the string.
size_t utf8_len(const char *text) {
	size_t n = 0;
	unsigned char c = 0;
	while ((c = *(text++))) if (c < 192) ++n;
	return n;
}
