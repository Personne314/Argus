#pragma once

#include <stddef.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glew.h>


/// @struct Texture
/// @brief This is used to store an OpenGL Texture.
typedef struct {
	GLuint texture_id;	///< The texture id.
	size_t width;		///< The width of the texture (in pixels).
	size_t height;		///< The height of the texture (in pixels).
} Texture;


// Load an OpenGL texture from a file. 
Texture *texture_create(const char *path);

// Frees the memory allocated for a Texture.
void texture_free(Texture **p_texture);


// Binds the texture before using it.
void texture_bind(Texture *texture);
