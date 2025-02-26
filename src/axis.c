#include "axis.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


bool axis_prepare_x_title(Axis *axis, Glyphs *glyphs, Rect *p_rect, int window_width, int window_height) {
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->axis_vao = NULL;

	const float dx = 5.0f/window_width;

	Rect rect = *p_rect;
	float *vertices = NULL;
	float *textures = NULL;

	int n = 0;
	if (axis->title && strlen(axis->title)) {
		Rect text_rect = {rect.x+dx, rect.y, rect.w/2-2*dx, rect.h};
		if (glyphs_generate_vertical_text_buffers(glyphs, &text_rect, axis->title, 
			(float)window_width/window_height, &vertices, &textures, &n)) {
			fprintf(stderr, "[ARGUS]: error: unable to generate the buffers of data for the x axis title of a graph !\n");
			return true;
		}
	}

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

bool axis_prepare_y_title(Axis *axis, Glyphs *glyphs, Rect *p_rect, int window_width, int window_height) {
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->axis_vao = NULL;

	const float dy = 5.0f/window_height;

	Rect rect = *p_rect;
	float *vertices = NULL;
	float *textures = NULL;

	int n = 0;
	if (axis->title && strlen(axis->title)) {
		Rect text_rect = {rect.x, rect.y+(rect.h-dy)/2+dy, rect.w, (rect.h+dy)/2-2*dy};
		if (glyphs_generate_text_buffers(glyphs, &text_rect, axis->title, 
			(float)window_width/window_height, &vertices, &textures, &n)) {
			fprintf(stderr, "[ARGUS]: error: unable to generate the buffers of data for the y axis title of a graph !\n");
			return true;
		}
	}

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



bool axis_prepare_x_axis(Axis *axis, Glyphs *glyphs, float y, float range, float offset, 
float base, float d, int n, int window_width, int window_height, float grid_width) {
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->axis_vao = NULL;

	const float window_ratio = (float)window_width/window_height;
	const float dy = 5.0f/window_height;


	float *vertices = malloc(192*n*sizeof(float));
	float *textures = malloc(192*n*sizeof(float));
	if (!vertices || !textures) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc buffers for the data of the y axis of a graph !\n");
		return true;
	}
	
	int size = 0;
	for (int i = 0; i < n; ++i) {
		float value = base + i*d;
		char text_buffer[16];
		snprintf(text_buffer, sizeof(text_buffer), "%g", value); 

		int n;
		float *v = NULL, *t = NULL;
		Rect rect = {
			offset + (i*grid_width-0.5)*d/range, y+dy,
			d/range, 4*dy
		};
		
		if (glyphs_generate_text_buffers(glyphs, &rect, text_buffer, window_ratio, &v, &t, &n)) {
			fprintf(stderr, "[ARGUS]: error: unable to generate the buffers of data for the y axis of a graph !\n");
			if (v) free(v);
			if (t) free(t);
			free(vertices);
			free(textures);
			return true;
		}

		for (int i = 0; i < 12*n; ++i) {
			vertices[12*(size)+i] = v[i];
			textures[12*(size)+i] = t[i];
		}

		size += n;
		free(v);
		free(t);

	}

	axis->axis_vao = glyphs_generate_text_vao(vertices, textures, size);
	free(vertices);
	free(textures);
	if (!axis->axis_vao) {
		fprintf(stderr, "[ARGUS]: error: unable to generate the VAO for the y axis of a graph !\n");
		return true;
	}
	return false;

}

bool axis_prepare_y_axis(Axis *axis, Glyphs *glyphs, float x, float range, float offset, 
float base, float d, int n, int window_width, int window_height, float grid_height) {
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->axis_vao = NULL;

	const float window_ratio = (float)window_width/window_height;
	const float dx = 5.0f/window_width;



	return false;

}



void axis_reset_graphics(Axis *axis) {
	if (axis->title_vao) vao_free(axis->title_vao);
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->title_vao = NULL;
	axis->axis_vao = NULL;
}
