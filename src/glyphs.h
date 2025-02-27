#pragma once 

#include <GL/glew.h>
#include <stdbool.h>

#include "vao.h"
#include "structs.h"



/// @struct Glyphs
/// @brief Used to load and use a font as an atlas of textures.
struct Glyphs {
	GLuint texture_id;	///< OpenGL texture id.
	float ratio;		///< The glyph ratio of the loaded font (w/h).
};
typedef struct Glyphs Glyphs;


// Creates a glyphs set from a ttf file.
Glyphs *glyphs_create(int size);

// Frees a glyphs structure.
void glyphs_free(Glyphs *glyphs);

// Binds the texture of the glyphs.
void glyphs_bind(Glyphs *glyphs);

// Creates buffers containing data to render a text in a given rect.
bool glyphs_generate_text_buffers(Glyphs *glyphs, Rect *p_rect, 
const char *text, float screen_ratio, float **vertices, float **textures, int *n);

// Creates buffers containing data to render a vertical text in a given rect.
bool glyphs_generate_vertical_text_buffers(Glyphs *glyphs, Rect *p_rect,  
const char *text, float screen_ratio, float **vertices, float **textures, int *n);

// Generates a VAO from buffers to render text.
VAO *glyphs_generate_text_vao(float *vertices, float *textures, int nb_char);
