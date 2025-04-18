#include "curve.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <float.h>
#include "point.h"



/// @brief Creates a curve.
/// @return A pointer to the newly created curve, or NULL if allocation fails.
Curve *curve_create() {
	Curve *curve = malloc(sizeof(Curve));
	if (!curve) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a Curve\n");
		return NULL;
	}
	curve->color = COLOR_BLACK;
	curve->curve_vao = NULL;
	curve->x_min = FLT_MAX;
	curve->x_max = FLT_MIN;
	curve->y_min = FLT_MAX;
	curve->y_max = FLT_MIN;
	curve->x_val = NULL;
	curve->y_val = NULL;
	curve->to_render = false;
	curve->update = NULL;
	curve->mode = DRAW_CURVE;
	return curve;
}

/// @brief Frees the memory allocated for a Curve.
/// @param p_curve A pointer to the pointer of the Curve to be freed. Cannot be NULL.
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

/// @brief Pushes new x-axis data into the curve's buffer.
/// @param curve Pointer to the curve receiving the new data.
/// @param data The raw buffer containing the data.
/// @param n The number of values to add.
/// @note n must be lower or equal to the length of data.
void curve_push_x_data_raw(Curve *curve, float *data, size_t n) {
	if (n + curve->x_val->size > curve->x_val->cap) {
		fprintf(stderr, "[ARGUS]: warning: the space left in the buffer of the x axis of a graph "
			"is lower than the amount of data that will be pushed. The oldset data will be erased.\n");
	}
	for (size_t i = 0; i < n; ++i) {
		const float val = data[i];
		ringbuffer_push_back(curve->x_val, val);
		if (val < curve->x_min) curve->x_min = val;
		if (val > curve->x_max) curve->x_max = val;
	}
}

/// @brief Pushes new y-axis data into the curve's buffer.
/// @param curve Pointer to the curve receiving the new data.
/// @param data The raw buffer containing the data.
/// @param n The number of values to add.
/// @note n must be lower or equal to the length of data.
void curve_push_y_data_raw(Curve *curve, float *data, size_t n) {
	if (n + curve->y_val->size > curve->y_val->cap) {
		fprintf(stderr, "[ARGUS]: warning: the space left in the buffer of the y axis of a graph "
			"is lower than the amount of data that will be pushed. The oldset data will be erased.\n");
	}
	for (size_t i = 0; i < n; ++i) {
		const float val = data[i];
		ringbuffer_push_back(curve->y_val, val);
		if (val < curve->y_min) curve->y_min = val;
		if (val > curve->y_max) curve->y_max = val;
	}
}












/// @brief Generates the vertices for a curve.
/// @param x The x coordinates data to use.
/// @param y The y coordinates data to use.
/// @param vertices The vector where to store the vertices.
/// @param limits The axis limits.
/// @param rect The rect of the graph.
void curve_prepare_curve(RingBuffer *x, RingBuffer *y, Vector *vertices, const Rect limits, const Rect rect) {
	const size_t size = x->size;
	if (size < 2) return;
	const float x_min = limits.x;
	const float y_min = limits.y;
	const float x_max = limits.w;
	const float y_max = limits.h;

	// Adds all the points of the curve if they're visible.
	bool curve_started = false;
	size_t i = 0;
	Point prev, current;
	prev.x = (ringbuffer_at(x, 0)-x_min) / (x_max-x_min);
	prev.y = 1.0f-(ringbuffer_at(y, 0)-y_min) / (y_max-y_min);
	while (i < size) {

		// Gets the current point coordinates and checks what should be added to the point vector.
		current.x = (ringbuffer_at(x, i)-x_min) / (x_max-x_min);
		current.y = 1.0f-(ringbuffer_at(y, i)-y_min) / (y_max-y_min);
		if (curve_started) {

			// Both points are in the graph area.
			if (current.x >= 0 && current.x <= 1 && current.y >= 0 && current.y <= 1) {
				vector_push_back(vertices, rect.x + rect.w*current.x);
				vector_push_back(vertices, rect.y + rect.h*current.y);

			// The first point is in the graph area but not the second.
			} else {
				Point new_current;
				move_in_rectangle(prev, current, &new_current);
				vector_push_back(vertices, rect.x + rect.w*new_current.x);
				vector_push_back(vertices, rect.y + rect.h*new_current.y);
				curve_started = false;
			}

		} else {

			// The second point is in the graph area but not the first.
			if (current.x >= 0 && current.x <= 1 && current.y >= 0 && current.y <= 1) {
				Point new_prev;
				move_in_rectangle(current, prev, &new_prev);
				vector_push_back(vertices, rect.x + rect.w*new_prev.x);
				vector_push_back(vertices, rect.y + rect.h*new_prev.y);
				vector_push_back(vertices, rect.x + rect.w*current.x);
				vector_push_back(vertices, rect.y + rect.h*current.y);
				curve_started = true;
				
			// Both points are out of the graph but they segment intersects the graph area. 
			} else if (intersect_rectangle(prev, current)) {
				Point new_current, new_prev;
				move_in_rectangle(prev, current, &new_current);
				move_in_rectangle(current, prev, &new_prev);
				vector_push_back(vertices, rect.x + rect.w*new_prev.x);
				vector_push_back(vertices, rect.y + rect.h*new_prev.y);
				vector_push_back(vertices, rect.x + rect.w*new_current.x);
				vector_push_back(vertices, rect.y + rect.h*new_current.y);
			}
		}

		// Next point.
		prev = current;
		++i;
	}
}





