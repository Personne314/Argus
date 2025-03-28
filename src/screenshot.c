#include "screenshot.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

#include "curve.h"
#include "curves.h"



static GLuint fbo_texture = 0;
static GLuint fbo		  = 0;
static size_t fbo_width	  = 0;
static size_t fbo_height  = 0;


bool screenshot_fbo_create(size_t width, size_t height) {
	screenshot_fbo_free();
	if (!width || !height) {
		fprintf(stderr, "[ARGUS]: error: (%ld,%ld) isn't a valid dimension "
			"to create the screenshot FBO!\n", width, height);
		return false;
	}
	fbo_width = width;
	fbo_height = height;


	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glGenTextures(1, &fbo_texture);
		glBindTexture(GL_TEXTURE_2D, fbo_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			fprintf(stderr, "[ARGUS]: error: unable to create the screenshot FBO!\n");
			glBindFramebuffer(GL_FRAMEBUFFER, 0); 
			glDrawBuffer(GL_BACK);
			screenshot_fbo_free();
			return false;
		}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glDrawBuffer(GL_BACK);
	return true;
}

void screenshot_fbo_free() {
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &fbo_texture);
	fbo_width = 0;
	fbo_height = 0;
	fbo_texture = 0;
	fbo = 0;
}




static bool screenshot_save_fbo_content(const char *path) {

	// Récupère les pixels de l'image depuis la VRAM.
	GLubyte* pixels = malloc(fbo_width*fbo_height*4);
	if (!pixels) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc the screenshot pixel buffer!\n");
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
		pixels, fbo_width, fbo_height, 32, fbo_width*4, 
		0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
	);

	IMG_SavePNG(surface, path);
	
	SDL_FreeSurface(surface);
	free(pixels);
	return true;
}



void *fbo_window;

bool screenshot_graph(Graph *graph, Glyphs *glyphs, const char *name) {
	if (!fbo) {
		fprintf(stderr, "[ARGUS]: error: the screenshot FBO must be "
			"created before trying to make a graph screenshot!\n");
		return false;
	}

	Graph *graph_fullscreen = graph_create((Rect){0,0,1,1});
	
	const size_t n = graph->curves->size;
	void *data = n > 0 ? realloc(graph_fullscreen->curves->data, n*sizeof(Curve*)) : graph_fullscreen->curves->data;
	if (!data) {
		fprintf(stderr, "[ARGUS]: error: unable to creates the fullscreen graph for the screenshot rendering!\n");
		graph_free(&graph_fullscreen);
		return false;
	}
	graph_fullscreen->curves->data = data;
	graph_fullscreen->curves->size = n;
	for (size_t i = 0; i < n; i++) {
		graph_fullscreen->curves->data[i] = graph->curves->data[i];
	}
	
	graph_prepare_static(graph_fullscreen, glyphs, fbo_width, fbo_height);
	graph_prepare_dynamic(graph_fullscreen, glyphs, fbo_width, fbo_height);

	GLint old_viewport[4];
	glGetIntegerv(GL_VIEWPORT, old_viewport);
	glViewport(0, 0, fbo_width, fbo_height);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		graph_render(graph_fullscreen, glyphs);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glDrawBuffer(GL_BACK);
	glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);

	graph_fullscreen->curves->size = 0;
	for (size_t i = 0; i < n; i++) {
		graph_fullscreen->curves->data[i] = NULL;
	}
	graph_free(&graph_fullscreen);





	screenshot_save_fbo_content(name);




	return true;
}
