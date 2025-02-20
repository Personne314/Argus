#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"
#include "render.h"



/// @brief Creates a graph.
/// @param rect The rect containing the graph in the window.
/// @return The created graph.
Graph *graph_create(Rect rect) {
	Graph *graph = malloc(sizeof(Graph));
	if (!graph) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a Graph");
		return NULL;
	}
	graph->rect = rect;
	graph->background_color = COLOR_GRAY3;
	graph->graph_color = COLOR_GRAY9;
	graph->title_color = COLOR_WHITE;
	graph->background_vao = NULL;
	graph->title_vao = NULL;
	graph->title = "";
	return graph;
}

/// @brief Frees a graph.
/// @param graph The graph to free.
void graph_free(Graph *graph) {
	if (graph->background_vao) vao_free(graph->background_vao);
	if (graph->title_vao) vao_free(graph->title_vao);
	free(graph);
}

/// @brief Prepares the static graphical components of a graph.
/// @param graph The graph to prepare.
/// @param glyphs The glyphs set used to render text.
/// @param window_width The width of the window.
/// @param window_height The height of the window.
/// @return true if there was an error.
bool graph_prepare_static(Graph *graph, Glyphs *glyphs, int window_width, int window_height) {
	const float dx = 5.0f/window_width;
	const float dy = 5.0f/window_height;

	// Calculates the rects of the components.
	Rect title_rect = {
		graph->rect.x+dx, graph->rect.y+dy, 
		graph->rect.w-2*dx, 25.0f/window_height-2*dx
	};
	Rect graph_rect = {
		graph->rect.x+dx, graph->rect.y+25.0f/window_height+dy, 
		graph->rect.w-2*dx, graph->rect.h-25.0f/window_height-2*dy
	};

	// Creates the text VAO for the graph title.
	if (!graph->title || !strlen(graph->title)) {
		graph_rect.y = graph->rect.y+dy;
		graph_rect.h = graph->rect.h-2*dy;
		graph->title_vao = NULL;
	} else {
		graph->title_vao = glyphs_generate_text_vao(glyphs, &title_rect, graph->title);
		if (!graph->title_vao) {
			fprintf(stderr, "[ARGUS]: error: unable to create the VAO for the title of a graph !\n");
			return true;
		}
	}

	// Initializes the vertices for the background part.
	float vertices[24] = {
		graph->rect.x,graph->rect.y, 
		graph->rect.x+graph->rect.w,graph->rect.y+graph->rect.h, 
		graph->rect.x,graph->rect.y+graph->rect.h, 
		graph->rect.x,graph->rect.y, 
		graph->rect.x+graph->rect.w,graph->rect.y, 
		graph->rect.x+graph->rect.w,graph->rect.y+graph->rect.h,
		graph_rect.x,graph_rect.y, 
		graph_rect.x+graph_rect.w,graph_rect.y+graph_rect.h, 
		graph_rect.x,graph_rect.y+graph_rect.h, 
		graph_rect.x,graph_rect.y, 
		graph_rect.x+graph_rect.w,graph_rect.y, 
		graph_rect.x+graph_rect.w,graph_rect.y+graph_rect.h
	};
	
	// Initializes the colorsof the vertices for the background part.
	float colors[48];
	for (int i = 0; i < 6; ++i) {
		colors[4*i] =	graph->background_color.r;
		colors[4*i+1] =	graph->background_color.g;
		colors[4*i+2] =	graph->background_color.b;
		colors[4*i+3] =	1.0f;
	}
	for (int i = 0; i < 6; ++i) {
		colors[24+4*i] =	graph->graph_color.r;
		colors[24+4*i+1] =	graph->graph_color.g;
		colors[24+4*i+2] =	graph->graph_color.b;
		colors[24+4*i+3] =	1.0f;
	}

	// Creates the VAO for the background part.
	void *data[2] = {vertices, colors};
	int sizes[2] = {2,4};
	int array_ids[2] = {0,1};
	int gl_types[2] = {GL_FLOAT,GL_FLOAT};
	graph->background_vao = vao_create(data, sizes, array_ids, gl_types, 12,2);
	if (!graph->background_vao) {
		fprintf(stderr, "[ARGUS]: error: unable to create the VAO for the background of a graph !\n");
		return true;
	}
	return false;

}

/// @brief Prepares the dynamic graphical components of a graph.
/// @param graph The graph to prepare.
/// @note This has to be called before each graph_render call.
/// @return true if there was an error.
bool graph_prepare_dynamic(Graph *graph) {
	return false;
}

/// @brief Frees the graphics components a the end of the render.
/// @param graph The graph to reset.
void graph_reset_graphics(Graph *graph) {
	if (graph->background_vao) vao_free(graph->background_vao);
	if (graph->title_vao) vao_free(graph->title_vao);
	graph->background_vao = NULL;
	graph->title_vao = NULL;
}

/// @brief Renders the graph.
/// @param graph The graph to render.
/// @param glyphs The glyphs set to use to render texts.
void graph_render(Graph *graph, Glyphs *glyphs) {
	render_shape(graph->background_vao, 1.0f);
	render_text(glyphs, graph->title_vao, graph->title_color);
}
