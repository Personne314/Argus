#include "axis.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/// @brief Prepares the x axis title.
/// @param axis The axis to prepare.
/// @param glyphs The glyphs set to use.
/// @param p_rect The rect of the axis.
/// @param window_width The window width.
/// @param window_height The window height.
/// @return true is there was an error.
bool axis_prepare_x_title(Axis *axis, Glyphs *glyphs, Rect *p_rect, int window_width, int window_height) {
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->axis_vao = NULL;

	// Constants used for the vertices generation.
	const float dx = 5.0f/window_width;
	const Rect rect = *p_rect;

	// Generates the vertices of the title.
	int n = 0;
	float *vertices = NULL;
	float *textures = NULL;
	if (axis->title && strlen(axis->title)) {
		Rect text_rect = {rect.x+dx, rect.y, rect.w/2-2*dx, rect.h};
		if (glyphs_generate_vertical_text_buffers(glyphs, &text_rect, axis->title, 
			(float)window_width/window_height, &vertices, &textures, &n)) {
			fprintf(stderr, "[ARGUS]: error: unable to generate the buffers of data for the x axis title of a graph !\n");
			return true;
		}
	}

	// Creates the VAO.
	if (vertices && textures) {
		axis->title_vao = glyphs_generate_text_vao(vertices, textures, n);
		free(vertices);
		free(textures);
		if (!axis->title_vao) {
			fprintf(stderr, "[ARGUS]: error: unable to creates the VAO for the x axis title of a graph !\n");
			return true;
		}
	} else axis->title_vao = NULL;
	return false;

}

/// @brief Prepares the y axis title.
/// @param axis The axis to prepare.
/// @param glyphs The glyphs set to use.
/// @param p_rect The rect of the axis.
/// @param window_width The window width.
/// @param window_height The window height.
/// @return true is there was an error.
bool axis_prepare_y_title(Axis *axis, Glyphs *glyphs, Rect *p_rect, int window_width, int window_height) {
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->axis_vao = NULL;

	// Constants used for the vertices generation.
	const float dy = 5.0f/window_height;
	const Rect rect = *p_rect;

	// Generates the vertices of the title.
	int n = 0;
	float *vertices = NULL;
	float *textures = NULL;
	if (axis->title && strlen(axis->title)) {
		Rect text_rect = {rect.x, rect.y+(rect.h-dy)/2+dy, rect.w, (rect.h+dy)/2-2*dy};
		if (glyphs_generate_text_buffers(glyphs, &text_rect, axis->title, 
			(float)window_width/window_height, &vertices, &textures, &n)) {
			fprintf(stderr, "[ARGUS]: error: unable to generate the buffers of data for the y axis title of a graph !\n");
			return true;
		}
	}

	// Creates the VAO.
	if (vertices && textures) {
		axis->title_vao = glyphs_generate_text_vao(vertices, textures, n);
		free(vertices);
		free(textures);
		if (!axis->title_vao) {
			fprintf(stderr, "[ARGUS]: error: unable to creates the VAO for the y axis title of a graph !\n");
			return true;
		}
	} else axis->title_vao = NULL;
	return false;

}

/// @brief Prepares the x axis graduation depending of the min and max values.
/// @param axis The axis to prepare.
/// @param glyphs The glyph set to use.
/// @param p_grid_rect The rect of the grid, used to place labels at the right place.
/// @param range The range of values (max-min).
/// @param offset The offset of the grid used to position the labels according to the lines. 
/// @param base The first number to render.
/// @param d The delta between each value to render.
/// @param n The number of values to render.
/// @param window_width The window width.
/// @param window_height The window height.
/// @return true if there was an error.
bool axis_prepare_x_axis(Axis *axis, Glyphs *glyphs, Rect *p_grid_rect, float range, float offset, 
float base, float d, int n, int window_width, int window_height) {
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->axis_vao = NULL;

	// Constants used for the vertices generation.
	const float window_ratio = (float)window_width/window_height;
	const float dy = 5.0f/window_height;
	const Rect grid_rect = *p_grid_rect;

	// Malloc the buffers to store the vertices.
	float *vertices = malloc(192*n*sizeof(float));
	float *textures = malloc(192*n*sizeof(float));
	if (!vertices || !textures) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc buffers for the data of the x axis of a graph !\n");
		return true;
	}
	
	// Generates the vertices of each numbers and adds it to vertices and textures.
	int size = 0;
	for (int i = 0; i < n; ++i) {
		float value = base + i*d;
		char text_buffer[16];
		snprintf(text_buffer, sizeof(text_buffer), "%g", value); 

		// Calculates the number to render and converts it to a string.
		Rect rect = {
			offset + (i*grid_rect.w-0.25)*d/range, grid_rect.y+grid_rect.h+dy,
			0.5*d/range, 4*dy
		};

		// Gets the vertices buffers of the number to render.
		int n = 0;
		float *v = NULL, *t = NULL;		
		if (glyphs_generate_text_buffers(glyphs, &rect, text_buffer, window_ratio, &v, &t, &n)) {
			fprintf(stderr, "[ARGUS]: error: unable to generate the buffers of data for the x axis of a graph !\n");
			if (v) free(v);
			if (t) free(t);
			free(vertices);
			free(textures);
			return true;
		}

		// Calculates offsets when needed.
		float x_max = grid_rect.x+grid_rect.w-v[12*n-2];
		if (x_max > 0) x_max = 0;
		float x_min = grid_rect.x-v[6];
		if (x_min < 0) x_min = 0;

		// Gets the vertices buffers of the number to render.
		for (int i = 0; i < 6*n; ++i) {
			vertices[12*(size)+2*i]		= v[2*i]+x_max+x_min;
			vertices[12*(size)+2*i+1]	= v[2*i+1];
			textures[12*(size)+2*i]		= t[2*i];
			textures[12*(size)+2*i+1]	= t[2*i+1];
		}

		// Frees the buffers and update the size of the VAO.
		size += n;
		free(v);
		free(t);
	}
	
	// Creates the VAO.
	axis->axis_vao = glyphs_generate_text_vao(vertices, textures, size);
	free(vertices);
	free(textures);
	if (!axis->axis_vao) {
		fprintf(stderr, "[ARGUS]: error: unable to generate the VAO for the x axis of a graph !\n");
		return true;
	}
	return false;
}

