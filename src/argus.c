#include "argus.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <stdbool.h>
#include <pthread.h>

#include "graph.h"

// Init state of the library.
static bool init = false;

// Argus window components.
static SDL_Window *window;		///< SDL window used to display the graphs.
static SDL_GLContext context;	///< OpenGL context for the rendering process.
static int width;	///< Width of the window.
static int height;	///< Height of the window.
static char *title;	///< Title of the window.

// Array of graphs to display in the window.
static int lines;			///< Number of lines in the graph grid.
static int columns;			///< Number of columns in the graph grid.
static int current_line;	///< Current graph line.
static int current_column;	///< Current graph column.
static Graph **grid;		///< Graphs grid.

static void (*update_func)(void*, double);	///< Function used to update the graph data.
static void* update_args;	///< Arguments to give to the update function.
static double dt;			///< Time interval to give to the update function.


// Main mutex used to make the library thread safe.
static pthread_mutex_t argus_mutex = PTHREAD_MUTEX_INITIALIZER;


// Macro to check if the module was initialized.
#define CHECK_INIT(init, mutex) do { \
	pthread_mutex_lock(&mutex); \
	if (!init) { \
		fprintf(stderr, "[ARGUS]: error: not initialized !\n"); \
		pthread_mutex_unlock(&mutex); \
		return; \
	} \
} while(0);




static void argus_window_render() {

}



/// @brief Warning for the user in case of argus_quit call forgotten at exit.
/// 
/// This function isn't ment to be called by the user. It is registered with atexit
/// to be automatically called at exit. This funciton warns the user if they forget
/// to call argus_quit before the end of the program.
///
/// @note This function automatically calls argus_quit when needed.
static void argus_not_quit_on_exit() {
	pthread_mutex_lock(&argus_mutex);
	bool should_quit = init;
	pthread_mutex_unlock(&argus_mutex);
	if (should_quit) {
		fprintf(stderr, "[ARGUS]: warning: the quit function wasn't called before exit ! Calling it now...\n");
		argus_quit();
	}
}

/// @brief Used to initialize the lib. 
/// @note This must be called before using this lib.
void argus_init() {
	pthread_mutex_lock(&argus_mutex);
	if (init) {
		fprintf(stderr, "[ARGUS]: error: already initialized !\n");
		pthread_mutex_unlock(&argus_mutex);
		return;
	}

	// Initialise tout les modules de la SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fprintf(stderr, "[ARGUS]: fatal: Error during SDL2 initialisation : %s\n", SDL_GetError());
		pthread_mutex_unlock(&argus_mutex);
		return;
	}

	// Initialise SDL_IMG pour la lecture des PNG.
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		fprintf(stderr, "[ARGUS]: fatal: Error during SDL_image initialisation : %s\n", IMG_GetError());
		SDL_Quit();
		pthread_mutex_unlock(&argus_mutex);
		return;
	}

	// Initialise SDL_TTF pour l'affichage de texte.
	if (TTF_Init() < 0) {
		fprintf(stderr, "[ARGUS]: fatal: Error during SDL_ttf initialisation : %s\n", IMG_GetError());
		SDL_Quit();
		IMG_Quit();
		pthread_mutex_unlock(&argus_mutex);
		return;
	}

	// Initialize the window data.
	window = NULL;
	context = NULL;
	width = 640;
	height = 480;
	title = "ARGUS Window";

	// Malloc the initial grid.
	lines = 1;
	columns = 1;
	current_line = 0;
	current_column = 0;
	grid = calloc(1,sizeof(Graph*));
	if (!grid) {
		fprintf(stderr, "[ARGUS]: fatal: failed to malloc the buffer for the graph grid !\n");
		SDL_Quit();
		IMG_Quit();
		pthread_mutex_unlock(&argus_mutex);
		return;
	}
	grid[0] = graph_create();
	if (!grid[0]) {
		fprintf(stderr, "[ARGUS]: fatal: failed to create the graphs of the window !\n");
		pthread_mutex_unlock(&argus_mutex);
		argus_quit();
		return;
	}

	// Reset the update function informations.
	update_func = NULL;
	update_args = NULL;
	dt = 1.0;

	// Initialization done.
	static bool atexit_registered = false;
	if (!atexit_registered) {
		atexit(argus_not_quit_on_exit);
		atexit_registered = true;
	}
	pthread_mutex_unlock(&argus_mutex);
	init = true;
}

