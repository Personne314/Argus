#pragma once

#include <stdbool.h>
#include "glyphs.h"
#include "shader.h"
#include "vao.h"
#include "structs.h"
#include "texture.h"


// Renders a text VAO.
void render_text(Glyphs *glyphs, VAO *vao, Color color);

// Renders a shape from a VAO with a given transparency.
void render_shape(VAO *vao, float transparency);

// Renders a curve from a VAO with a given transparency.
void render_curve(VAO *vao, Color color, bool continuous);

// Renders a texture from a VAO.
void render_texture(VAO *vao, Texture *texture, float fade);
