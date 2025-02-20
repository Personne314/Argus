#pragma once

#include <stdbool.h>

#include "vao.h"
#include "shader.h"
#include "glyphs.h"


struct Graph {
	Glyphs *glyphs;
	const char *title;
	float x;
	float y;
	float w;
	float h;



	
	VAO *vao;
};
typedef struct Graph Graph;


Graph *graph_create(Glyphs *glyphs, int window_width, int window_height, float x, float y, float w, float h);
void graph_free(Graph *graph);

void graph_set_title(const char *title);
bool graph_prepare_dynamic(Graph *graph);
void graph_reset_graphics(Graph *graph);

void graph_render(Graph *graph);
