#pragma once

#include <stdbool.h>

#include "glyphs.h"
#include "structs.h"
#include "vao.h"



struct Axis {
	VAO *axis_vao;
	VAO *title_vao;
	const char *title;
	float min;
	float max;
	bool auto_adapt;
	bool log;
};
typedef struct Axis Axis;

#define AXIS_INIT (Axis){NULL, NULL, NULL, 0.0f, 0.0f, true, false}


bool axis_prepare_x_title(Axis *axis, Glyphs *glyphs, Rect *p_rect, int window_width, int window_height);
bool axis_prepare_y_title(Axis *axis, Glyphs *glyphs, Rect *p_rect, int window_width, int window_height);

bool axis_prepare_x_axis(Axis *axis, Glyphs *glyphs, float y, float range, float offset, 
	float base, float d, int n, int window_width, int window_height, float grid_width);
bool axis_prepare_y_axis(Axis *axis, Glyphs *glyphs, float x, float range, float offset, 
	float base, float d, int n, int window_width, int window_height, float grid_height);

void axis_reset_graphics(Axis *axis);
