#pragma once

#include "ring_buffer.h"
#include "vector.h"
#include "vao.h"



struct Curve {
	RingBuffer *x_val;
	RingBuffer *y_val;
	float x_min;
	float x_max;
	float y_min;
	float y_max;
};
typedef struct Curve Curve;



Curve *curve_create();
void curve_free(Curve **p_curve);

void curve_set_data_cap(Curve *curve, size_t cap);

void curve_push_x_data(Curve *curve, Vector *data);
void curve_push_y_data(Curve *curve, Vector *data);



// Creates a VAO for a curve.
VAO *curve_prepare_vao(float *x_val, float *y_val, int n);
