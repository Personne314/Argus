#pragma once

#include <stdbool.h>
#include "ring_buffer.h"
#include "vector.h"
#include "axis.h"
#include "structs.h"
#include "vao.h"
#include "enums.h"



/// @struct Curve
/// @brief Represents a curve with associated data buffers and axis limits.
typedef struct {
    Color color;    ///< The color of the curve.
	VAO *curve_vao; ///< The VAO of the curve.
    RingBuffer *x_val;	///< Buffer storing x-axis values.
    RingBuffer *y_val;	///< Buffer storing y-axis values.
    void (*update)(float *x, float *y, double dt); ///< update function.
    float x_min;	///< Minimum x-axis value.
    float x_max;	///< Maximum x-axis value.
    float y_min;	///< Minimum y-axis value.
    float y_max;	///< Maximum y-axis value.
    DrawMode mode;  ///< The draw mode to use for the curve.
    bool to_render; ///< true if the VAO must be recreated.

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

// Pushes new x-axis data into the curve's buffer.
void curve_push_x_data_raw(Curve *curve, float *data, size_t n);

// Pushes new y-axis data into the curve's buffer.
void curve_push_y_data_raw(Curve *curve, float *data, size_t n);

// Prepares the VAO of a curve in a given graph.
bool curve_prepare_dynamic(Curve *curve, const Axis *x_axis, const Axis *y_axis, const Rect rect);

// Sets the update function of a curve.
void curve_set_update_function(Curve *curve, void (*func)(float *x, float *y, double dt));

/// Calls the update function of the curve.
void curve_update(Curve *curve, double dt);

// Creates a VAO for a curve.
VAO *curve_prepare_vao(float *x_val, float *y_val, int n);
