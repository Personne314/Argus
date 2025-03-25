#include "glyphs.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "font.h"
#include "string.h"



// List of all french diacritials.
const char *french_chars[30] = {
	"À", "à", "Â", "â", "Ç", "ç", "É", "é", "È", "è", "Ê", "ê", "Ë", "ë", "Î", 
	"î", "Ï", "ï", "Ô", "ô", "Œ", "œ", "Ù", "ù", "Û", "û", "Ü", "ü", "Ÿ", "ÿ"
};

/// @brief Gets the id in the glyphs set of the given char.
/// @param c The UTF8 from which to get the id.
/// @return The id of c in the glyphs set.
int id_from_char(int c) { 
	if (c >= 0 && c < 128) return c;
	switch (c) {
		case 50048: return 128;
		case 50080: return 129;
		case 50050: return 130;
		case 50082: return 131;
		case 50055: return 132;
		case 50087: return 133;
		case 50057: return 134;
		case 50089: return 135;
		case 50056: return 136;
		case 50088: return 137;
		case 50058: return 138;
		case 50090: return 139;
		case 50059: return 140;
		case 50091: return 141;
		case 50062: return 142;
		case 50094: return 143;
		case 50063: return 144;
		case 50095: return 145;
		case 50068: return 146;
		case 50100: return 147;
		case 50578: return 148;
		case 50579: return 149;
		case 50073: return 150;
		case 50105: return 151;
		case 50075: return 152;
		case 50107: return 153;
		case 50076: return 154;
		case 50108: return 155;
		case 50616: return 156;
		case 50111: return 157;
		default: return 0;
	}
}

/// @brief Creates a glyphs set from a ttf file.
/// @param size Font size used to generate the glyphs images.
/// @return The generated glyphs sets. 
Glyphs *glyphs_create(int size) {

	// Loads the font from memory.
	SDL_RWops *rw = SDL_RWFromMem((void*)UbuntuMono_ttf, UbuntuMono_len);
	if (!rw) {
        fprintf(stderr, "[ARGUS] error: failed to create RWops: %s\n", SDL_GetError());
		return NULL;
	}
	TTF_Font *font = TTF_OpenFontRW(rw, 1, size);
	if (!font) {
		fprintf(stderr, "[ARGUS] error: failed to load font: %s\n", TTF_GetError());
		return NULL;
	}

	// Malloc the glyphs set structure.
	Glyphs *glyphs = malloc(sizeof(Glyphs));
	if (!glyphs) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc the glyphs structure !\n");
		TTF_CloseFont(font);
		return NULL;
	}

	// Gets the glyph dimensions and initializes the rects.
	SDL_Rect glyph_rect, glyphs_rect;
	TTF_SizeText(font, "M", &glyph_rect.w, &glyph_rect.h);
	glyphs->ratio = (float)glyph_rect.w/glyph_rect.h;
	glyphs_rect.w = 16*glyph_rect.w;
	glyphs_rect.h = 10*glyph_rect.h;
	glyphs_rect.x = 0;
	glyphs_rect.y = 0;
	glyph_rect.x = 0;
	glyph_rect.y = 0;

	// Creates a surface for the glyphs set.
	SDL_Surface *surface = SDL_CreateRGBSurface(
		0, 16*glyph_rect.w, 10*glyph_rect.h, 32, 
		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
	);
	if (!surface) {
		fprintf(stderr, "[ARGUS]: error: unable to creates the glyphs surface !\n");
		TTF_CloseFont(font);
		free(glyphs);
		return NULL;
	}
	SDL_FillRect(surface, &glyphs_rect, SDL_MapRGBA(surface->format, 0,0,0,0));

	// For each ASCII character, creates its image and adds it to the main surface.
	char c[3] = {0,0,0};
	SDL_Color white = {0xFF,0xFF,0xFF,0xFF};
	for (int i = 0; i < 128; ++i) {
		c[0] = (char)i;
		if (!isgraph(c[0])) continue;

		// Creates the glyph image.
		SDL_Surface* temp = TTF_RenderText_Blended(font, c, white);
		SDL_Surface* glyph = SDL_ConvertSurface(temp, temp->format, SDL_PIXELFORMAT_RGBA8888);
		SDL_FreeSurface(temp);

		// Adds the image to the surface.
		SDL_Rect dest_rect = {(c[0]%16)*glyph_rect.w, (c[0]/16)*glyph_rect.h, glyph_rect.w, glyph_rect.h};
		SDL_BlitSurface(glyph, &glyph_rect, surface, &dest_rect);
		SDL_FreeSurface(glyph);

	}

	// For each french UTF8 character, creates its image and adds it to the main surface.
	for (int i = 0; i < 30; ++i) {
		c[0] = french_chars[i][0];
		c[1] = french_chars[i][1];

		// Creates the glyph image.
		SDL_Surface* temp = TTF_RenderUTF8_Blended(font, c, white);
		SDL_Surface* glyph = SDL_ConvertSurface(temp, temp->format, SDL_PIXELFORMAT_RGBA8888);
		SDL_FreeSurface(temp);

		// Adds the image to the surface.
		SDL_Rect dest_rect = {(i%16)*glyph_rect.w, (i/16 + 8)*glyph_rect.h, glyph_rect.w, glyph_rect.h};
		SDL_BlitSurface(glyph, &glyph_rect, surface, &dest_rect);
		SDL_FreeSurface(glyph);

	}
	TTF_CloseFont(font);

	// Generates the texture.
	glGenTextures(1, &glyphs->texture_id);
	glBindTexture(GL_TEXTURE_2D, glyphs->texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	return glyphs;
}

