#pragma once

#include <stdbool.h>

#include "glyphs.h"
#include "structs.h"
#include "vao.h"


/// @struct Axis
/// @brief Used to create an axis (x or y) for a graph.
typedef struct {
	VAO *axis_vao;		///< VAO to render the graduations.
	VAO *title_vao;		///< VAO to render the title.
	char *title;		///< Title of the axis.
	float min;			///< Min value of the axis.
	float max;			///< Max value of the axis.
	AxisAdaptMode auto_adapt;	///< The adapt mode to use.
} Axis;

// Default axis value.
#define AXIS_INIT (Axis){NULL, NULL, NULL, 0.0f, 1.0f, true}


// Prepares the x axis title.
bool axis_prepare_x_title(Axis *axis, Glyphs *glyphs, Rect *p_rect, int window_width, int window_height);

// Prepares the y axis title.
bool axis_prepare_y_title(Axis *axis, Glyphs *glyphs, Rect *p_rect, int window_width, int window_height);

// Prepares the x axis graduation depending of the min and max values.
bool axis_prepare_x_axis(Axis *axis, Glyphs *glyphs, Rect *p_grid_rect, float range, float offset, 
	float base, float d, int n, int window_width, int window_height);

// Prepares the y axis graduation depending of the min and max values.
bool axis_prepare_y_axis(Axis *axis, Glyphs *glyphs, Rect *p_grid_rect, float range, float offset, 
	float base, float d, int n, int window_width, int window_height);

// Reset the graphical components of an axis after the rendering process.
void axis_reset_graphics(Axis *axis);
