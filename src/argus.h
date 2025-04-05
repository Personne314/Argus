#pragma once

#include <stddef.h>
#include <stdint.h>

#include "structs.h"
#include "vector.h"
#include "enums.h"



////////////////////////////////////////////////////////////////
//                       Init functions                       //
////////////////////////////////////////////////////////////////

// Used to initialize the lib. 
void argus_init();

// Returns the init state of the lib.
bool argus_is_init();

// Frees the memory used by the lib.
void argus_quit();


////////////////////////////////////////////////////////////////
//                     Global Setters                         //
////////////////////////////////////////////////////////////////

// Resizes the Argus window.
void argus_set_size(int w, int h);

// Sets the window title.
void argus_set_title(const char *window_title);

// Sets the graph grid dimensions.
void argus_set_grid_size(int m, int n);

// Sets the graph currently being manipulated.
void argus_set_current_graph(int x, int y);

// Defines the current screenshot save path.
void argus_set_screenshot_path(const char *path);

// Defines the current screenshot size.
void argus_set_screenshot_size(int width, int height);

// Sets the update frequency of the data.
void argus_set_update_frequency(float f);

// Sets the update duration of the data.
void argus_set_update_duration(float d);

// Sets the update timestep of the data.
void argus_set_update_timestep(float t);

// Sets the window render frequency.
void argus_set_render_frequency(float f);

// Sets the window background color.
void argus_set_background_color(Color c);


////////////////////////////////////////////////////////////////
//                     Graph functions                        //
////////////////////////////////////////////////////////////////

// Sets the current graph title.
void argus_graph_set_title(const char *graph_title);

// Sets the current graph title color.
void argus_graph_set_title_color(Color c);

// Sets the current graph background color.
void argus_graph_set_background_color(Color c);

// Sets the current graph color.
void argus_graph_set_color(Color c);

// Sets the current graph x axis title.
void argus_graph_set_x_title(const char *axis_title);

// Sets the current graph y axis title.
void argus_graph_set_y_title(const char *axis_title);

// Sets the current graph text color.
void argus_graph_set_text_color(Color c);

// Adds a new curve to the current graph.
void argus_graph_add_curve();

// Removes the current curve from the current graph.
void argus_graph_remove_curve();

// Sets the curve currently being manipulated.
void argus_graph_set_current_curve(size_t id);

// Returns the number of curves in the current graph.
size_t argus_graph_get_curve_amount();

// Sets the adapt parameter for both axis.
void argus_graph_adapt(AxisAdaptMode mode);

// Sets the adapt parameter for x axis.
void argus_graph_adapt_x(AxisAdaptMode mode);

// Sets the adapt parameter for y axis.
void argus_graph_adapt_y(AxisAdaptMode mode);

// Sets the limits of the x axis of the current graph.
void argus_graph_set_x_limits(float min, float max);

// Sets the limits of the y axis of the current graph.
void argus_graph_set_y_limits(float min, float max);


////////////////////////////////////////////////////////////////
//                      Curve functions                       //
////////////////////////////////////////////////////////////////

// Sets the data size of the current curve in the current graph.
void argus_curve_set_size(size_t size);

// Sets the color of the current curve in the current graph.
void argus_curve_set_color(Color color);

// Adds data to the x values of the current curve in the current graph.
void argus_curve_add_x(Vector *data);

// Adds data to the y values of the current curve in the current graph.
void argus_curve_add_y(Vector *data);

// Adds data to the x values of the current curve in the current graph.
void argus_curve_add_x_raw(float *data, size_t n);

// Adds data to the y values of the current curve in the current graph.
void argus_curve_add_y_raw(float *data, size_t n);

// Sets the update function of the current curve.
void argus_curve_set_update_function(void (*func)(float *x, float *y, double dt));


////////////////////////////////////////////////////////////////
//                    Rendering function                      //
////////////////////////////////////////////////////////////////

// Shows a window containing the defined graphs.
void argus_show();
