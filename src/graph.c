#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "structs.h"
#include "render.h"
#include "grid.h"
#include "icons.h"
#include "screenshot.h"



/// @brief The save button action. Save a screen of a graph.
/// @param args The graph to screen and the glyphs used to render it.
void save_screnshot(void *args) {
	Graph *graph = *(Graph**)args;
	Glyphs *glyphs = *(Glyphs**)(args+sizeof(Graph*));
	const char *title = graph->title;
	if (!title || !strlen(title)) title = "graph";
	screenshot_graph(graph, glyphs, title);
}



/// @brief Creates a graph.
/// @param rect The rect containing the graph in the window.
/// @return The created graph.
Graph *graph_create(Rect rect) {
	
	// Malloc the graph.
	Graph *graph = malloc(sizeof(Graph));
	if (!graph) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a Graph!\n");
		return NULL;
	}

	// Initializes the graph.
	graph->x_axis = AXIS_INIT;
	graph->y_axis = AXIS_INIT;
	graph->rect = rect;
	graph->grid_rect = RECT_INIT;
	graph->background_color = COLOR_GRAY9;
	graph->graph_color = COLOR_GRAY3;
	graph->title_color = COLOR_WHITE;
	graph->text_color = COLOR_BLACK;
	graph->background_vao = NULL;
	graph->grid_vao = NULL;
	graph->title_vao = NULL;
	graph->save = NULL;
	graph->title = NULL;

	// Creates the curves vector.
	graph->curves = curves_create();
	if (!graph->curves) {
		fprintf(stderr, "[ARGUS]: error: unable to create the curves of a Graph!\n");
		free(graph);
		return NULL;
	}
	return graph;
}

/// @brief Frees the memory allocated for a Graph.
/// @param p_graph A pointer to the pointer of the Graph to be freed. Cannot be NULL.
/// @note After freeing, the pointer *p_graph is set to NULL to avoid double-free.
void graph_free(Graph **p_graph) {
	Graph *graph = *p_graph;
	if (!graph) return;
	curves_free(&graph->curves);
	vao_free(&graph->background_vao);
	vao_free(&graph->title_vao);
	vao_free(&graph->grid_vao);
	free(graph->x_axis.title);
	free(graph->y_axis.title);
	axis_reset_graphics(&graph->x_axis);
	axis_reset_graphics(&graph->y_axis);
	imagebutton_free(&graph->save);
	free(graph->title);
	free(graph);
	*p_graph = NULL;
}

