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
/// @param array_ids Lists of array ids for the attrib.
/// @param gl_types Lists of data types.
/// @param buffer_len Length of the data lists (number of vectors).
/// @param n Number of lists in data.
/// @return The created VAO.
VAO *vao_create(void** data, int* sizes, int* array_ids, int* gl_types, int buffer_len, int n) {

	// Malloc the VAO structure.
	VAO *vao = malloc(sizeof(VAO));
	if (!vao) {
		fprintf(stderr, "[ARGUS]: error: failed to malloc a VAO structure !\n");
		return NULL;
	}
	vao->size = buffer_len;
	
	// Creates the VBO.
	int type_sizes[n];
	for (int i = 0; i < n; i++) type_sizes[i] = sizeFromGLType(gl_types[i]);
	vao->vbo = vbo_create(data, sizes, type_sizes, buffer_len, n);
	if (!vao->vbo) {
		fprintf(stderr, "[ARGUS]: error: unable to create a VBO for a VAO !\n");
		free(vao);
		return NULL;
	}

	// Creates the VAO and links the VBO to it.
	long long int offset = 0;
	glGenVertexArrays(1, &vao->vao_id);
	vao_bind(vao);
		vbo_bind(vao->vbo);
			for (int i = 0; i < n; i++) {
				glVertexAttribPointer(array_ids[i], sizes[i], gl_types[i], GL_FALSE, 0, (void*)(offset));
				glEnableVertexAttribArray(array_ids[i]);
				offset += sizes[i] * type_sizes[i] * buffer_len;
			}
		vbo_bind(NULL);
	vao_bind(NULL);

	// Renvoie le VAO initialisé.
	return vao;

}

/// @brief Frees a VAO.
/// @param vao The VAO to free.
void vao_free(VAO *vao) {
	if(glIsVertexArray(vao->vao_id) == GL_TRUE) glDeleteVertexArrays(1, &vao->vao_id);
	if (vao->vbo) vbo_free(vao->vbo);
	free(vao);
}

/// @brief Binds a VAO.
/// @param vao The VAO to bind.
/// @note If vao == NULL, unbinds the currently used VAO.
void vao_bind(VAO *vao) {
	if (vao) glBindVertexArray(vao->vao_id);
	else glBindVertexArray(0);
}
