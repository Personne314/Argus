#pragma once

#include <GL/glew.h>


/// @struct VBO
/// @brief Used to create and use a VBO.
struct VBO {
	GLuint vbo_id;	///< OpenGL VBO id.
	int size;		///< Number of vectors in the VBO.
};
typedef struct VBO VBO;


// Constructs a VBO using the given parameters.
VBO *vbo_create(void** data, int* sizes, int* type_sizes, int buffer_len, int n);

// Frees a VBO.
void vbo_free(VBO *vbo);

// Binds a VBO.
void vbo_bind(VBO *vbo);
