#pragma once

#include <stddef.h>
#include <stdbool.h>

#include "glyphs.h"
#include "graph.h"

extern void *fbo_window;
bool screenshot_fbo_create(size_t width, size_t height);
void screenshot_fbo_free();

bool screenshot_graph(Graph *graph, Glyphs *glyphs, const char *name);
