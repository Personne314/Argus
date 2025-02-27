#pragma once

#include <GL/glew.h>
#include "vbo.h"


/// @struct VAO
/// @brief Used to manage an OpenGL VAO.
struct VAO {
	GLuint vao_id;	///< OpenGL VAO id.
	VBO *vbo;		///< VBO used in the VAO.
	int size;		///< Number of vectors in the VAO.
};
typedef struct VAO VAO;


// Constructs a VAO using the given parameters.
VAO *vao_create(void** data, int* sizes, int* array_ids, int* gl_types, int buffer_len, int n);

// Frees a VAO.
void vao_free(VAO *vao);

// Binds a VAO.
void vao_bind(VAO *vao);
