#pragma once

#include <stdbool.h>

#include "graph.h"
#include "glyphs.h"


// Prepares the VAO for the grid static part.
bool grid_prepare_static(Graph *graph, Rect *p_grid_rect);

// Prepares the VAO for the grid dynamic part.
bool grid_prepare_dynamic(Graph *graph, Glyphs *glyphs, Rect *p_grid_rect, int window_width, int window_height);

