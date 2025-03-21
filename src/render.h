#pragma once

#include <stdbool.h>
#include "glyphs.h"
#include "shader.h"
#include "vao.h"


// Renders a text VAO.
void render_text(Glyphs *glyphs, VAO *vao, Color color);

// Renders a shape from a VAO with a given transparency.
void render_shape(VAO *vao, float transparency);

// Renders a curve from a VAO with a given transparency.
void render_curve(VAO *vao, bool continuous);
