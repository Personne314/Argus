#include "screenshot.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include "curve.h"
#include "curves.h"



// OpenGL FBO used to render screenshots.
static GLuint fbo		  = 0;	//< The FBO OpenGL id.
static GLuint fbo_texture = 0;	//< The FBO's texture OpenGL id.
static size_t fbo_width	  = 0;	//< The FBO's texture width.
static size_t fbo_height  = 0;	//< The FBO's texture height.

// Parameters of the screenshots.
static size_t screenshot_width = 480;		 //< The width of the screenshots.
static size_t screenshot_height = 640;		 //< The height of the screenshots.
static const char *screenshot_folder = "./"; //< The folder where to save the screenshots.



/// @brief Creates the FBO for the screenshot.
/// @return false if there was an error. 
bool screenshot_fbo_create() {
	screenshot_fbo_free();
	if (!screenshot_width || !screenshot_height) {
		fprintf(stderr, "[ARGUS]: error: (%ld,%ld) isn't a valid dimension "
			"to create the screenshot FBO!\n", screenshot_width, screenshot_height);
		return false;
	}

	// Creates the FBO.
	fbo_width = screenshot_width;
	fbo_height = screenshot_height;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// Creates the FBOs texture.
		glGenTextures(1, &fbo_texture);
		glBindTexture(GL_TEXTURE_2D, fbo_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenshot_width, screenshot_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

		// Checks if the FBO is well-built.
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			fprintf(stderr, "[ARGUS]: error: unable to create the screenshot FBO!\n");
			glBindFramebuffer(GL_FRAMEBUFFER, 0); 
			glDrawBuffer(GL_BACK);
			screenshot_fbo_free();
			return false;
		}

	// Unbinds the FBO.
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glDrawBuffer(GL_BACK);
	return true;
}

/// @brief Frees the FBO for the screenshot.
void screenshot_fbo_free() {
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &fbo_texture);
	fbo_width = 0;
	fbo_height = 0;
	fbo_texture = 0;
	fbo = 0;
}


/// @brief Defines the current screenshot size.
/// @param width The width of the screenshot.
/// @param height The height of the screenshot.
/// @note width and height must be greater than 0.
void screenshot_set_size(size_t width, size_t height) {
	if (!width || !height) {
		fprintf(stderr, "[ARGUS]: error: (%ld, %ld) is not a valid dimension for screenshots !\n", width, height);
		return;
	}
	screenshot_width = width;
	screenshot_height = height;
}

/// @brief Defines the current screenshot save path.
/// @param path The location where to save the screenshot.
void screenshot_set_path(const char *path) {
	if (!path || !strlen(path)) screenshot_folder = "./";
	else screenshot_folder = path;
}


/// @brief This function saves the content of the FBO in a .png file.
/// @param path The path where to save the image.
/// @return false if there was an error.
static bool screenshot_save_fbo_content(const char *path) {

	// Gets the image from VRAM.
	GLubyte* pixels = malloc(fbo_width*fbo_height*4);
	if (!pixels) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc the screenshot pixel buffer!\n");
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Inverts the lines of the image besause the texture y-coordinate of the
	// FBO are inverted compared with the screen y-coordinate.
	for (size_t i = 0; i < fbo_height / 2; i++) {
		GLubyte *row1 = pixels + i * fbo_width * 4;
		GLubyte *row2 = pixels + (fbo_height - 1 - i) * fbo_width * 4;
		for (size_t j = 0; j < fbo_width * 4; j++) {
			GLubyte temp = row1[j];
			row1[j] = row2[j];
			row2[j] = temp;
		}
	}

	// Converts the pixels into a surface.
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
		pixels, fbo_width, fbo_height, 32, fbo_width*4, 
		0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
	);
	if (!pixels) {
		fprintf(stderr, "[ARGUS]: error: unable to create the screenshot surface!\n");
		free(pixels);
		return false;
	}

	// Saves the image.
	IMG_SavePNG(surface, path);
	SDL_FreeSurface(surface);
	free(pixels);
	return true;
}


/// @brief Takes a screenshot of a graph and saves it into a file.
/// @param graph The graph to take a screen of.
/// @param glyphs The glyphs used to render the graph.
/// @param name The name of the .png file where to store the image.
/// @return false if there was an error.
bool screenshot_graph(Graph *graph, Glyphs *glyphs, const char *name) {
	if (!fbo) {
		fprintf(stderr, "[ARGUS]: error: the screenshot FBO must be "
			"created before trying to make a graph screenshot!\n");
		return false;
	}

	// Creates a copy of the graph used to generate VAOs adapted to the screenshot dimension.
	Graph *graph_fullscreen = graph_create((Rect){0,0,1,1});
	if (!graph_fullscreen) {
		fprintf(stderr, "[ARGUS]: error: unable to creates the fullscreen graph for the screenshot rendering!\n");
		return false;
	}
	
	// Copies the curves into the temp graph.
	const size_t n = graph->curves->size;
	void *data = n > 0 ? realloc(graph_fullscreen->curves->data, n*sizeof(Curve*)) : graph_fullscreen->curves->data;
	if (!data) {
		fprintf(stderr, "[ARGUS]: error: unable to realloc the curve data buffer for the screenshot graph!\n");
		graph_free(&graph_fullscreen);
		return false;
	}
	graph_fullscreen->curves->data = data;
	graph_fullscreen->curves->size = n;
	for (size_t i = 0; i < n; i++) {
		graph_fullscreen->curves->data[i] = graph->curves->data[i];
	}
	
	// Prepares the graph.
	graph_fullscreen->title = graph->title;
	graph_fullscreen->x_axis.title = graph->x_axis.title;
	graph_fullscreen->y_axis.title = graph->y_axis.title;
	graph_prepare_static(graph_fullscreen, glyphs, fbo_width, fbo_height);
	graph_prepare_dynamic(graph_fullscreen, glyphs, fbo_width, fbo_height);
	imagebutton_free(&graph_fullscreen->save);

	// Renders the screenshot in the FBO.
	GLint old_viewport[4];
	glGetIntegerv(GL_VIEWPORT, old_viewport);
	glViewport(0, 0, fbo_width, fbo_height);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		graph_render(graph_fullscreen, glyphs);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glDrawBuffer(GL_BACK);
	glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);

	// Frees the temp graph.
	graph_fullscreen->curves->size = 0;
	for (size_t i = 0; i < n; i++) graph_fullscreen->curves->data[i] = NULL;
	graph_free(&graph_fullscreen);

	// Creates the path of the screenshot image.
	const size_t name_len = strlen(name);
	const size_t screenshot_folder_len = strlen(screenshot_folder);
	char *path = malloc(name_len+screenshot_folder_len+5);
	if (!path) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc the path buffer for the screenshot graph!\n");
		return false;
	}
	memcpy(path, screenshot_folder, screenshot_folder_len);
	memcpy(path+screenshot_folder_len, name, name_len);
	memcpy(path+screenshot_folder_len+name_len, ".png", 5);

	// Saves the generated screenshot.
	screenshot_save_fbo_content(path);
	return true;
}
