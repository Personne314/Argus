#include "curve.h"

#include <stdio.h>
#include <stdlib.h>






Curve *curve_create() {
	Curve *curve = malloc(sizeof(Curve));
	if (!curve) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a Curve\n");
		return NULL;
	}
	curve->x_min = 0.0f;
	curve->x_max = 0.0f;
	curve->y_min = 0.0f;
	curve->y_max = 0.0f;
	curve->x_val = NULL;
	curve->y_val = NULL;
	return curve;
}

void curve_free(Curve **p_curve) {
	Curve *curve = *p_curve;
	if (!curve) return;
	ringbuffer_free(&curve->x_val);
	ringbuffer_free(&curve->y_val);
	free(curve);
	*p_curve = NULL;
}


void curve_set_data_cap(Curve *curve, size_t cap) {
	ringbuffer_free(&curve->x_val);
	ringbuffer_free(&curve->y_val);
	curve->x_val = ringbuffer_create(cap);
	curve->y_val = ringbuffer_create(cap);
}




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
