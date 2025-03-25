#pragma once

#include <stdbool.h>
#include "vao.h"
#include "structs.h"
#include "texture.h"


/// @struct Button
/// @brief Used to create a clickable button.
typedef struct {
	void (*callback)(void*);	//< The function to call on a click.
	void *args;	//<	Callback function arguments.
	Rect rect;	//< The button clickable area.
} Button;

/// @struct ImageButton
/// @brief Used to create a clickable image.
typedef struct {
	Button button;	//< The button of the image.
	VAO *image_vao;	//< The VAO of the image.
	Texture *image;	//< The image to render.
} ImageButton;


// Update function of a button. Calls the callback function if the button is clicked.
void button_update(Button *button, float x, float y, bool clicked);


// Creates an ImageButton with a callback function.
ImageButton *imagebutton_create(const unsigned char *image, size_t size, 
	void (*callback)(void*), void *args);

// Frees the memory allocated for a ImageButton.
void imagebutton_free(ImageButton **p_button);


// Prepares the VAO of the button.
bool imagebutton_prepare_static(ImageButton *button, Rect *rect);


// Update function of an ImageButton. Calls the callback function if the button is clicked.
void imagebutton_update(ImageButton *button, float x, float y, bool clicked);

// Renders an ImageButton.
void imagebutton_render(ImageButton *button);