/// @brief Prepares the static graphical components of a graph.
/// @param graph The graph to prepare.
/// @param glyphs The glyphs set used to render text.
/// @param window_width The width of the window.
/// @param window_height The height of the window.
/// @return false if there was an error.
bool graph_prepare_static(Graph *graph, Glyphs *glyphs, int window_width, int window_height) {
	bool is_x_title = graph->x_axis.title && strlen(graph->x_axis.title);
	bool is_y_title = graph->y_axis.title && strlen(graph->y_axis.title);
	const float dx = 5.0f/window_width;
	const float dy = 5.0f/window_height;
	const float text_height = 30.0f;

	// Calculates the rects of the components.
	Rect title_rect = {
		graph->rect.x+dx, graph->rect.y+dy, 
		graph->rect.w-2*dx, text_height/window_height-2*dy
	};
	Rect graph_rect = {
		graph->rect.x+dx, graph->rect.y+text_height/window_height+1*dy, 
		graph->rect.w-2*dx, graph->rect.h-text_height/window_height-2*dy
	};

	// Creates the text VAO for the graph title.
	if (!graph->title || !strlen(graph->title)) {
		graph_rect.y = graph->rect.y+dy;
		graph_rect.h = graph->rect.h-2*dy;
		graph->title_vao = NULL;
	} else {
		int n;
		float *vertices;
		float *textures;
		if (!glyphs_generate_text_buffers(glyphs, &title_rect, graph->title, 
			(float)window_width/window_height, &vertices, &textures, &n)) {
			fprintf(stderr, "[ARGUS]: error: unable to create buffer to store the data of the title of a graph!\n");
			return false;
		}
		graph->title_vao = glyphs_generate_text_vao(vertices, textures, n);
		if (!graph->title_vao) {
			fprintf(stderr, "[ARGUS]: error: unable to create the VAO for the title of a graph!\n");
			return false;
		}
	}

	// Calculate the rects of the axis.
	float y_axis_height = text_height/window_height * (is_y_title ? 2 : 1);
	if (is_y_title) y_axis_height -= dy;
	Rect y_axis_rect = {
		graph_rect.x+dx, graph_rect.y+graph_rect.h-y_axis_height, 
		graph_rect.w-2*dx, y_axis_height
	};
	float x_axis_width = text_height/window_width * (is_x_title ? 2 : 1);
	Rect x_axis_rect = {
		graph_rect.x, graph_rect.y+dy, 
		x_axis_width, graph_rect.h-2*dy
	};
	x_axis_rect.h -= y_axis_rect.h;
	y_axis_rect.w -= x_axis_rect.w;
	y_axis_rect.x += x_axis_rect.w;

	// Calculate the rect of the grid.
	graph->grid_rect = (Rect){
		x_axis_rect.x+x_axis_rect.w,x_axis_rect.y,
		y_axis_rect.w, x_axis_rect.h 
	};

	// Prepares the axis VAOs.
	if (!axis_prepare_x_title(&graph->x_axis, glyphs, &x_axis_rect, window_width, window_height)) {
		fprintf(stderr, "[ARGUS]: error: unable to prepare the x axis title of a graph!\n");
		return false;
	}
	if (!axis_prepare_y_title(&graph->y_axis, glyphs, &y_axis_rect, window_width, window_height)) {
		fprintf(stderr, "[ARGUS]: error: unable to prepare the y axis title of a graph!\n");
		return false;
	}

	// Creates the save button.
	imagebutton_free(&graph->save);
	void **args = malloc(sizeof(Graph*)+sizeof(Glyphs*));
	if (!args) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc the arguments buffer of the save button of a Graph!\n");
		return false;
	}
	args[0] = graph;
	args[1] = glyphs;
	graph->save = imagebutton_create(save_screnshot, args);
	if (!graph->save) {
		fprintf(stderr, "[ARGUS]: error: unable to create the save button of a Graph!\n");
		free(args);
		return false;
	}

	// Prepares the save button VAO.
	Rect save_rect = {graph->rect.x+dx, graph->rect.y+graph->rect.h-6*dy, 5*dx, 5*dy};
	if (!imagebutton_prepare_static(graph->save, &save_rect, save_icon, save_icon_size)) {
		fprintf(stderr, "[ARGUS]: error: unable to prepare the save button VAO of a graph!\n");
		return false;
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
	
	// Initializes the colors of the vertices for the background part.
	float colors[48];
	for (int i = 0; i < 6; ++i) {
		colors[4*i] =	graph->graph_color.r;
		colors[4*i+1] =	graph->graph_color.g;
		colors[4*i+2] =	graph->graph_color.b;
		colors[4*i+3] =	1.0f;
	}
	for (int i = 0; i < 6; ++i) {
		colors[24+4*i] =	graph->background_color.r;
		colors[24+4*i+1] =	graph->background_color.g;
		colors[24+4*i+2] =	graph->background_color.b;
		colors[24+4*i+3] =	1.0f;
	}

	// Creates the VAO for the background part.
	void *data[2] = {vertices, colors};
	int sizes[2] = {2,4};
	int gl_types[2] = {GL_FLOAT,GL_FLOAT};
	graph->background_vao = vao_create(data, sizes, gl_types, 12,2);
	if (!graph->background_vao) {
		fprintf(stderr, "[ARGUS]: error: unable to create the VAO for the background of a graph!\n");
		return false;
	}
	return true;
}

