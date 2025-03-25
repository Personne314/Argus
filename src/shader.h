#pragma once

#include <GL/glew.h>
#include <stdbool.h>


/// @enum ShaderName
/// @brief List of constants that represents each shader.
typedef enum {
	SHADER_SHAPE,
	SHADER_TEXT,
	SHADER_CURVE,
	SHADER_TEXTURE,
	SHADERNAME_SIZE
} ShaderName;

/// @struct Shader
/// @brief Used to store an OpenGL shader.
typedef struct {
	GLuint vert_id;	///< Vertex shader id.
	GLuint frag_id;	///< Fragment shader id.
	GLuint prog_id;	///< Shader program id.
} Shader;

// List of used shaders.
extern Shader *shaders[SHADERNAME_SIZE];

// Returns the sources of a shader.
bool shader_get_sources(const ShaderName shader, const char **name, const char **vert, 
	const char **frag, const char ***attr_names, int *n);

// Creates a shader from source codes.
Shader *shader_create(const char *vert, const char *frag, const char *name, 
	const char *attr_names[], const int n);

// Frees the memory allocated for a Shader.
void shader_free(Shader **p_shader);

// Binds a shader.
void shader_use(Shader *shader);

// Do a call to glGetUniformLocation.
int shader_uniform_location(Shader *shader, const char *str);
