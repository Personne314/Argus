#pragma once


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
void argus_set_window_size(int w, int h);

// Sets the graph grid dimensions.
void argus_set_graph_grid(int m, int n);

// Sets the graph currently being manipulated.
void argus_set_current_graph(int x, int y);

// Defines the update function of the graph data.
void argus_set_update_function(void (*func)(void*, double), void *args);


////////////////////////////////////////////////////////////////
//                    Rendering function                      //
////////////////////////////////////////////////////////////////

// Shows a window containing the defined graphs.
void argus_show();
