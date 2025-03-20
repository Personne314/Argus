#pragma once

#include "vector.h"
#include "vao.h"



struct Curve {
	Vector *x_val;
	Vector *y_val;
	float x_min;
	float x_max;
	float y_min;
	float y_max;
};
typedef struct Curve Curve;



// Creates a VAO for a curve.
VAO *curve_prepare_vao(float *x_val, float *y_val, int n);
