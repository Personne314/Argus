#include "button.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include "render.h"



/// @brief Update function of a button. Calls the callback function if the button is clicked.
/// @param button The button to update.
/// @param x The x coordinate of the mouse cursor.
/// @param y The y coordinate of the mouse cursor.
/// @param clicked true if the left click was used.
void button_update(Button *button, float x, float y, bool clicked) {
	if (!clicked) return;
	Rect rect = button->rect;
	if (x >= rect.x && x <= rect.x+rect.w && y >= rect.y && y >= rect.y+rect.h) {
		button->callback(button->args);
	}
}


/// @brief Creates an ImageButton with a callback function.
/// @param path The path to the image to use.
/// @param callback The function to call in case of a click.
/// @param args The arguments of the callback function. This will be free by imagebutton_free.
/// @return The created button.
/// @note In case of an error, args won't be free. 
ImageButton *imagebutton_create(const unsigned char *image, size_t size, 
void (*callback)(void*), void *args) {

	// Malloc the button.
	ImageButton *button = malloc(sizeof(ImageButton));
	if (!button) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a ImageButton\n");
		return NULL;
	}
	button->button.callback = callback;
	button->button.args = args;
	button->button.rect = (Rect){0,0,0,0};
	button->image_vao = NULL;

	// Creates an RWops for IMG_Load_RW.
	SDL_RWops *rw = SDL_RWFromMem((void*)image, size);
	if (!rw) {
		fprintf(stderr, "[ARGUS]: error: unable to create an RWops during "
			"ImageButton creation: %s\n", SDL_GetError());
		free(button);
		return NULL;
	}

	// Creates the image surface.
	SDL_Surface *surface = IMG_Load_RW(rw, 1);
	if (!surface) {
		fprintf(stderr, "[ARGUS]: error: unable to create the image surface "
			"during ImageButton creation: %s\n", SDL_GetError());
		vao_free(&button->image_vao);
		free(button);
		return NULL;
	}

	surface = IMG_Load("./save.png");

	// Loads the Texture.
	button->image = create_texture("./save.png");
	texture_save(button->image, "DUMP.PNG");
	if (!button->image) {
		fprintf(stderr, "[ARGUS]: error: unable to load the image of an ImageButton\n");
		vao_free(&button->image_vao);
		free(button);
		return NULL;
	}
	printf(":%p\n", button->image);
	return button;
}


/// @brief Frees the memory allocated for a ImageButton.
/// @param p_button A pointer to the pointer of the ImageButton to be freed. Cannot be NULL.
/// @note After freeing, the pointer *p_button is set to NULL to avoid double-free.
void imagebutton_free(ImageButton **p_button) {
	ImageButton *button = *p_button;
	if (!button) return;
	vao_free(&button->image_vao);
	texture_free(&button->image);
	free(button->button.args);
	free(button);
	*p_button = NULL;
}


/// @brief Prepares the VAO of the button.
/// @param button The button to prepare.
/// @param rect The rect of the button.
bool imagebutton_prepare_static(ImageButton *button, Rect *rect) {
	if (button->image_vao) vao_free(&button->image_vao);
	button->image_vao = NULL;
	button->button.rect = *rect;
	float vertices[12] = {
		rect->x,rect->y,rect->x+rect->w,rect->y+rect->h,rect->x,rect->y+rect->h,
		rect->x,rect->y,rect->x+rect->w,rect->y,rect->x+rect->w,rect->y+rect->h
	};
	float textures[12] = {0,1,1,0,0,0,0,1,1,1,1,0};
	void *data[2] = {vertices, textures};
	int sizes[2] = {2,2};
	int gl_types[2] = {GL_FLOAT, GL_FLOAT};
	button->image_vao = vao_create(data, sizes, gl_types, 6,2);
	if (!button->image_vao) {
		fprintf(stderr, "[ARGUS]: error: unable to create the VAO of an ImageButton\n");
		free(button);
		return false;
	}
	return true;
}


/// @brief Update function of an ImageButton. Calls the callback function if the button is clicked.
/// @param button The button to update.
/// @param x The x coordinate of the mouse cursor.
/// @param y The y coordinate of the mouse cursor.
/// @param clicked true if the left click was used.
void imagebutton_update(ImageButton *button, float x, float y, bool clicked) {
	button_update(&button->button, x,y, clicked);
}


/// @brief Renders an ImageButton.
/// @param button The button to render.
void imagebutton_render(ImageButton *button) {
	render_texture(button->image_vao, button->image);
}
