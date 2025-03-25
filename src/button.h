#pragma once

#include "vao.h"
#include "structs.h"
#include "texture.h"


typedef struct {
	void (*callback)(void*);	
	void *args;				
	Rect rect;
} Button;

typedef struct {
	VAO *image_vao;
	Texture *image;
} ImageButton;
