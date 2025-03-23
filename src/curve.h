#pragma once

#include "ring_buffer.h"
#include "vector.h"
#include "axis.h"
#include "structs.h"
#include "vao.h"



/// @struct Curve
/// @brief Represents a curve with associated data buffers and axis limits.
typedef struct {
    Color color;
	VAO *curve_vao;
    RingBuffer *x_val;	///< Buffer storing x-axis values.
    RingBuffer *y_val;	///< Buffer storing y-axis values.
    float x_min;	///< Minimum x-axis value.
    float x_max;	///< Maximum x-axis value.
    float y_min;	///< Minimum y-axis value.
    float y_max;	///< Maximum y-axis value.
} Curve;


// Creates a curve.
Curve *curve_create();

// Frees the memory allocated for a Curve.
void curve_free(Curve **p_curve);


// Sets the capacity of the curve's data buffers.
void curve_set_data_cap(Curve *curve, size_t cap);

// Pushes new x-axis data into the curve's buffer.
void curve_push_x_data(Curve *curve, Vector *data);

// Pushes new y-axis data into the curve's buffer.
void curve_push_y_data(Curve *curve, Vector *data);


bool curve_prepare_dynamic(Curve *curve, const Axis *x_axis, const Axis *y_axis, const Rect rect);


// Creates a VAO for a curve.
VAO *curve_prepare_vao(float *x_val, float *y_val, int n);
