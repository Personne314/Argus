#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stddef.h>


/// @struct Texture
/// @brief Used to store an OpenGL texture.
struct Texture {
	size_t width;		//< The texture width.
	size_t height;		//< The texture height.
	GLuint texture_id;	//< The texture OpenGL id.
};
typedef struct Texture Texture;


// Creates and OpenGL texture from a raw bytes array.
Texture *texture_create(const unsigned char *data, size_t size);

// Frees the memory allocated for a Texture.
void texture_free(Texture **p_texture);


// Binds a Texture.
void texture_bind(Texture *texture);
