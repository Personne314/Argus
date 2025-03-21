#include "curve.h"

#include <stdio.h>
#include <stdlib.h>


/// @brief Creates a VAO for a curve.
/// @param x_val x coordinates of the points.
/// @param y_val y coordinates of the points.
/// @param n The number of points. n == length(x_val) == length(y_val)
/// @return The created VAO.
VAO *curve_prepare_vao(float *x_val, float *y_val, int n) {

	// Creates the vertices buffer.
	float *vertices = malloc(2*n*sizeof(float));
	if (!vertices) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a buffer for the vertices of a curve VAO !\n");
		return NULL;
	}
	for (int i = 0; i < n; ++i) {
		vertices[2*i]	= x_val[i];
		vertices[2*i+1]	= y_val[i];
	}

	// Creates the VAO.
	void *data = vertices;
	int size = 2;
	int gl_type = GL_FLOAT;
	VAO *vao = vao_create(&data, &size, &gl_type, n,1);
	free(vertices);
	if (!vao) fprintf(stderr, "[ARGUS]: error: unable to create a VAO for a curve !\n");
	return vao;

}
