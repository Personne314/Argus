#include "vao.h"

#include <stdio.h>
#include <stdlib.h>



// Returns the size of a type corresponding to an OpenGL constant.
int sizeFromGLType(int type) {
	switch (type) {
	case GL_FLOAT:	return sizeof(float);
	case GL_DOUBLE:	return sizeof(double);
	case GL_INT:	return sizeof(int);
	case GL_SHORT:	return sizeof(short);
	default:		return 1;
	}
}

/// @brief Constructs a VAO using the given parameters.
/// @param data Arrays of vectors of data.
/// @param sizes Lists of data vectors sizes.
/// @param gl_types Lists of data types.
/// @param buffer_len Length of the data lists (number of vectors).
/// @param n Number of lists in data.
/// @note n is the length of data, sizes, gl_types and n*sizes[i] is the len of data[i]. 
/// @return The created VAO.
VAO *vao_create(void** data, int* sizes, int* gl_types, size_t buffer_len, int n) {

	// Malloc the VAO structure.
	VAO *vao = malloc(sizeof(VAO));
	if (!vao) {
		fprintf(stderr, "[ARGUS]: error: failed to malloc a VAO structure !\n");
		return NULL;
	}
	vao->size = buffer_len;
	
	// Creates the VBO.
	int type_sizes[n];
	for (int i = 0; i < n; ++i) type_sizes[i] = sizeFromGLType(gl_types[i]);
	vao->vbo = vbo_create(data, sizes, type_sizes, buffer_len, n);
	if (!vao->vbo) {
		fprintf(stderr, "[ARGUS]: error: unable to create a VBO for a VAO !\n");
		free(vao);
		return NULL;
	}

	// Creates the VAO and links the VBO to it.
	size_t offset = 0;
	glGenVertexArrays(1, &vao->vao_id);
	vao_bind(vao);
		vbo_bind(vao->vbo);
			for (int i = 0; i < n; ++i) {
				glVertexAttribPointer(i, sizes[i], gl_types[i], GL_FALSE, 0, (void*)(offset));
				glEnableVertexAttribArray(i);
				offset += sizes[i] * type_sizes[i] * buffer_len;
			}
		vbo_bind(NULL);
	vao_bind(NULL);

	// Renvoie le VAO initialisé.
	return vao;

}

/// @brief Frees the memory allocated for a VAO.
/// @param p_vao A pointer to the pointer of the VAO to be freed. Cannot be NULL.
/// @note After freeing, the pointer *p_vao is set to NULL to avoid double-free.
void vao_free(VAO **p_vao) {
	VAO *vao = *p_vao;
	if (!vao) return;
	if(glIsVertexArray(vao->vao_id) == GL_TRUE) {
		glDeleteVertexArrays(1, &vao->vao_id);
	}
	vbo_free(&vao->vbo);
	free(vao);
	*p_vao = NULL;
}

/// @brief Binds a VAO.
/// @param vao The VAO to bind.
/// @note If vao == NULL, unbinds the currently used VAO.
void vao_bind(VAO *vao) {
	if (vao) glBindVertexArray(vao->vao_id);
	else glBindVertexArray(0);
}