/// @brief Frees the memory allocated for a Glyphs.
/// @param p_glyphs A pointer to the pointer of the Glyphs to be freed. Cannot be NULL.
/// @note After freeing, the pointer *p_glyphs is set to NULL to avoid double-free.
void glyphs_free(Glyphs **p_glyphs) {
	Glyphs *glyphs = *p_glyphs;
	if (!p_glyphs) return;
	glDeleteTextures(1, &glyphs->texture_id);
	free(glyphs);
	*p_glyphs = NULL;
}

/// @brief Binds the texture of the glyphs.
/// @param glyphs The glyphs to bind.
void glyphs_bind(Glyphs *glyphs) {
	if (glyphs) glBindTexture(GL_TEXTURE_2D, glyphs->texture_id);
	else glBindTexture(GL_TEXTURE_2D, 0);
}

/// @brief Used to get the vertices of a glyph.
/// @param vertices Buffer to stores the vertices.
/// @param offset Offset in the buffer.
/// @param id id of the character from which to get the vertices.
/// @note vertices is supposed to be at least (offset + 12) * sizeof(float) bytes long.
void glyphs_get_vertices(float *vertices, int offset, int id) {
	float x = (id%16)*0.0625f;
	float y = (id/16)*0.1f;

	// Upper-left triangle.
	vertices[offset]	= x;
	vertices[offset+1]	= y;
	vertices[offset+2]	= x+0.0625f;
	vertices[offset+3]	= y+0.1f;
	vertices[offset+4]	= x;
	vertices[offset+5]	= y+0.1f;

	// Lower-right triangle.
	vertices[offset+6]	= x;
	vertices[offset+7]	= y;
	vertices[offset+8]	= x+0.0625f;
	vertices[offset+9]	= y;
	vertices[offset+10]	= x+0.0625f;
	vertices[offset+11]	= y+0.1f;

}

/// @brief Creates buffers containing data to render a text in a given rect.
/// @param glyphs The glyphs set to use.
/// @param p_rect Pointer on the rect that will contains the text.
/// @param text The text to render.
/// @return false if there was an error.
bool glyphs_generate_text_buffers(Glyphs *glyphs, Rect *p_rect, const char *text, 
float screen_ratio, float **vertices, float **textures, int *n) {

	// Initialize the rects.
	Rect rect = *p_rect;
	Rect glyph_rect = RECT_INIT;
	glyph_rect.h = rect.h;
	glyph_rect.w = rect.h*glyphs->ratio/screen_ratio;

	// Calculates the size and the position of the glyphs.
	*n = utf8_len(text);
	const float exceeding = (*n)*glyph_rect.w/rect.w;
	if (exceeding > 1.0f) {
		glyph_rect.w /= exceeding;
		glyph_rect.h /= exceeding;
		glyph_rect.x = rect.x;
		glyph_rect.y = rect.y + 0.5f*(rect.h-glyph_rect.h);
	} else {
		glyph_rect.x = rect.x + 0.5f*(rect.w-(*n)*glyph_rect.w);
		glyph_rect.y = rect.y;
	}

	// Malloc for the vertices buffers.
	*vertices = malloc(12*(*n)*sizeof(float));
	if (!*vertices) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a buffer for the vertices of a text VAO !\n");
		return false;
	}
	*textures = malloc(12*(*n)*sizeof(float));
	if (!*textures) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a buffer for the texture vertices of a text VAO !\n");
		free(*vertices);
		return false;
	}

	// For each character, adds it to the vertices lists.
	int c = 0;
	int i = 0;
	while ((c = utf8_iterate(&text))) {
		const int id = id_from_char(c)&0x000000FF;

		// Upper-left triangle.
		(*vertices)[12*i]		= glyph_rect.x;
		(*vertices)[12*i+1]		= glyph_rect.y;
		(*vertices)[12*i+2]		= glyph_rect.x + glyph_rect.w;
		(*vertices)[12*i+3]		= glyph_rect.y + glyph_rect.h;
		(*vertices)[12*i+4]		= glyph_rect.x;
		(*vertices)[12*i+5]		= glyph_rect.y + glyph_rect.h;

		// Lower-right triangle.
		(*vertices)[12*i+6]		= glyph_rect.x;
		(*vertices)[12*i+7]		= glyph_rect.y;
		(*vertices)[12*i+8]		= glyph_rect.x + glyph_rect.w;
		(*vertices)[12*i+9]		= glyph_rect.y;
		(*vertices)[12*i+10]	= glyph_rect.x + glyph_rect.w;
		(*vertices)[12*i+11]	= glyph_rect.y + glyph_rect.h;

		// Adds the texture vertices.
		glyphs_get_vertices(*textures, 12*i, id);
		glyph_rect.x += glyph_rect.w;
		++i;
	}
	return true;
}

