#pragma once



struct Graph {
	const char *title;
};
typedef struct Graph Graph;


Graph *graph_create();
void graph_free(Graph *graph);

void graph_render(Graph *graph);
