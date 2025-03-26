#include "texture.h"

#include <SDL2/SDL_image.h>



/// @brief Creates and OpenGL texture from a raw bytes array.
/// @param data The data of the image to use.
/// @param size The size of data.
/// @return The created texture or NULL in case of an error.
Texture *texture_create(const unsigned char *data, size_t size) {
		
	// Creates an RWops for IMG_Load_RW.
	SDL_RWops *rw = SDL_RWFromMem((void*)data, size);
	if (!rw) {
		fprintf(stderr, "[ARGUS]: error: unable to create an RWops during "
			"texture creation: %s\n", SDL_GetError());
		return NULL;
	}

	// Creates the image surface.
	SDL_Surface *surface = IMG_Load_RW(rw, 1);
	if (!surface) {
		fprintf(stderr, "[ARGUS]: error: unable to create the image surface "
			"during texture creation: %s\n", SDL_GetError());
		return NULL;
	}

	// Malloc the texture struct.
	Texture *texture = malloc(sizeof(Texture));
	if (!texture) {
		fprintf(stderr, "\e[0;31merror:\e[0m Impossible de malloc une structure de texture !\n");
		SDL_FreeSurface(surface);
		return NULL;
	}
	texture->width = surface->w;
	texture->height = surface->h;
	
	// Creates the texture.
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


/// @brief Binds a Texture.
/// @param texture The Texture to bind.
/// @note If texture == NULL, unbinds the currently used Texture.
void texture_bind(Texture *texture) {
	if (texture) glBindTexture(GL_TEXTURE_2D, texture->texture_id);
	else glBindTexture(GL_TEXTURE_2D, 0);
}
