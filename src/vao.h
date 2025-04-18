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

/// @struct InstancedVAO
/// @brief Used to manage an OpenGL VAO with instanciation.
typedef struct {
	VBO *vbo_shared;	///< The VBO for data shared between instances.
	VBO *vbo_instanced;	///< The VBO for instance data.
	size_t size;		///< Number of instances.
	GLuint vao_id;		///< OpenGL VAO id.
} InstancedVAO;



// Constructs a VAO using the given parameters.
VAO *vao_create(void** data, int* sizes, int* gl_types, size_t buffer_len, int n);

// Frees the memory allocated for a VAO.
void vao_free(VAO **p_vao);

// Binds a VAO.
void vao_bind(VAO *vao);



// Constructs an InstancedVAO using the given parameters.
InstancedVAO *vao_create_instanced(
	void** shared_data, int* shared_sizes, int* shared_gl_types, size_t shared_buffer_len, int shared_n,
	void** instance_data, int* instance_sizes, int* instance_gl_types, size_t instance_buffer_len, int instance_n
);

// Frees the memory allocated for a InstancedVAO.
void vao_free_instanced(InstancedVAO **p_vao);

// Binds an InstancedVAO.
void vao_bind_instanced(InstancedVAO *vao);
