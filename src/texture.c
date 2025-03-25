#include "texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>


/// @brief Load an OpenGL texture from a file. 
/// @param path The path to the file.
/// @return The created Texture struct.
Texture *texture_create(const char *path) {

	// Loads the image.
	SDL_Surface *surface = IMG_Load(path);
	if (!surface) {
		fprintf(stderr, "[ARGUS]: error: unable to load image '%s'!\n", path);
		return NULL;
	}

	// Malloc the struct of the Texture.
	Texture *texture = malloc(sizeof(Texture));
	if (!texture) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a Texture struct!\n");
		SDL_FreeSurface(surface);
		return NULL;
	}
	texture->width = surface->w;
	texture->height = surface->h;

	// Creates the OenGL texture.
	glGenTextures(1, &texture->texture_id);
	texture_bind(texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	texture_bind(NULL);
	SDL_FreeSurface(surface);
	return texture;
}

/// @brief Frees the memory allocated for a Texture.
/// @param p_texture A pointer to the pointer of the Texture to be freed. Cannot be NULL.
/// @note After freeing, the pointer *p_texture is set to NULL to avoid double-free.
void texture_free(Texture **p_texture) {
	Texture *texture = *p_texture;
	if (!texture) return;
	glDeleteTextures(1, &texture->texture_id);
	free(texture);
	*p_texture = NULL;
}


/// @brief Binds the texture before using it.
/// @param texture The texture to bind.
/// @note If texture == NULL, the currently bound texture will be unbind.
void texture_bind(Texture *texture) {
	if (texture) glBindTexture(GL_TEXTURE_2D, texture->texture_id);
	else glBindTexture(GL_TEXTURE_2D, 0);
}
