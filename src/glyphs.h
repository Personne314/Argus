#pragma once 

#include <GL/glew.h>



/// @brief 
struct Glyphs {
	GLuint texture_id;	///< 
	float ratio;		///< 
	float tile_size;	///< 
};
typedef struct Glyphs Glyphs;


// Creates a glyphs set from a ttf file.
Glyphs *create_glyph_atlas(int size);

// Frees a glyphs structure.
void free_atlas(Glyphs *glyphs);

// Binds the texture of the glyphs.
void atlas_bind(Glyphs *glyphs);

// Used to get the vertices of a glyph.
void atlas_get_vertices(float *vertices, int offset, int c);
