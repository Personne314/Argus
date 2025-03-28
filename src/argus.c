#include "argus.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glew.h>
#include <stdbool.h>
#include <pthread.h>

#include "graph.h"
#include "shader.h"
#include "glyphs.h"
#include "curves.h"
#include "curve.h"
#include "screenshot.h"
#include "button.h"



////////////////////////////////////////////////////////////////
//                     Library components                     //
////////////////////////////////////////////////////////////////

// Init state of the library.
static bool init = false;

// Argus window components.
static SDL_Window *window;		///< SDL window used to display the graphs.
static SDL_GLContext context;	///< OpenGL context for the rendering process.
static int width;			///< Width of the window.
static int height;			///< Height of the window.
static const char *title;	///< Title of the window.

// Array of graphs to display in the window.
static int lines;			///< Number of lines in the graph grid.
static int columns;			///< Number of columns in the graph grid.
static int current_line;	///< Current graph line.
static int current_column;	///< Current graph column.
static float offset_width;	///< Width of the offsets between each graph.
static float offset_height;	///< Height of the offsets between each graph.
static Graph **grid;		///< Graphs grid.

// Current curve id.
static int current_curve;

// Update function.
static void (*update_func)(void*, double);	///< Function used to update the graph data.
static void* update_args;	///< Arguments to give to the update function.
static double dt;			///< Time interval to give to the update function.

// Main mutex used to make the library thread safe.
static pthread_mutex_t argus_mutex = PTHREAD_MUTEX_INITIALIZER;

// Macro to check if the module was initialized.
#define CHECK_INIT(init, mutex, ...) do { \
    pthread_mutex_lock(&mutex); \
    if (!(init)) { \
        fprintf(stderr, "[ARGUS]: fatal: not initialized !\n"); \
        pthread_mutex_unlock(&mutex); \
        return __VA_ARGS__; \
    } \
} while(0);

// Macro to get the current graph.
#define CURRENT_GRAPH grid[current_line*columns+current_column]

// Macro to get the current curve.
// Always check if current_curve >= 0 before using this.
#define CURRENT_CURVE grid[current_line*columns+current_column]->curves->data[current_curve]



////////////////////////////////////////////////////////////////
//                       Init functions                       //
////////////////////////////////////////////////////////////////

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

	// Resets the data.
	window = NULL;
	context = NULL;
	grid = NULL;
	update_func = NULL;
	update_args = NULL;
	for (int i = 0; i < SHADERNAME_SIZE; ++i) shaders[i] = NULL;
	dt = 1.0;
	width = 640;
	height = 480;
	current_line = -1;
	title = "ARGUS Window";

	// Malloc the initial grid.
	lines = 1;
	columns = 1;
	offset_width = 10.0f/width;
	offset_height = 10.0f/height;
	current_line = 0;
	current_column = 0;
	grid = calloc(1,sizeof(Graph*));
	if (!grid) {
		fprintf(stderr, "[ARGUS]: fatal: failed to malloc the buffer for the graph grid !\n");
		pthread_mutex_unlock(&argus_mutex);
		argus_quit();
		return;
	}
	grid[0] = graph_create((Rect){offset_width, offset_height, 1.0f-2.0f*offset_width, 1.0f-2.0f*offset_height});
	if (!grid[0]) {
		fprintf(stderr, "[ARGUS]: fatal: failed to create the graphs of the window !\n");
		pthread_mutex_unlock(&argus_mutex);
		argus_quit();
		return;
	}

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
	if (grid) {
		for (int i = 0; i < lines*columns; ++i) graph_free(grid+i);
		free(grid);
	}
	for (int i = 0; i < SHADERNAME_SIZE; ++i) shader_free(shaders+i);
	if (context) SDL_GL_DeleteContext(context);
	if (window) SDL_DestroyWindow(window);

	// Quits the SDL. 
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	// Quit done.
	init = false;
	pthread_mutex_unlock(&argus_mutex);
}





////////////////////////////////////////////////////////////////
//                         Setters                            //
////////////////////////////////////////////////////////////////

