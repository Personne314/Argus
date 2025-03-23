#include "grid.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "curve.h"
#include "vao.h"
#include "axis.h"



/// @brief Prepares the VAO for the grid.
/// @param vao Pointer used to return the VAO.
/// @param x_axis x axis of the grid.
/// @param y_axis y axis of the grid.
/// @param p_grid_rect Pointer to the grid rect.
/// @return false if there was an error.
bool grid_prepare_graphics(Graph *graph, Glyphs *glyphs, Rect *p_grid_rect, int window_width, int window_height) {
	Rect grid_rect = *p_grid_rect;

	// Frees the previous VAO.
	vao_free(&graph->grid_vao);
	graph->grid_vao = NULL;
	if (graph->x_axis.min >= graph->x_axis.max || graph->y_axis.min >= graph->y_axis.max) {
		fprintf(stderr, "[ARGUS]: error: The axis min and max value are invalid! x_min:%f, x_max:%f ; "
			"y_min:%f, y_max:%f\n", graph->x_axis.min, graph->x_axis.max, graph->y_axis.min, graph->y_axis.max);
		return false;
	}

	// Calculates the number of values to display.
	const float x_range = graph->x_axis.max - graph->x_axis.min;
	const float y_range = graph->y_axis.max - graph->y_axis.min;
	float dx = powf(10.0f, floor(log10f(x_range)));
	float dy = powf(10.0f, floor(log10f(y_range)));
	while (x_range/dx < 5) dx /= 2;
	while (y_range/dy < 5) dy /= 2;

	// Calculates the positions of the lines.
	const int n_x = ceil(x_range/dy);
	const int n_y = ceil(y_range/dy);
	const float min_x_grad = dx*ceil(graph->x_axis.min/dx);
	const float min_y_grad = dy*ceil(graph->y_axis.min/dy);
	const float x_offset = (min_x_grad-graph->x_axis.min)/x_range;
	const float y_offset = (min_y_grad-graph->y_axis.min)/y_range;

	// Malloc buffers to store the lines coordinates.
	float *x_coord = malloc((8+2*(n_x+n_y)) * sizeof(float));
	float *y_coord = malloc((8+2*(n_x+n_y)) * sizeof(float));
	if (!x_coord || !y_coord) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc buffers to store the grid coordinates !\n");
		free(x_coord);
		free(y_coord);
		return false;
	}

	// x coordinates of the grid box.
	x_coord[0] = grid_rect.x;
	x_coord[1] = grid_rect.x+grid_rect.w;
	x_coord[2] = grid_rect.x;
	x_coord[3] = grid_rect.x+grid_rect.w;
	x_coord[4] = grid_rect.x;
	x_coord[5] = grid_rect.x;
	x_coord[6] = grid_rect.x+grid_rect.w;
	x_coord[7] = grid_rect.x+grid_rect.w;

	// y coordinates of the grid box.
	y_coord[0] = grid_rect.y;
	y_coord[1] = grid_rect.y;
	y_coord[2] = grid_rect.y+grid_rect.h;
	y_coord[3] = grid_rect.y+grid_rect.h;
	y_coord[4] = grid_rect.y;
	y_coord[5] = grid_rect.y+grid_rect.h;
	y_coord[6] = grid_rect.y;
	y_coord[7] = grid_rect.y+grid_rect.h;

	// Adds the vertical lines.
	for (int i = 0; i < n_x; ++i) {
		x_coord[8+2*i]		= grid_rect.x + grid_rect.w * (x_offset+i*dx/x_range);
		x_coord[8+2*i+1]	= grid_rect.x + grid_rect.w * (x_offset+i*dx/x_range);
		y_coord[8+2*i]		= grid_rect.y;
		y_coord[8+2*i+1]	= grid_rect.y + grid_rect.h;
	}

	// Adds the horizontal lines.
	for (int i = 0; i < n_y; ++i) {
		x_coord[8+2*(n_x+i)]	= grid_rect.x;
		x_coord[8+2*(n_x+i)+1]	= grid_rect.x + grid_rect.w;
		y_coord[8+2*(n_x+i)]	= grid_rect.y + grid_rect.h - grid_rect.h * (y_offset+i*dy/y_range);
		y_coord[8+2*(n_x+i)+1]	= grid_rect.y + grid_rect.h - grid_rect.h * (y_offset+i*dy/y_range);
	}

	// Creates the grid VAO.
	graph->grid_vao = curve_prepare_vao(x_coord, y_coord, 8+2*(n_x+n_y));
	free(x_coord);
	free(y_coord);
	if (!graph->grid_vao) {
		fprintf(stderr, "[ARGUS]: error: unable to create a graph grid VAO !\n");
		return false;
	}

	// Creates the axis VAOs.
	if (!axis_prepare_x_axis(&graph->x_axis, glyphs, &graph->grid_rect, x_range,
		grid_rect.x + grid_rect.w*x_offset, min_x_grad, dx, n_x, window_width, window_height)) {
			fprintf(stderr, "[ARGUS]: error: unable to prepare a graph x axis !\n");
			return false;
	}
	if (!axis_prepare_y_axis(&graph->y_axis, glyphs, &graph->grid_rect, y_range, 
		grid_rect.h*y_offset, min_y_grad, dy, n_y, window_width, window_height)) {
			fprintf(stderr, "[ARGUS]: error: unable to prepare a graph y axis !\n");
			return false;
	}
	return true;
}