/// @brief 
/// @param x 
/// @param y 
/// @param vertices 
/// @param limits 
/// @param rect 
void curve_prepare_scatter(RingBuffer *x, RingBuffer *y, Vector *vertices, const Rect limits, const Rect rect) {
	const size_t size = x->size;
	if (size < 2) return;
	const float x_min = limits.x;
	const float y_min = limits.y;
	const float x_max = limits.w;
	const float y_max = limits.h;

	// Adds all the points of the curve if they're visible.
	bool curve_started = false;
	size_t i = 0;
	Point current;
	while (i < size) {

		// Gets the current point coordinates and checks what should be added to the point vector.
		current.x = (ringbuffer_at(x, i)-x_min) / (x_max-x_min);
		current.y = 1.0f-(ringbuffer_at(y, i)-y_min) / (y_max-y_min);
		if (current.x >= 0 && current.x <= 1 && current.y >= 0 && current.y <= 1) {


			///////////////////////////////////////////////////////////
			//                   ADDS VERTICES HERE                  //
			///////////////////////////////////////////////////////////


		}

	}
}





/*



largeur/hauteur -> uniform

offset -> par instance

vertices -> identiques, instance ?





*/






/// @brief Prepares the VAO of a curve in a given graph.
/// @param curve The curve to prepare.
/// @param x_axis The x axis of the graph.
/// @param y_axis The y axis of the graph.
/// @param rect The rect of the graph where to draw the curve.
/// @return false if there was an error.
bool curve_prepare_dynamic(Curve *curve, const Axis *x_axis, const Axis *y_axis, const Rect rect) {
	vao_free(&curve->curve_vao);

	// Gets the number of points int the curve.
	if (curve->x_val->size != curve->y_val->size) {
		fprintf(stderr, "[ARGUS]: error: the curve x and y buffers are not of the same size!\n");
		return false;
	}
	if (!curve->x_val->size) return true;

	// Gets the axis limits.
	const Rect limits = {x_axis->min, y_axis->min, x_axis->max, y_axis->max};

	// Creates the buffer to store the points vertices.
	Vector *point_vec = vector_create(64);
	if (!point_vec) {
		fprintf(stderr, "[ARGUS]: error: unable to create a vector to store curve points!\n");
		return false;
	}

	// Generates the vertices according to the drawing mode.
	if (curve->mode == DRAW_SCATTER) curve_prepare_scatter(curve->x_val, curve->y_val, point_vec, limits, rect);
	else curve_prepare_curve(curve->x_val, curve->y_val, point_vec, limits, rect);
	if (!vector_size(point_vec)) return true;

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

/// @brief Sets the update function of a curve.
/// @param curve The curve that will be updated.
/// @param func The function used for the update.
/// @note func should place the coordinates of the new point in *x and *y.
/// @note The initial value of *x and *y will be set to 0 berfore the first call to the function.
/// @note The initial value of *x and *y will be set to the previous value added before the other calls.
void curve_set_update_function(Curve *curve, void (*func)(float *x, float *y, double dt)) {
	if (!curve->x_val) {
		fprintf(stderr, "[ARGUS]: warning: the curve x axis capacity "
			"hasn't been set! The update function won't be called.\n");
		return;
	}
	if (!curve->y_val) {
		fprintf(stderr, "[ARGUS]: warning: the curve x axis capacity "
			"hasn't been set! The update function won't be called.\n");
		return;
	}
	curve->update = func;
}

/// @brief Calls the update function of the curve.
/// @param curve The function to update.
/// @param dt The timestep between each update.
void curve_update(Curve *curve, double dt) {
	if (!curve->update || !curve->x_val || !curve->y_val) return;
	float x = curve->x_val->size ? ringbuffer_at(curve->x_val, 0) : 0.0f;
	float y = curve->y_val->size ? ringbuffer_at(curve->y_val, 0) : 0.0f;
	curve->update(&x, &y, dt);
	ringbuffer_push_back(curve->x_val, x);
	ringbuffer_push_back(curve->y_val, y);
	if (x < curve->x_min) curve->x_min = x;
	if (x > curve->x_max) curve->x_max = x;
	if (y < curve->y_min) curve->y_min = y;
	if (y > curve->y_max) curve->y_max = y;
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