/// @brief Creates buffers containing data to render a vertical text in a given rect.
/// @param glyphs The glyphs set to use.
/// @param p_rect Pointer on the rect that will contains the text.
/// @param text The text to render.
/// @return false if there was an error.
bool glyphs_generate_vertical_text_buffers(Glyphs *glyphs, Rect *p_rect, const char *text, 
float screen_ratio, float **vertices, float **textures, int *n) {

	// Initialize the rects.
	Rect rect = *p_rect;
	Rect glyph_rect = RECT_INIT;
	glyph_rect.h = rect.w*glyphs->ratio*screen_ratio;
	glyph_rect.w = rect.w;

	// Calculates the size and the position of the glyphs.
	*n = utf8_len(text);
	const float exceeding = (*n)*glyph_rect.h/rect.h;
	if (exceeding > 1.0f) {
		glyph_rect.w /= exceeding;
		glyph_rect.h /= exceeding;
		glyph_rect.x = rect.x + 0.5f*(rect.w-glyph_rect.w);
		glyph_rect.y = rect.y;
	} else {
		glyph_rect.x = rect.x;
		glyph_rect.y = rect.y + 0.5f*(rect.h-(*n)*glyph_rect.h);
	}
	glyph_rect.y += ((*n)-1)*glyph_rect.h; 

	// Malloc for the vertices buffers.
	*vertices = malloc(12*(*n)*sizeof(float));
	if (!vertices) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a buffer for the vertices of a text VAO !\n");
		return false;
	}
	*textures = malloc(12*(*n)*sizeof(float));
	if (!textures) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a buffer for the texture vertices of a text VAO !\n");
		free(vertices);
		return false;
	}

	// For each character, adds it to the vertices lists.
	int c = 0;
	int i = 0;
	while ((c = utf8_iterate(&text))) {
		const int id = id_from_char(c)&0x000000FF;

		// Upper-left triangle.
		(*vertices)[12*i]		= glyph_rect.x;
		(*vertices)[12*i+1]		= glyph_rect.y + glyph_rect.h;
		(*vertices)[12*i+2]		= glyph_rect.x + glyph_rect.w;
		(*vertices)[12*i+3]		= glyph_rect.y;
		(*vertices)[12*i+4]		= glyph_rect.x + glyph_rect.w;
		(*vertices)[12*i+5]		= glyph_rect.y + glyph_rect.h;

		// Lower-right triangle.
		(*vertices)[12*i+6]		= glyph_rect.x;
		(*vertices)[12*i+7]		= glyph_rect.y + glyph_rect.h;
		(*vertices)[12*i+8]		= glyph_rect.x;
		(*vertices)[12*i+9]		= glyph_rect.y;
		(*vertices)[12*i+10]	= glyph_rect.x + glyph_rect.w;
		(*vertices)[12*i+11]	= glyph_rect.y;

		// Adds the texture vertices.
		glyphs_get_vertices(*textures, 12*i, id);
		glyph_rect.y -= glyph_rect.h;
		++i;
	}
	return true;
}
	
/// @brief Generates a VAO from buffers to render text.
/// @param vertices Vertices buffer. It's size must be at least 12*nb_char*sizeof(float).
/// @param textures Textures buffer. It's size must be at least 12*nb_char*sizeof(float).
/// @param nb_char The number of characters in the buffers.
/// @return The generated VAO. 
VAO *glyphs_generate_text_vao(float *vertices, float *textures, int nb_char) {
	void *data[2] = {vertices, textures};
	int sizes[2] = {2,2};
	int gl_types[2] = {GL_FLOAT, GL_FLOAT};
	VAO *vao = vao_create(data, sizes, gl_types, 6*nb_char, 2);
	if (!vao) fprintf(stderr, "[ARGUS]: error: unable to create a VAO for a text buffer !\n");	
	return vao;
}
