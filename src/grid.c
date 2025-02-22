#include "grid.h"

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"



void grid_prepare_graphics(VAO **vao, Axis *x_axis, Axis *y_axis) {
	if (*vao) vao_free(*vao);
	*vao = NULL;

	Vector *x_lines = create_vector(8);
	Vector *y_lines = create_vector(8);
	if (!x_lines || !y_lines) {
		vector_free(x_lines);
		vector_free(y_lines);
		fprintf(stderr, "[ARGUS]: error: unable to create the vectors for the graph grid !\n");
		return;
	}






	vector_free(x_lines);
	vector_free(y_lines);

}
