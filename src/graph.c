#include "graph.h"

#include <stdio.h>
#include <stdlib.h>


Graph *graph_create() {
	Graph *graph = malloc(sizeof(Graph));
	if (!graph) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a Graph");
		return NULL;
	}


	return graph;

}

void graph_free(Graph *graph) {
	free(graph);
}


void graph_render(Graph *graph) {

}
