#pragma once

#include <stddef.h>
#include <GL/glew.h>
#include "vbo.h"


/// @struct VAO
/// @brief Used to manage an OpenGL VAO.
typedef struct {
	VBO *vbo;		///< VBO used in the VAO.
	size_t size;	///< Number of vectors in the VAO.
	GLuint vao_id;	///< OpenGL VAO id.
} VAO;


// Constructs a VAO using the given parameters.
VAO *vao_create(void** data, int* sizes, int* gl_types, size_t buffer_len, size_t n);

// Frees the memory allocated for a VAO.
void vao_free(VAO **p_vao);

// Binds a VAO.
void vao_bind(VAO *vao);