/// @brief Prepares the dynamic graphical components of a graph.
/// @param graph The graph to prepare.
/// @note This has to be called before each graph_render call.
/// @return false if there was an error.
bool graph_prepare_dynamic(Graph *graph, Glyphs *glyphs, int window_width, int window_height) {

	// Adapts the x axis if needed.
	if ((graph->x_axis.auto_adapt == ADAPTMODE_AUTO_EXTEND ||
		graph->x_axis.auto_adapt == ADAPTMODE_AUTO_FIT) && 
		curves_size(graph->curves)) {
		if (graph->x_axis.auto_adapt == ADAPTMODE_AUTO_FIT) {
			graph->x_axis.min = FLT_MAX;
			graph->x_axis.max = FLT_MIN;
		}
		for (size_t i = 0; i < curves_size(graph->curves); ++i) {
			const Curve *curve = graph->curves->data[i];
			if (graph->x_axis.min > curve->x_min) graph->x_axis.min = curve->x_min;
			if (graph->x_axis.max < curve->x_max) graph->x_axis.max = curve->x_max;
		}

	// Translates the x axis if needed.
	} else if (graph->x_axis.auto_adapt == ADAPTMODE_SLIDING_WINDOW) {
		for (size_t i = 0; i < curves_size(graph->curves); ++i) {
			const Curve *curve = graph->curves->data[i];
			if (!curve->to_render || !curve->x_val->size) continue;
			float new_x = ringbuffer_at(curve->x_val, curve->x_val->size-1);	
			if (new_x < graph->x_axis.min) {
				float dx = new_x-graph->x_axis.min; 
				graph->x_axis.min += dx;
				graph->x_axis.max += dx;
			}
			if (new_x > graph->x_axis.max) {
				float dx = new_x-graph->x_axis.max; 
				graph->x_axis.min += dx;
				graph->x_axis.max += dx;
			}
		}
	}

	// Adapt the y axis if needed.
	if ((graph->y_axis.auto_adapt == ADAPTMODE_AUTO_EXTEND ||
		graph->y_axis.auto_adapt == ADAPTMODE_AUTO_FIT) && 
		curves_size(graph->curves)) {
		if (graph->y_axis.auto_adapt == ADAPTMODE_AUTO_FIT) {
			graph->y_axis.min = FLT_MAX;
			graph->y_axis.max = FLT_MIN;
		}
		for (size_t i = 0; i < curves_size(graph->curves); ++i) {
			const Curve *curve = graph->curves->data[i];
			if (graph->y_axis.min > curve->y_min) graph->y_axis.min = curve->y_min;
			if (graph->y_axis.max < curve->y_max) graph->y_axis.max = curve->y_max;
		}

	// Translates the y axis if needed.
	} else if (graph->y_axis.auto_adapt == ADAPTMODE_SLIDING_WINDOW) {
		for (size_t i = 0; i < curves_size(graph->curves); ++i) {
			const Curve *curve = graph->curves->data[i];
			if (!curve->to_render || !curve->y_val->size) continue;
			float new_y = ringbuffer_at(curve->y_val, curve->y_val->size-1);	
			if (new_y < graph->y_axis.min) {
				float dy = new_y-graph->y_axis.min; 
				graph->y_axis.min += dy;
				graph->y_axis.max += dy;
			}
			if (new_y > graph->y_axis.max) {
				float dy = new_y-graph->y_axis.max; 
				graph->y_axis.min += dy;
				graph->y_axis.max += dy;
			}
		}
	}

	// Sets the default value of the graphs.
	if (graph->x_axis.min >= graph->x_axis.max) {
		graph->x_axis.min = 0.0f;
		graph->x_axis.max = 1.0f;
	}
	if (graph->y_axis.min >= graph->y_axis.max) {
		graph->y_axis.min = 0.0f;
		graph->y_axis.max = 1.0f;
	}

	// Prepares the grid VAO.
	if (!grid_prepare_dynamic(graph, glyphs, &graph->grid_rect, window_width, window_height)) {
		fprintf(stderr, "[ARGUS]: error: unable to create the grid of a graph!\n");
		return false;
	}

	// Prepares the curves VAOs.
	for (size_t i = 0; i < curves_size(graph->curves); ++i) {
		if (!curve_prepare_dynamic(graph->curves->data[i], &graph->x_axis, &graph->y_axis, graph->grid_rect)) {
			fprintf(stderr, "[ARGUS]: error: unable to create the vao of a curve!\n");
			return false;
		}
	}
	return true;
}

/// @brief Frees the graphics components a the end of the render.
/// @param graph The graph to reset.
void graph_reset_graphics(Graph *graph) {
	axis_reset_graphics(&graph->x_axis);
	axis_reset_graphics(&graph->y_axis);
	vao_free(&graph->grid_vao);
	vao_free(&graph->background_vao);
	vao_free(&graph->title_vao);
}

/// @brief Renders the graph.
/// @param graph The graph to render.
/// @param glyphs The glyphs set to use to render texts.
void graph_render(Graph *graph, Glyphs *glyphs) {
	render_shape(graph->background_vao, 1.0f);
	render_text(glyphs, graph->title_vao, graph->title_color);
	render_text(glyphs, graph->x_axis.title_vao, graph->text_color);
	render_text(glyphs, graph->y_axis.title_vao, graph->text_color);
	render_text(glyphs, graph->x_axis.axis_vao, graph->text_color);
	render_text(glyphs, graph->y_axis.axis_vao, graph->text_color);
	for (size_t i = 0; i < curves_size(graph->curves); ++i) {
		Curve *curve = graph->curves->data[i];
		render_curve(curve->curve_vao, curve->color, true);
	}
	render_curve(graph->grid_vao, graph->text_color, false);
	imagebutton_render(graph->save);
}
