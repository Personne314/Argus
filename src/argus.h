#pragma once

#include "structs.h"



////////////////////////////////////////////////////////////////
//                       Init functions                       //
////////////////////////////////////////////////////////////////

// Used to initialize the lib. 
void argus_init();

// Frees the memory used by the lib.
void argus_quit();


////////////////////////////////////////////////////////////////
//                         Setters                            //
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


////////////////////////////////////////////////////////////////
//                    Rendering function                      //
////////////////////////////////////////////////////////////////

// Shows a window containing the defined graphs.
void argus_show();
