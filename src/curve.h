#pragma once

#include "ring_buffer.h"
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
void curve_free(Curve **curve);


// Creates a VAO for a curve.
VAO *curve_prepare_vao(float *x_val, float *y_val, int n);