/// @brief Resizes the Argus window.
/// @param w Width of the window.
/// @param h Height of the window.
/// @note If w or h is negative or null, the default window size will be used (640*480). 
/// @note This function sets the current graph to (0,0)
void argus_set_size(int w, int h) {
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

/// @brief Sets the window title.
/// @param title The title of the window.
void argus_set_title(const char *window_title) {
	title = window_title;
}

/// @brief Sets the graph grid dimensions.
/// @param w Number of columns.
/// @param h Number of lines.
/// This function sets the dimensions of the graph grid in the window.
/// The window will contains m*n graphs rendered in a grid.
/// @note If m or n is lower than 1, its value will be set to 1.
/// @note A call to this function will destroys all previously created graphs.
void argus_set_grid_size(int w, int h) {
	CHECK_INIT(init, argus_mutex)
	if (w < 1) {
		fprintf(stderr, "[ARGUS]: warning: number of columns lower than 1. Will be set to 1.\n");
		w = 1;
	}
	if (h < 1) {
		fprintf(stderr, "[ARGUS]: warning: number of lines lower than 1. Will be set to 1.\n");
		h = 1;
	}

	// Clears the old grid.
	if (grid) {
		for (int i = 0; i < lines*columns; ++i) graph_free(grid+i);
		free(grid);
	}

	// Malloc the new grid.
	lines = h;
	columns = w;
	offset_width = 10.0f/width;
	offset_height = 10.0f/height;
	current_line = 0;
	current_column = 0;
	grid = calloc(lines*columns, sizeof(Graph));
	if (!grid) {
		fprintf(stderr, "[ARGUS]: fatal: failed to malloc the buffer for the graph grid !\n");
		pthread_mutex_unlock(&argus_mutex);
		argus_quit();
		return;
	}

	// Creates the graphs.
	float graph_width = (1.0f-offset_width)/columns - offset_width;
	float graph_height = (1.0f-offset_height)/lines - offset_height;
	for (int i = 0; i < lines*columns; ++i) {
		int c = i%columns;
		int l = i/columns;
		grid[i] = graph_create((Rect){
			offset_width + c*(offset_width+graph_width), 
			offset_height + l*(offset_height+graph_height),
			graph_width, graph_height
		});
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
	current_curve = -1;
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

/// @brief Defines the current screenshot save path.
/// @param path The location where to save the screenshot.
void argus_set_screenshot_path(const char * path) {
	screenshot_set_path(path);
}

/// @brief Defines the current screenshot size.
/// @param width The width of the screenshot.
/// @param height The height of the screenshot.
/// @note width and height must be greater than 0.
void argus_set_screenshot_size(size_t width, size_t height) {
	screenshot_set_size(width, height);
}





////////////////////////////////////////////////////////////////
//                     Graph functions                        //
////////////////////////////////////////////////////////////////

/// @brief Sets the current graph title.
/// @param title The title of the graph.
void argus_graph_set_title(const char *graph_title) {
	CHECK_INIT(init, argus_mutex)
	CURRENT_GRAPH->title = graph_title;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the current graph title color.
/// @param c The color to use.
void argus_graph_set_title_color(Color c) {
	CHECK_INIT(init, argus_mutex)
	CURRENT_GRAPH->title_color.r = c.r;
	CURRENT_GRAPH->title_color.g = c.g;
	CURRENT_GRAPH->title_color.b = c.b;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the current graph background color.
/// @param c The color to use.
void argus_graph_set_background_color(Color c) {
	CHECK_INIT(init, argus_mutex)
	CURRENT_GRAPH->background_color.r = c.r;
	CURRENT_GRAPH->background_color.g = c.g;
	CURRENT_GRAPH->background_color.b = c.b;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the current graph color.
/// @param c The color to use.
void argus_graph_set_graph_color(Color c) {
	CHECK_INIT(init, argus_mutex)
	CURRENT_GRAPH->graph_color.r = c.r;
	CURRENT_GRAPH->graph_color.g = c.g;
	CURRENT_GRAPH->graph_color.b = c.b;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the current graph x axis title.
/// @param axis_title The title of the graph x axis.
void argus_graph_set_x_title(const char *axis_title) {
	CHECK_INIT(init, argus_mutex)
	CURRENT_GRAPH->x_axis.title = axis_title;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the current graph y axis title.
/// @param axis_title The title of the graph y axis.
void argus_graph_set_y_title(const char *axis_title) {
	CHECK_INIT(init, argus_mutex)
	CURRENT_GRAPH->y_axis.title = axis_title;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the current graph text color.
/// @param c The color to use.
void argus_graph_set_text_color(Color c) {
	CHECK_INIT(init, argus_mutex)
	CURRENT_GRAPH->text_color = c;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Adds a new curve to the current graph.
/// @note This should be called at least once on a graph before trying to draw anything.
void argus_graph_add_curve() {
	CHECK_INIT(init, argus_mutex)
	curves_push_back_curve(CURRENT_GRAPH->curves);
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Removes the current curve from the current graph.
/// @note After the call, the current curve is set to the preceding curve, or
/// the next one if the deleted curve was the first one, or -1 if the deleted
/// curve was the last one.
void argus_graph_remove_curve() {
	CHECK_INIT(init, argus_mutex)
	if (current_curve >= 0) {
		curves_delete_curve(CURRENT_GRAPH->curves, current_curve);
		if (curves_size(CURRENT_GRAPH->curves)) {
			current_curve = current_curve ? current_curve - 1 : 0;
		} else current_curve = -1;
	}
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the curve currently being manipulated.
/// @param id The id of the curve to manipulate.
/// @note id must be a valid index between 0 and the number of curves in the graph-1.
void argus_graph_set_current_curve(size_t id) {
	CHECK_INIT(init, argus_mutex)
	if (id >= curves_size(CURRENT_GRAPH->curves)) {
		fprintf(stderr, "[ARGUS]: error: %ld isn't an existing curve id ! The current curve won't be changed.\n", id);	
	} else current_curve = id;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Returns the number of curves in the current graph.
size_t argus_graph_get_current_curve_number() {
	CHECK_INIT(init, argus_mutex, 0)
	return curves_size(CURRENT_GRAPH->curves);
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the auto-adapt parameter for both axis.
/// @note If true, the axis min/max will be automatically changed to match the contained curves.
/// @param adapt The value of the auto-adapt parameter.
void argus_graph_auto_adapt(bool adapt) {
	CHECK_INIT(init, argus_mutex)
	CURRENT_GRAPH->x_axis.auto_adapt = adapt;
	CURRENT_GRAPH->y_axis.auto_adapt = adapt;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the auto-adapt parameter for x axis.
/// @note If true, the axis min/max will be automatically changed to match the contained curves.
/// @param adapt The value of the auto-adapt parameter.
void argus_graph_auto_adapt_x(bool adapt) {
	CHECK_INIT(init, argus_mutex)
	CURRENT_GRAPH->x_axis.auto_adapt = adapt;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the auto-adapt parameter for y axis.
/// @note If true, the axis min/max will be automatically changed to match the contained curves.
/// @param adapt The value of the auto-adapt parameter.
void argus_graph_auto_adapt_y(bool adapt) {
	CHECK_INIT(init, argus_mutex)
	CURRENT_GRAPH->y_axis.auto_adapt = adapt;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the limits of the x axis of the current graph.
/// @param min The minimal value of the axis.
/// @param max The maximal value of the axis.
/// @note This function will deactivate the auto-adapt mode if it's activated.
void argus_graph_set_x_limits(float min, float max) {
	CHECK_INIT(init, argus_mutex)
	if (min >= max) {
		fprintf(stderr, "[ARGUS]: error: min (%f) > max (%f) ! The axis limits won't change.\n", min, max);
		return;
	}
	if (CURRENT_GRAPH->x_axis.auto_adapt) {
		fprintf(stderr, "[ARGUS]: warning: x axis was on auto-adapt mode. This will be deactivated.\n");
		CURRENT_GRAPH->x_axis.auto_adapt = false;
	}
	CURRENT_GRAPH->x_axis.min = min;
	CURRENT_GRAPH->x_axis.max = max;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the limits of the y axis of the current graph.
/// @param min The minimal value of the axis.
/// @param max The maximal value of the axis.
/// @note This function will deactivate the auto-adapt mode if it's activated.
void argus_graph_set_y_limits(float min, float max) {
	CHECK_INIT(init, argus_mutex)
	if (min >= max) {
		fprintf(stderr, "[ARGUS]: error: min (%f) > max (%f) ! The axis limits won't change.\n", min, max);
		return;
	}
	if (CURRENT_GRAPH->y_axis.auto_adapt) {
		fprintf(stderr, "[ARGUS]: warning: y axis was on auto-adapt mode. This will be deactivated.\n");
		CURRENT_GRAPH->y_axis.auto_adapt = false;
	}
	CURRENT_GRAPH->y_axis.min = min;
	CURRENT_GRAPH->y_axis.max = max;
	pthread_mutex_unlock(&argus_mutex);
}





////////////////////////////////////////////////////////////////
//                      Curve functions                       //
////////////////////////////////////////////////////////////////

/// @brief Sets the data size of the current curve in the current graph.
/// @param size The size of the data buffer.
/// @note size must be greater than 0.
void argus_curve_set_size(size_t size) {
	CHECK_INIT(init, argus_mutex)
	if (current_curve < 0) {
		fprintf(stderr, "[ARGUS]: warning: There is not any selected curve. The size won't change.\n");	
		return;
	}
	curve_set_data_cap(CURRENT_CURVE, size);
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Sets the color of the current curve in the current graph.
/// @param color The color of the curve.
void argus_curve_set_color(Color color) {
	CHECK_INIT(init, argus_mutex)
	if (current_curve < 0) {
		fprintf(stderr, "[ARGUS]: warning: There is not any selected curve. The color won't change.\n");	
		return;
	}
	CURRENT_CURVE->color = color;
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Adds data to the x values of the current curve in the current graph.
/// @param data The vector containing the data to add to the curve.
void argus_curve_add_x(Vector *data) {
	CHECK_INIT(init, argus_mutex)
	if (current_curve < 0) {
		fprintf(stderr, "[ARGUS]: warning: There is not any selected curve. The x data won't change.\n");	
		return;
	}
	curve_push_x_data(CURRENT_CURVE, data);
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Adds data to the y values of the current curve in the current graph.
/// @param data The vector containing the data to add to the curve.
void argus_curve_add_y(Vector *data) {
	CHECK_INIT(init, argus_mutex)
	if (current_curve < 0) {
		fprintf(stderr, "[ARGUS]: warning: There is not any selected curve. The y data won't change.\n");	
		return;
	}
	curve_push_y_data(CURRENT_CURVE, data);
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Adds data to the x values of the current curve in the current graph.
/// @param data The raw buffer containing the data.
/// @param n The number of values to add.
/// @note n must be lower or equal to the length of data.
void argus_curve_add_x_raw(float *data, size_t n) {
	CHECK_INIT(init, argus_mutex)
	if (current_curve < 0) {
		fprintf(stderr, "[ARGUS]: warning: There is not any selected curve. The x data won't change.\n");	
		return;
	}
	curve_push_x_data_raw(CURRENT_CURVE, data, n);
	pthread_mutex_unlock(&argus_mutex);
}

/// @brief Adds data to the y values of the current curve in the current graph.
/// @param data The raw buffer containing the data.
/// @param n The number of values to add.
/// @note n must be lower or equal to the length of data.
void argus_curve_add_y_raw(float *data, size_t n) {
	CHECK_INIT(init, argus_mutex)
	if (current_curve < 0) {
		fprintf(stderr, "[ARGUS]: warning: There is not any selected curve. The y data won't change.\n");	
		return;
	}
	curve_push_y_data_raw(CURRENT_CURVE, data, n);
	pthread_mutex_unlock(&argus_mutex);
}




#include "texture.h"
#include "icons.h"
////////////////////////////////////////////////////////////////
//                    Rendering function                      //
////////////////////////////////////////////////////////////////

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
		fprintf(stderr, "[ARGUS]: error: Error during OpenGL context creation : %s.\n", SDL_GetError());
		goto ARGUS_ERROR_CONTEXT_CREATION;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	SDL_GL_SetSwapInterval(0);

	// GLEW initialization.
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "[ARGUS]: error: Failed to initialize GLEW.\n");
		goto ARGUS_ERROR_CONTEXT_CREATION;
	}

	// Initializes the FBO for the screenshots.
	if (!screenshot_fbo_create(1000,500)) {
		fprintf(stderr, "[ARGUS]: error: failed to initialize screenshot FBO!\n");
		goto ARGUS_ERROR_SCREENSHOT_INIT;
	}

	// Initializes the shaders.
	const char *name;
	const char *vert_source;
	const char *frag_source;
	const char **attr_names;
	int n;
	for (int i = 0; i < SHADERNAME_SIZE; ++i) {
		shader_get_sources((ShaderName)i, &name, &vert_source, &frag_source, &attr_names, &n);
		shaders[i] = shader_create(vert_source, frag_source, name, attr_names, n);
		if (!shaders[i]) {
			fprintf(stderr, "[ARGUS]: error: failed to initializes the shaders !\n");
			goto ARGUS_ERROR_SHADERS_CREATION;
		}
	}

	// Loads the glyphs set.
	Glyphs *glyphs = glyphs_create(64);
	if (!glyphs) {
		fprintf(stderr, "[ARGUS]: error: unable to load the glyphs set !\n");
		goto ARGUS_ERROR_GLYPHS_CREATION;
	}

	// Renders the window.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < lines*columns; ++i) {
		if (!graph_prepare_static(grid[i], glyphs, width, height) || 
			!graph_prepare_dynamic(grid[i], glyphs, width, height)) {
			fprintf(stderr, "[ARGUS]: error: Error during graph preparation !\n");
			goto ARGUS_ERROR_GRAPHS_PREPARATION;
		}
		graph_render(grid[i], glyphs);
	}
	SDL_GL_SwapWindow(window);

	// Window main loop.
	bool run = true;
	int x = 0, y = 0;
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
		bool left_released = false;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: 
				run = false; 
				break;
			case SDL_MOUSEBUTTONUP:
				left_released = event.button.button == SDL_BUTTON_LEFT;
				break;
			}
		}

		// Updates the save buttons.
		bool updated = false;
		SDL_GetMouseState(&x,&y);
		const float xf = (float)x/width;
		const float yf = (float)y/height;
		for (size_t i = 0; i < (size_t)lines*columns; ++i) {
			updated |= imagebutton_update(grid[i]->save, xf,yf, left_released);	
		}

		// If there is an update function, calculates the amount of  
		if (update_func) {
			updates_to_do += (time-last_loop)/(1000.0*dt);
			int loops_to_do = (int)updates_to_do;
			updates_to_do -= loops_to_do;
			for (int i = 0; i < loops_to_do; ++i) {
				update_func(update_args, dt);
			}
			updated = true;			
		}

		// Renders the window to update the shown data.
		if (updated) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (int i = 0; i < lines*columns; ++i) {
				if (!graph_prepare_dynamic(grid[i], glyphs, width, height)) {
					fprintf(stderr, "[ARGUS]: error: Error during graph preparation !\n");
					goto ARGUS_ERROR_GRAPHS_PREPARATION;
				}
				graph_render(grid[i], glyphs);
			}
			SDL_GL_SwapWindow(window);
		}
		last_loop = time;
	}

	// Frees in case of an error or at the end of the function.
ARGUS_ERROR_GRAPHS_PREPARATION:
	for (int i = 0; i < lines*columns; ++i) {
		graph_reset_graphics(grid[i]);
	}
	glyphs_free(&glyphs);
ARGUS_ERROR_GLYPHS_CREATION:
ARGUS_ERROR_SHADERS_CREATION:
	for (int i = 0; i < SHADERNAME_SIZE; ++i) shader_free(shaders+i);
	screenshot_fbo_free();
ARGUS_ERROR_SCREENSHOT_INIT:
	SDL_GL_DeleteContext(context);
	context = NULL;
ARGUS_ERROR_CONTEXT_CREATION:
	SDL_DestroyWindow(window);
	window = NULL;
	pthread_mutex_unlock(&argus_mutex);
}
