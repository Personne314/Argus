#include "curve.h"

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>



/// @brief Creates a curve.
/// @return A pointer to the newly created curve, or NULL if allocation fails.
Curve *curve_create() {
	Curve *curve = malloc(sizeof(Curve));
	if (!curve) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a Curve\n");
		return NULL;
	}
	curve->x_min = FLT_MAX;
	curve->x_max = FLT_MIN;
	curve->y_min = FLT_MAX;
	curve->y_max = FLT_MIN;
	curve->x_val = NULL;
	curve->y_val = NULL;
	return curve;
}

/// @brief Frees the memory allocated for a Curve.
/// @param vao A pointer to the pointer of the Curve to be freed. Cannot be NULL.
/// @note After freeing, the pointer *p_curve is set to NULL to avoid double-free.
void curve_free(Curve **p_curve) {
	Curve *curve = *p_curve;
	if (!curve) return;
	ringbuffer_free(&curve->x_val);
	ringbuffer_free(&curve->y_val);
	free(curve);
	*p_curve = NULL;
}


/// @brief Sets the capacity of the curve's data buffers.
/// @param curve Pointer to the curve whose buffers will be resized.
/// @param cap New capacity for the x and y data buffers.
/// @note This function frees the existing buffers and allocates new ones with the specified capacity.
void curve_set_data_cap(Curve *curve, size_t cap) {
	ringbuffer_free(&curve->x_val);
	ringbuffer_free(&curve->y_val);
	curve->x_val = ringbuffer_create(cap);
	curve->y_val = ringbuffer_create(cap);
}

/// @brief Pushes new x-axis data into the curve's buffer.
/// @param curve Pointer to the curve receiving the new data.
/// @param data Pointer to the vector containing the new x-axis values.
void curve_push_x_data(Curve *curve, Vector *data) {
	const size_t n = vector_size(data);
	if (n + curve->x_val->size > curve->x_val->cap) {
		fprintf(stderr, "[ARGUS]: warning: the space left in the buffer of the x axis of a graph "
			"is lower than the amount of data that will be pushed. The oldset data will be erased.\n");
	}
	for (size_t i = 0; i < n; ++i) {
		const float val = data->data[i];
		ringbuffer_push_back(curve->x_val, val);
		if (val < curve->x_min) curve->x_min = val;
		if (val > curve->x_max) curve->x_max = val;
	}
}

/// @brief Pushes new y-axis data into the curve's buffer.
/// @param curve Pointer to the curve receiving the new data.
/// @param data Pointer to the vector containing the new y-axis values.
void curve_push_y_data(Curve *curve, Vector *data) {
	const size_t n = vector_size(data);
	if (n + curve->y_val->size > curve->y_val->cap) {
		fprintf(stderr, "[ARGUS]: warning: the space left in the buffer of the y axis of a graph "
			"is lower than the amount of data that will be pushed. The oldset data will be erased.\n");
	}
	for (size_t i = 0; i < n; ++i) {
		const float val = data->data[i];
		ringbuffer_push_back(curve->y_val, val);
		if (val < curve->y_min) curve->y_min = val;
		if (val > curve->y_max) curve->y_max = val;
	}
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
