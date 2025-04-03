#pragma once

#include <stdbool.h>

#include "vao.h"
#include "curves.h"
#include "shader.h"
#include "glyphs.h"
#include "structs.h"
#include "axis.h"
#include "button.h"



/// @struct Graph
/// @brief Constains all the components to render a graph.
typedef struct {
	Axis x_axis;			///< X Axis.
	Axis y_axis;			///< Y Axis.
	Rect rect;				///< The rect of the graph.
	Rect grid_rect;			///< The rect of the grid of the graph.
	Color background_color;	///< The color of the background of the graph.
	Color graph_color;		///< The color of the graph.
	Color title_color;		///< The color of the title of the graph.
	Color text_color;		///< The color of all of the texts in the graph.
	Curves *curves;			///< List of the curves to draw.
	VAO *grid_vao;			///< VAO for the grid of the graph.
	VAO *background_vao;	///< VAO for the background of the graph.
	VAO *title_vao;			///< VAO for the graph title.
	ImageButton *save;		///< Button used to save the graph as a png.
	char *title;			///< The graph title.
} Graph;


// Creates a graph.
Graph *graph_create(Rect rect);

// Frees the memory allocated for a Graph.
void graph_free(Graph **p_graph);

// Prepares the static graphical components of a graph.
bool graph_prepare_static(Graph *graph, Glyphs *glyphs, int window_width, int window_height);

// Prepares the dynamic graphical components of a graph.
bool graph_prepare_dynamic(Graph *graph, Glyphs *glyphs, int window_width, int window_height);

// Frees the graphics components a the end of the render.
void graph_reset_graphics(Graph *graph);

// Renders the graph.
void graph_render(Graph *graph, Glyphs *glyphs);