/// @brief Prepares the y axis graduation depending of the min and max values.
/// @param axis The axis to prepare.
/// @param glyphs The glyph set to use.
/// @param p_grid_rect The rect of the grid, used to place labels at the right place.
/// @param range The range of values (max-min).
/// @param offset The offset of the grid used to position the labels according to the lines. 
/// @param base The first number to render.
/// @param d The delta between each value to render.
/// @param n The number of values to render.
/// @param window_width The window width.
/// @param window_height The window height.
/// @return true if there was an error.
bool axis_prepare_y_axis(Axis *axis, Glyphs *glyphs, Rect *p_grid_rect, float range, float offset, 
float base, float d, int n, int window_width, int window_height) {
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->axis_vao = NULL;

	// Constants used for the vertices generation.
	const float window_ratio = (float)window_width/window_height;
	const float dx = 5.0f/window_width;
	const Rect grid_rect = *p_grid_rect;

	// Malloc the buffers to store the vertices.
	float *vertices = malloc(192*n*sizeof(float));
	float *textures = malloc(192*n*sizeof(float));
	if (!vertices || !textures) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc buffers for the data of the y axis of a graph !\n");
		return true;
	}

	// Generates the vertices of each numbers and adds it to vertices and textures.
	int size = 0;
	for (int i = 0; i < n; ++i) {

		// Calculates the number to render and converts it to a string.
		float value = base + i*d;
		char text_buffer[16];
		snprintf(text_buffer, sizeof(text_buffer), "%g", value); 

		// Calculates the rect to render the text.
		Rect rect = {
			grid_rect.x-5*dx, grid_rect.y + grid_rect.h - (offset + (i*grid_rect.h+0.25)*d/range),
			4*dx, 0.5*d/range
		};
		
		// Gets the vertices buffers of the number to render.
		int n = 0;
		float *v = NULL, *t = NULL;
		if (glyphs_generate_vertical_text_buffers(glyphs, &rect, text_buffer, window_ratio, &v, &t, &n)) {
			fprintf(stderr, "[ARGUS]: error: unable to generate the buffers of data for the y axis of a graph !\n");
			if (v) free(v);
			if (t) free(t);
			free(vertices);
			free(textures);
			return true;
		}

		// Calculates offsets when needed.
		float y_max = grid_rect.y-v[12*n-1];
		if (y_max < 0) y_max = 0;
		float y_min = grid_rect.y+grid_rect.h-v[1];
		if (y_min > 0) y_min = 0;

		// Copy the vertices and adds an offset when needed.
		for (int i = 0; i < 6*n; ++i) {
			vertices[12*(size)+2*i]		= v[2*i];
			vertices[12*(size)+2*i+1]	= v[2*i+1]+y_max+y_min;
			textures[12*(size)+2*i]		= t[2*i];
			textures[12*(size)+2*i+1]	= t[2*i+1];
		}

		// Frees the buffers and pasupdate the size of the VAO.
		size += n;
		free(v);
		free(t);
	}
	
	// Creates the VAO.
	axis->axis_vao = glyphs_generate_text_vao(vertices, textures, size);
	free(vertices);
	free(textures);
	if (!axis->axis_vao) {
		fprintf(stderr, "[ARGUS]: error: unable to generate the VAO for the x axis of a graph !\n");
		return true;
	}
	return false;
}

/// @brief Reset the graphical components of an axis after the rendering process.
/// @param axis The axis to reset.
void axis_reset_graphics(Axis *axis) {
	if (axis->title_vao) vao_free(axis->title_vao);
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->title_vao = NULL;
	axis->axis_vao = NULL;
}
