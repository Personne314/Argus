#pragma once

#include "structs.h"
#include "vector.h"



////////////////////////////////////////////////////////////////
//                       Init functions                       //
////////////////////////////////////////////////////////////////

// Used to initialize the lib. 
void argus_init();

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

// Defines the update function of the graph data.
void argus_set_update_function(void (*func)(void*, double), void *args);


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
void argus_graph_set_graph_color(Color c);

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
size_t argus_graph_get_current_curve_number();


////////////////////////////////////////////////////////////////
//                      Curve functions                       //
////////////////////////////////////////////////////////////////

// Sets the data size of the current curve in the current graph.
void argus_graph_curve_set_size(size_t size);

// Adds the x values of the current curve in the current graph.
void argus_graph_curve_add_x(Vector *data);

// Adds the y values of the current curve in the current graph.
void argus_graph_curve_add_y(Vector *data);


////////////////////////////////////////////////////////////////
//                    Rendering function                      //
////////////////////////////////////////////////////////////////

// Shows a window containing the defined graphs.
void argus_show();
