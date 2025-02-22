#pragma once

#include <stdbool.h>

#include "vao.h"
#include "shader.h"
#include "glyphs.h"
#include "structs.h"
#include "axis.h"



/// @brief Constains all the components to render a graph.
struct Graph {
	Axis x_axis;	// X Axis.
	Axis y_axis;	// Y Axis.
	Rect rect;		// The rect of the graph.
	Color background_color;	// The color of the background of the graph.
	Color graph_color;		// The color of the graph.
	Color title_color;		// The color of the title of the graph.
	Color text_color;		// The color of all of the texts in the graph.
	VAO *grid_vao;	// VAO for the grid of the graph.
	VAO *background_vao;	// VAO for the background of the graph.
	VAO *title_vao;		// VAO for the graph title.
	const char *title;	// The graph title.
};
typedef struct Graph Graph;


// Creates a graph.
Graph *graph_create(Rect rect);

// Frees a graph.
void graph_free(Graph *graph);

// Prepares the static graphical components of a graph.
bool graph_prepare_static(Graph *graph, Glyphs *glyphs, int window_width, int window_height);

// Prepares the dynamic graphical components of a graph.
bool graph_prepare_dynamic(Graph *graph);

// Frees the graphics components a the end of the render.
void graph_reset_graphics(Graph *graph);

// Renders the graph.
void graph_render(Graph *graph, Glyphs *glyphs);
