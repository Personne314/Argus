#include "grid.h"

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"
#include "curve.h"



void grid_prepare_graphics(VAO **vao, Axis *x_axis, Axis *y_axis, Rect *p_grid_rect) {
	if (*vao) vao_free(*vao);
	*vao = NULL;
	Rect grid_rect = *p_grid_rect;

	Vector *x_lines = vector_create(8);
	Vector *y_lines = vector_create(8);
	if (!x_lines || !y_lines) {
		vector_free(x_lines);
		vector_free(y_lines);
		fprintf(stderr, "[ARGUS]: error: unable to create the vectors for the graph grid !\n");
		return;
	}
	vector_free(x_lines);
	vector_free(y_lines);





	float grid_x[16] = {
		grid_rect.x, grid_rect.x+grid_rect.w,
		grid_rect.x, grid_rect.x+grid_rect.w,
		grid_rect.x, grid_rect.x,
		grid_rect.x+grid_rect.w, grid_rect.x+grid_rect.w
	};
	float grid_y[16] = {
		grid_rect.y, grid_rect.y,
		grid_rect.y+grid_rect.h, grid_rect.y+grid_rect.h,
		grid_rect.y, grid_rect.y+grid_rect.h,
		grid_rect.y, grid_rect.y+grid_rect.h
	};
	*vao = curve_prepare_vao(grid_x, grid_y, 8);






}
