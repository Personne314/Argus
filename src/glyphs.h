#pragma once 

#include <GL/glew.h>

#include "vao.h"
#include "structs.h"



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

// Create a rect to render a text in a rect.
VAO *glyphs_generate_text_vao(Glyphs *glyphs, Rect *p_rect, const char *text);
