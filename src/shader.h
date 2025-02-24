#pragma once

#include <GL/glew.h>
#include <stdbool.h>


/// @enum ShaderName
/// @brief List of constants that represents each shader.
enum ShaderName {
	SHADER_SHAPE,
	SHADER_TEXT,
	SHADER_CURVE,
	SHADER_LIST_SIZE
};
typedef enum ShaderName ShaderName;

/// @struct Shader
/// @brief Used to store an OpenGL shader.
struct Shader {
	GLuint vert_id;	///< Vertex shader id.
	GLuint frag_id;	///< Fragment shader id.
	GLuint prog_id;	///< Shader program id.
};
typedef struct Shader Shader;

// List of used shaders.
extern Shader *shaders[SHADER_LIST_SIZE];

// Returns the sources of a shader.
bool shader_get_sources(const ShaderName shader, const char **name, const char **vert, 
	const char **frag, const int **attr_ids, const char ***attr_names, int *n);

// Creates a shader from source codes.
Shader *shader_create(const char *vert, const char *frag, const char *name, 
	const int *attr_ids, const char *attr_names[], const int n);

// Creates a shader from source codes.
void shader_free(Shader *shader);

// Frees a shader.
void shader_use(Shader *shader);

// Binds a shader.
int shader_uniform_location(Shader *shader, const char *str);
