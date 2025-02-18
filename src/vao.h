#pragma once

#include <GL/glew.h>
#include "vbo.h"


// struct VAO : Permet de gérer un Vertex Buffer Array OpenGL.
struct VAO {
	GLuint vao_id;	///< OpenGL VBO id.
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
