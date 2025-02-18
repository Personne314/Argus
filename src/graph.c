#include "graph.h"

#include <stdio.h>
#include <stdlib.h>

#include "shader.h"



Graph *graph_create(int window_width, int window_height, float x, float y, float w, float h) {
	Graph *graph = malloc(sizeof(Graph));
	if (!graph) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a Graph");
		return NULL;
	}
	graph->x = x;
	graph->y = y;
	graph->w = w;
	graph->h = h;
	graph->vao = NULL;
	
	return graph;
}

void graph_free(Graph *graph) {
	if (graph->vao) vao_free(graph->vao);
	free(graph);
}





void graph_set_title(const char *title) {

}


bool graph_prepare_static(Graph *graph, int window_width, int window_height) {

}


bool graph_prepare_dynamic(Graph *graph) {
	float vertices[12] = {
		graph->x,graph->y, graph->x+graph->w,graph->y+graph->h, graph->x,graph->y+graph->h, 
		graph->x,graph->y, graph->x+graph->w,graph->y, graph->x+graph->w,graph->y+graph->h
	};
	float colors[24] = {1,1,1,1, 0,1,1,1, 1,1,0,1, 1,1,1,1, 1,0,1,1, 0,1,1,1};
	void *data[2] = {vertices, colors};
	int sizes[2] = {2,4};
	int array_ids[2] = {0,1};
	int gl_types[2] = {GL_FLOAT,GL_FLOAT};
	graph->vao = vao_create(data, sizes, array_ids, gl_types, 6,2);
	return false;
}

void graph_reset_graphics(Graph *graph) {
	vao_free(graph->vao);
	graph->vao = NULL;
}

void graph_render(Graph *graph) {
	shader_use(shaders[SHADER_SHAPE]);
		vao_bind(graph->vao);
			glUniform1f(shader_uniform_location(shaders[SHADER_SHAPE], "transparency"), 1.0f);
			glDrawArrays(GL_TRIANGLES, 0, graph->vao->size);
		vao_bind(NULL);
	shader_use(NULL);
}
