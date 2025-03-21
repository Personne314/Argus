#pragma once

#include <stddef.h>
#include <GL/glew.h>


/// @struct VBO
/// @brief Used to manage an OpenGL VBO.
typedef struct {
	size_t size;	///< Number of vectors in the VBO.
	GLuint vbo_id;	///< OpenGL VBO id.
} VBO;


// Constructs a VBO using the given parameters.
VBO *vbo_create(void** data, int* sizes, int* type_sizes, size_t buffer_len, int n);

// Frees the memory allocated for a VBO.
void vbo_free(VBO **p_vbo);

// Binds a VBO.
void vbo_bind(VBO *vbo);
