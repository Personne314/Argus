#include "curve.h"

#include <float.h>
#include <math.h>
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
	curve->curve_vao = NULL;
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
	vao_free(&curve->curve_vao);
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










bool curve_prepare_dynamic(Curve *curve, const Axis *x_axis, const Axis *y_axis, const Rect rect) {
	vao_free(&curve->curve_vao);

	// Gets the number of points int the curve.
	if (curve->x_val->size != curve->y_val->size) {
		fprintf(stderr, "[ARGUS]: error: the curve x and y buffers are not of the same size!\n");
		return false;
	}
	const size_t size = curve->x_val->size;
	if (size <= 1) {
		fprintf(stderr, "[ARGUS]: warning: the curve does not contain enough data to draw a curve!\n");
		return true;
	}

	// Gets the axis limits.
	const float x_min = x_axis->min;
	const float x_max = x_axis->max;
	const float y_min = y_axis->min;
	const float y_max = y_axis->max;

	// Creates the buffer to store the points.
	Vector *point_vec = vector_create(64);
	if (!point_vec) {
		fprintf(stderr, "[ARGUS]: error: unable to create a vector to store curve points!\n");
		return false;
	}








	float x_prev = (ringbuffer_at(curve->x_val, 0)-x_min) / (x_max-x_min);
	float y_prev = (ringbuffer_at(curve->y_val, 0)-y_min) / (y_max-y_min);

	size_t i = 0;
	bool curve_started = false;
	while (i < size) {
		float x = (ringbuffer_at(curve->x_val, i)-x_min) / (x_max-x_min);
		float y = (ringbuffer_at(curve->y_val, i)-y_min) / (y_max-y_min);

		if (curve_started) {



		} else {
			if (x >= x_min && x <= x_max && y >= y_min && y <= y_max) {
				// POINT 2 DANS LE RECTANGLE, 1 DEHORS.
			} else 

		}

		x_prev = x;
		y_prev = y;
		++i;
	}






	// Creates the VAO.
	void *data = point_vec->data;
	int sizes = 2;
	int gl_types = GL_FLOAT;
	curve->curve_vao = vao_create(&data, &sizes, &gl_types, vector_size(point_vec)/2,1);
	vector_free(&point_vec);
	if (!curve->curve_vao) {
		fprintf(stderr, "[ARGUS]: error: unable to create a VAO for a curve !\n");
		return false;
	}
	return true;
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
