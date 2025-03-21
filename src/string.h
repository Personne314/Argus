#pragma once

#include <stddef.h>
#include <stdint.h>


// Iterates over an UTF8 string.
uint32_t utf8_iterate(const char **text);

// Returns the len of an UTF8 string.
size_t utf8_len(const char *text);
