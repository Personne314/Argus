#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "glyphs.h"
#include "graph.h"



// Creates the FBO for the screenshot.
bool screenshot_fbo_create();

// Frees the FBO for the screenshot.
void screenshot_fbo_free();


// Defines the current screenshot size.
void screenshot_set_size(int width, int height);

// Defines the current screenshot save path.
void screenshot_set_path(const char *path);


// Takes a screenshot of a graph and saves it into a file.
bool screenshot_graph(Graph *graph, Glyphs *glyphs, const char *name);