/// @brief Frees the memory used and quit the SDL.
/// @note This must be called once you're done using this lib.
void argus_quit() {
	CHECK_INIT(init, argus_mutex);
	
	// Frees the argus variables.
	if (context) SDL_GL_DeleteContext(context);
	if (window) SDL_DestroyWindow(window);
	if (grid) {
		for (int i = 0; i < lines*columns; ++i)
			if (grid[i]) graph_free(grid[i]);
		free(grid);
	}

	// Quits the SDL. 
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	// Quit done.
	init = false;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Resizes the Argus window.
/// @param w width of the window.
/// @param h height of the window.
/// @note If w or h is negative or null, the default window size will be used (640*480). 
/// @note This function sets the current graph to (0,0)
void argus_set_window_size(int w, int h) {
	CHECK_INIT(init, argus_mutex)
	if (w <= 0 || h <= 0) {
		fprintf(stderr, "[ARGUS]: warning: w,h must be > 0 in argus_set_window_size. Default size will be used !\n");
		width = 640;
		height = 480;
	} else {
		width = w;
		height = h;
	}
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the graph grid dimensions
/// @param w Number of columns.
/// @param h Number of lines.
/// This function sets the dimensions of the graph grid in the window.
/// The window will contains m*n graphs rendered in a grid.
/// @note If m or n is lower than 1, its value will be set to 1.
/// @note A call to this function will destroys all previously created graphs.
void argus_set_graph_grid(int w, int h) {
	CHECK_INIT(init, argus_mutex)
	if (w < 1) {
		fprintf(stderr, "[ARGUS]: warning: number of columns lower than 1. Will be set to 1.\n");
		w = 1;
	}
	if (h < 1) {
		fprintf(stderr, "[ARGUS]: warning: number of lines lower than 1. Will be set to 1.\n");
		h = 1;
	}
	lines = h;
	columns = w;
	current_line = 0;
	current_column = 0;

	// Malloc the new grid.
	if (grid) {
		for (int i = 0; i < lines*columns; ++i)
			if (grid[i]) graph_free(grid[i]);
		free(grid);
	}
	grid = calloc(lines*columns, sizeof(Graph));
	if (!grid) {
		fprintf(stderr, "[ARGUS]: fatal: failed to malloc the buffer for the graph grid !\n");
		pthread_mutex_unlock(&argus_mutex);
		argus_quit();
		return;
	}

	// Creates the graphs.
	for (int i = 0; i < lines*columns; ++i) {
		grid[i] = graph_create();
		if (!grid[i]) {
			fprintf(stderr, "[ARGUS]: fatal: failed to create the graphs of the window !\n");
			pthread_mutex_unlock(&argus_mutex);
			argus_quit();
			return;
		}
	}
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the graph currently being manipulated.
/// @param x x coordinate in the grid.
/// @param y y coordinate in the grid.
/// @note If x or y is lower than 1, its value will be set to 1.
/// @note If x or y is greater than the amount of lines/columns, its value will the amount of lines/columns -1.
void argus_set_current_graph(int x, int y) {
	CHECK_INIT(init, argus_mutex)
	if (x < 0) {
		fprintf(stderr, "[ARGUS]: warning: columns id lower than 0. Will be set to 0.\n");
		x = 0;
	}
	if (y < 0) {
		fprintf(stderr, "[ARGUS]: warning: lines id lower than 0. Will be set to 0.\n");
		y = 0;
	}
	if (x < 1) {
		fprintf(stderr, "[ARGUS]: warning: columns id greater than %d. Will be set to %d.\n", columns-1, columns-1);
		x = columns-1;
	}
	if (y < 1) {
		fprintf(stderr, "[ARGUS]: warning: lines id lower than %d. Will be set to %d.\n", lines-1, lines-1);
		y = lines-1;
	}
	current_line = y;
	current_column = x;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Defines the update function of the graph data.
/// @param func Function pointer to use.
/// @param args Arguments to give to func on each call. 
/// @note func must takes a void* (args) and a double (the elapsed time between each call).
void argus_set_update_function(void (*func)(void*, double), void *args) {
	CHECK_INIT(init, argus_mutex)
	update_func = func;
	update_args = args;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Shows a window containing the defined graphs.
/// @note This function returns only when the created window is closed.
void argus_show() {
	CHECK_INIT(init, argus_mutex)

	// Creates the window.
	window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		width, height,
		SDL_WINDOW_OPENGL
	);
	if (!window) {
		fprintf(stderr, "[ARGUS]: Error during window creation : %s.\n", SDL_GetError());
		pthread_mutex_unlock(&argus_mutex);
		return;
	}

	// Sets the OpenGL attributes and creates the OpenGL context.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	context = SDL_GL_CreateContext(window);
	if (!context) {
		fprintf(stderr, "[ARGUS]: Error during OpenGL context creation : %s.\n", SDL_GetError());
		SDL_DestroyWindow(window);
		window = NULL;
		pthread_mutex_unlock(&argus_mutex);
		return;
	}
	SDL_GL_SetSwapInterval(0);

	// Renders the window.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_GL_SwapWindow(window);
	argus_window_render();

	// Window main loop.
	bool run = true;
	uint32_t last_loop = 0;
	double updates_to_do = 0.0;
	while (run) {

		// Sleeps if there is nothing to do.
		uint32_t time = SDL_GetTicks();
		if (time-last_loop < 20) {
			SDL_Delay(1);
			continue;	
		}

		// Catch the windows events.
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: 
				run = false; 
				break;
			}
		}

		// If there is an update function, calculates the amount of  
		if (update_func) {
			updates_to_do += (time-last_loop)/(1000.0*dt);
			int loops_to_do = (int)updates_to_do;
			updates_to_do -= loops_to_do;
			for (int i = 0; i < loops_to_do; ++i) {
				update_func(update_args, dt);
			}

			// Renders the window to update the shown data.
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			argus_window_render();
			SDL_GL_SwapWindow(window);

		}
		last_loop = time;
	}

	// Destroys the windows.
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	context = NULL;
	window = NULL;
	pthread_mutex_unlock(&argus_mutex);
}
