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


/// @brief Constructs an InstancedVAO using the given parameters.
/// @param shared_data Lists of shared data vectors sizes.
/// @param shared_sizes Lists of shared data vectors sizes.
/// @param shared_gl_types Lists of shared data types.
/// @param shared_buffer_len Length of the shared data lists (number of vectors).
/// @param shared_n Number of lists in shared_data.
/// @param instance_data Lists of instance data vectors sizes.
/// @param instance_sizes Lists of instance data vectors sizes.
/// @param instance_gl_types Lists of instance data types.
/// @param instance_buffer_len Length of the instance data lists (number of instances).
/// @param instance_n Number of lists in instance_data.
/// @note shared_n is the length of shared_data, shared_sizes, shared_gl_types and shared_n*shared_sizes[i] is the len of shared_data[i].
/// @note instance_n is the length of instance_data, instance_sizes, instance_gl_types and instance_n*instance_sizes[i] is the len of instance_data[i].
/// @return The created InstancedVAO.
InstancedVAO *vao_create_instanced(
	void** shared_data, int* shared_sizes, int* shared_gl_types, size_t shared_buffer_len, int shared_n,
	void** instance_data, int* instance_sizes, int* instance_gl_types, size_t instance_buffer_len, int instance_n
) {

	// Malloc the VAO structure.
	InstancedVAO *vao = malloc(sizeof(VAO));
	if (!vao) {
		fprintf(stderr, "[ARGUS]: error: failed to malloc a VAO structure !\n");
		return NULL;
	}
	vao->size = instance_buffer_len;
	
	// Creates the shared VBO.
	int shared_type_sizes[shared_n];
	for (int i = 0; i < shared_n; ++i) shared_type_sizes[i] = sizeFromGLType(shared_gl_types[i]);
	vao->vbo_shared = vbo_create(shared_data, shared_sizes, shared_type_sizes, shared_buffer_len, shared_n);
	if (!vao->vbo_shared) {
		fprintf(stderr, "[ARGUS]: error: unable to create a shader VBO for an InstanceVAO !\n");
		free(vao);
		return NULL;
	}

	// Creates the instance VBO.
	int instance_type_sizes[instance_n];
	for (int i = 0; i < instance_n; ++i) instance_type_sizes[i] = sizeFromGLType(instance_gl_types[i]);
	vao->vbo_instanced = vbo_create(instance_data, instance_sizes, instance_type_sizes, instance_buffer_len, instance_n);
	if (!vao->vbo_instanced) {
		fprintf(stderr, "[ARGUS]: error: unable to create an instance VBO for an InstanceVAO !\n");
		vbo_free(&vao->vbo_shared);
		free(vao);
		return NULL;
	}

	// Creates the VAO and links the VBO to it.
	size_t offset = 0;
	glGenVertexArrays(1, &vao->vao_id);
	vao_bind(vao);

		// Links the shader data.
		vbo_bind(vao->vbo_shared);
			for (int i = 0; i < shared_n; ++i) {
				glVertexAttribPointer(i, shared_sizes[i], shared_gl_types[i], GL_FALSE, 0, (void*)(offset));
				glEnableVertexAttribArray(i);
				offset += shared_sizes[i] * shared_type_sizes[i] * shared_buffer_len;
			}
		
		// Links the instance data.
		vbo_bind(vao->vbo_instanced);
			offset = 0;
			for (int i = 0; i < instance_n; ++i) {
				glVertexAttribPointer(i+shared_n, instance_sizes[i], instance_gl_types[i], GL_FALSE, 0, (void*)(offset));
				glEnableVertexAttribArray(i);
				glVertexAttribDivisor(i+shared_n, 1);
				offset += instance_sizes[i] * instance_type_sizes[i] * instance_buffer_len;
			}
		vbo_bind(NULL);
	vao_bind(NULL);
	return vao;
}

/// @brief Frees the memory allocated for a InstancedVAO.
/// @param p_vao A pointer to the pointer of the InstancedVAO to be freed. Cannot be NULL.
/// @note After freeing, the pointer *p_vao is set to NULL to avoid double-free.
void vao_free_instanced(InstancedVAO **p_vao) {
	InstancedVAO *vao = *p_vao;
	if (!vao) return;
	if(glIsVertexArray(vao->vao_id) == GL_TRUE) {
		glDeleteVertexArrays(1, &vao->vao_id);
	}
	vbo_free(&vao->vbo_instanced);
	vbo_free(&vao->vbo_shared);
	free(vao);
	*p_vao = NULL;
}

/// @brief Binds an InstancedVAO.
/// @param vao The InstancedVAO to bind.
/// @note If vao == NULL, unbinds the currently used InstancedVAO.
void vao_bind_instanced(InstancedVAO *vao) {
	if (vao) glBindVertexArray(vao->vao_id);
	else glBindVertexArray(0);
}
