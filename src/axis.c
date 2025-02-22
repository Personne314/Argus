#include "axis.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void axis_prepare_x_vao(Axis *axis, Glyphs *glyphs, Rect *p_rect, int window_width, int window_height) {
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->axis_vao = NULL;

	const float dx = 5.0f/window_width;

	Rect rect = *p_rect;
	float *vertices = NULL;
	float *textures = NULL;

	int n = 0;
	if (axis->title && strlen(axis->title)) {
		Rect text_rect = {rect.x+dx, rect.y, rect.w/2-2*dx, rect.h};

		printf("%f %f %f %f\n", text_rect.x*window_width, text_rect.y*window_height, text_rect.w*window_width, text_rect.h*window_height);

		if (glyphs_generate_vertical_text_buffers(glyphs, &text_rect, axis->title, 
			(float)window_width/window_height, &vertices, &textures, &n)) {
			fprintf(stderr, "[ARGUS]: error: unable to generate the buffers of data for the x axis title of a graph !\n");
			return;
		}
	}

	if (vertices && textures) {
		axis->title_vao = glyphs_generate_text_vao(vertices, textures, n);
		free(vertices);
		free(textures);
		if (!axis->title_vao) {
			fprintf(stderr, "[ARGUS]: error: unable to creates the VAO for the x axis title of a graph !\n");
			return;
		}
	} else axis->title_vao = NULL;

}

void axis_prepare_y_vao(Axis *axis, Glyphs *glyphs, Rect *p_rect, int window_width, int window_height) {
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
			return;
		}
	}

	if (vertices && textures) {
		axis->title_vao = glyphs_generate_text_vao(vertices, textures, n);
		free(vertices);
		free(textures);
		if (!axis->title_vao) {
			fprintf(stderr, "[ARGUS]: error: unable to creates the VAO for the y axis title of a graph !\n");
			return;
		}
	} else axis->title_vao = NULL;
	
}

void axis_reset_graphics(Axis *axis) {
	if (axis->title_vao) vao_free(axis->title_vao);
	if (axis->axis_vao) vao_free(axis->axis_vao);
	axis->title_vao = NULL;
	axis->axis_vao = NULL;
}
