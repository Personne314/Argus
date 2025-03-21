#pragma once

#include <stdbool.h>

#include "graph.h"
#include "glyphs.h"


// Prepares the VAO for the grid.
bool grid_prepare_graphics(Graph *graph, Glyphs *glyphs, Rect *p_grid_rect, int window_width, int window_height);

