#include "vbo.h"

#include <stdlib.h>
#include <stdio.h>



/// @brief Constructs a VBO using the given parameters.
/// @param data Arrays of vectors of data. 
/// @param sizes Lists of data vectors sizes.
/// @param type_sizes Lists of data types sizes.
/// @param buffer_len Length of the data lists (number of vectors).
/// @param n Number of lists in data.
/// @return The created VBO.
VBO *vbo_create(void** data, int* sizes, int* type_sizes, int buffer_len, int n) {

	// Malloc the VBO structure.
	VBO *vbo = malloc(sizeof(VBO));
	if (!vbo) {
		fprintf(stderr, "[ARGUS]: error: failed to malloc a VBO structure !\n");
		return NULL;
	}
	glGenBuffers(1, &vbo->vbo_id);
	vbo->size = buffer_len;

	// Calculate the size used by each list and the total size.
	GLsizeiptr data_sizes[n+1];
	data_sizes[n] = 0;
	for (int i = 0; i < n; i++) {
		GLsizeiptr val = sizes[i] * type_sizes[i] * buffer_len;
		data_sizes[n] += val;
		data_sizes[i] = val;
	}

	// Bind the VBO, stores the data, then unbind it.
	vbo_bind(vbo);
		GLsizeiptr offset = 0;
		glBufferData(GL_ARRAY_BUFFER, data_sizes[n], 0, GL_STATIC_DRAW);
		for(int i = 0; i < n; i++) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, data_sizes[i], data[i]);
			offset += data_sizes[i];
		}
	vbo_bind(NULL);
	return vbo;
}

/// @brief Frees a VBO.
/// @param vbo The VBO to free.
void vbo_free(VBO *vbo) {
	if(glIsBuffer(vbo->vbo_id) == GL_TRUE) glDeleteBuffers(1, &vbo->vbo_id);
	free(vbo);
}

/// @brief Binds a VBO.
/// @param vbo The VBO to bind.
/// @note If vbo == NULL, unbinds the currently used VBO.
void vbo_bind(VBO *vbo) {
	if (vbo) glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);
	else glBindBuffer(GL_ARRAY_BUFFER, 0);
}
