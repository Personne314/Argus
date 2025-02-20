#include "shader.h"

#include <stdio.h>
#include <stdlib.h>



// Shape shader data.
/// @brief Shape vertex shader source. 
static const char source_shape_shader_vert[] = 
"#version 450 core\n \
in vec2 in_coord; \
in vec4 in_color; \
out vec4 frag_color; \
void main() { \
	gl_Position = vec4(-1+2*in_coord.x, 1-2*in_coord.y, 0.0, 1.0); \
	frag_color = in_color; \
}";

/// @brief Shape fragment shader source.
static const char source_shape_shader_frag[] = 
"#version 450 core\n \
uniform float transparency; \
in vec4 frag_color; \
out vec4 out_color; \
void main() { \
	float w = frag_color.w * transparency; \
	out_color = vec4(frag_color.xyz, w); \
}";

/// @brief Attrib identifiers for shape shader.
static const int shape_attr_ids[] = {0,1};

/// @brief Attrib names for  shape shader.
static const char *shape_attr_names[] = {"in_coord","in_color"};


// Text shader data.
/// @brief Text vertex shader source. 
static const char source_text_shader_vert[] = 
"#version 450 core\n \
in vec2 in_coord; \
in vec2 in_tex_coord; \
out vec2 tex_coord; \
void main() { \
	gl_Position = vec4(-1+2*in_coord.x, 1-2*in_coord.y, 0.0, 1.0); \
	tex_coord = in_tex_coord; \
}";

/// @brief Text fragment shader source.
static const char source_text_shader_frag[] = 
"#version 450 core\n \
in vec2 tex_coord; \
out vec4 out_color; \
uniform sampler2D tex; \
uniform vec3 color; \
void main() { \
	out_color = vec4(color, 1.0)*texture(tex, tex_coord); \
}";

/// @brief Attrib identifiers for shape shader.
static const int text_attr_ids[] = {0,1};

/// @brief Attrib names for  shape shader.
static const char *text_attr_names[] = {"in_coord","in_tex_coord"};



// Shader infos.
/// @brief Shader description structure.
struct ShaderInfo {
    const char *name;	///< The shader name, used for error display. 
    const char *vert;	///< The vertex shader sources.
    const char *frag;	///< The fragment shader sources.
	const int *attr_ids;		///< The attrib array ids.
	const char **attr_names;	///< The attrib names in the sources.
	const int n;				///< The number of attrib lists.
};
typedef struct ShaderInfo ShaderInfo;

/// @brief Shader description array.
static const ShaderInfo shader_infos[] = {
    [SHADER_SHAPE] = {
		"shape", source_shape_shader_vert, source_shape_shader_frag,
		shape_attr_ids, shape_attr_names, 2
	},
	[SHADER_TEXT] = {
		"text", source_text_shader_vert, source_text_shader_frag,
		text_attr_ids, text_attr_names, 2
	}
};

/// @brief Returns the sources of a shader.
/// @param shader The constants that name the shader from which to get the sources.
/// @param name A variable to store a const pointer ot the shader name.
/// @param vert A variable to store a const pointer ot the vertex shader source.
/// @param frag A variable to store a const pointer ot the fragment shader source.
/// @return true if the name don't describe a known shader.
bool shader_get_sources(const ShaderName shader, const char **name, const char **vert,
const char **frag, const int **attr_ids, const char ***attr_names, int *n) {
    if (shader < 0 || shader >= SHADER_LIST_SIZE) {
		fprintf(stderr, 
			"[ARGUS]: error: '%d' doesn't correspond to a valid ShaderName value ! "
			"Unable to get the shader sources.\n", shader);
        return true;
    }
    *name = shader_infos[shader].name;
    *vert = shader_infos[shader].vert;
    *frag = shader_infos[shader].frag;
	*attr_ids = shader_infos[shader].attr_ids;
	*attr_names = shader_infos[shader].attr_names;
	*n = shader_infos[shader].n;
    return false;
}

// List of used shaders.
Shader *shaders[SHADER_LIST_SIZE];

/// @brief Compiles a source code into a shader.
/// @param shader The variable where to store the shader id.
/// @param type The type of the shader to compile.
/// @param source The shader source code.
/// @return true if there was an error.
static bool compile_shader(GLuint *shader, GLenum type, const char *source, const char *name) {

	// Creates the shader.
	GLuint i = glCreateShader(type);
	*shader = i;
	if (*shader == 0) {
		fprintf(stderr, 
			"[ARGUS]: error: shader '%s' creation failed, the type '%d' doesn't exists !\n", 
			name, type
		);
		goto SHADER_CREATION_ERROR;
	}

	// Compiles the shader.
	GLint state = 0;
	glShaderSource(*shader, 1, (const GLchar**)(&source), 0);
	glCompileShader(*shader);
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &state);
	if (state != GL_TRUE) {
		fprintf(stderr, "[ARGUS]: error: shader '%s' compilation failed !\n", name);

		// Gets compiling error messages.
		GLint error_size = 0;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &error_size);
		char *char_error = malloc((error_size+1)*sizeof(char));
		if (!char_error) {
			fprintf(stderr, "[ARGUS]: error: failed to malloc a buffer to get the error message !\n");
			goto SHADER_COMPILATION_FAILURE;
		}
		glGetShaderInfoLog(*shader, error_size, &error_size, char_error);
		char_error[error_size] = '\0';

		// Prints the error message.
		fprintf(stderr, "[ARGUS]: error: %s", char_error);
		free(char_error);
		goto SHADER_COMPILATION_FAILURE;

	}
	return false;

	// Error cases.
SHADER_COMPILATION_FAILURE:
	glDeleteShader(*shader);
SHADER_CREATION_ERROR:
	*shader = 0;
	return true;
}

/// @brief Creates a shader from source codes.
/// @param vert Source code for the vertex shader.
/// @param frag Source code for the fragment shader.
/// @param name Shader name used to print errors.
/// @param attr_ids Attrib ids.
/// @param attr_names Attrib names.
/// @param n Number of elements in the attrib lists.
/// @return The created shader.
Shader *shader_create(const char *vert, const char *frag, const char *name, 
const int *attr_ids, const char *attr_names[], const int n) {

	// Malloc for the shader structure.
	Shader *shader = malloc(sizeof(Shader));
	if (!shader) {
		fprintf(stderr, "[ARGUS]: error: failed to malloc the shader '%s' !\n", name);
		goto SHADER_MALLOC_FAILURE;
	}

	// Vertex shader compilation.
	if (compile_shader(&shader->vert_id, GL_VERTEX_SHADER, vert, name)) {
		fprintf(stderr, "[ARGUS]: error: failed to compile the vertex shader of shader '%s' !\n", name);
		goto SHADER_VERTEX_COMPILATION_FAILURE;
	}

	// Fragment shader compilation.
	if (compile_shader(&shader->frag_id, GL_FRAGMENT_SHADER, frag, name)) {
		fprintf(stderr, "[ARGUS]: error: failed to compile the fragment shader of shader '%s' !\n", name);
		goto SHADER_FRAGMENT_COMPILATION_FAILURE;
	}

	// Creates the program.
	shader->prog_id = glCreateProgram();
	if (!shader->prog_id) {
		fprintf(stderr, "[ARGUS]: error: failed to create the program for shader '%s' !\n", name);
		goto SHADER_PROGRAM_CREATION_FAILURE;
	}
	glAttachShader(shader->prog_id, shader->vert_id);
	glAttachShader(shader->prog_id, shader->frag_id);

	// Attribes the lists and links the program.
	GLint state = 0;
	for (int i = 0; i < n; ++i) {
		glBindAttribLocation(shader->prog_id, attr_ids[i], attr_names[i]);
	}
	glLinkProgram(shader->prog_id);
	glGetProgramiv(shader->prog_id, GL_LINK_STATUS, &state);
	if(state != GL_TRUE) {
		fprintf(stderr, "[ARGUS]: error: shader '%s' linking failed !\n", name);

		// Gets linking error messages.
		GLint error_size = 0;
		glGetProgramiv(shader->prog_id, GL_INFO_LOG_LENGTH, &error_size);
		char *char_error = malloc((error_size+1)*sizeof(char));
		if (!char_error) {
			fprintf(stderr, "[ARGUS]: error: failed to malloc a buffer to get the error message !\n");
			goto SHADER_LINK_ERROR;
		}
		glGetShaderInfoLog(shader->prog_id, error_size, &error_size, char_error);
		char_error[error_size] = '\0';

		// Prints the error message.
		fprintf(stderr, "[ARGUS]: error: %s", char_error);
		free(char_error);
		goto SHADER_LINK_ERROR;
	}
	return shader;

	// Error cases.
SHADER_LINK_ERROR:
	glDeleteProgram(shader->prog_id);
SHADER_PROGRAM_CREATION_FAILURE:
	glDeleteShader(shader->frag_id);
SHADER_FRAGMENT_COMPILATION_FAILURE:
	glDeleteShader(shader->vert_id);
SHADER_VERTEX_COMPILATION_FAILURE:
	free(shader);
SHADER_MALLOC_FAILURE:
	return NULL;
}

/// @brief Frees a shader.
/// @param shader The shader to free.
void shader_free(Shader *shader) {
	glDeleteShader(shader->vert_id);
	glDeleteShader(shader->frag_id);
	glDeleteProgram(shader->prog_id);
	free(shader);
}

/// @brief Binds a shader.
/// @param shader The shader to bind.
/// @note If shader == NULL, unbinds the currently used shader.
void shader_use(Shader *shader) {
	if (shader) glUseProgram(shader->prog_id);
	else glUseProgram(0);
}

/// @brief Do a call to glGetUniformLocation.
/// @param shader The shader from where to get the uniform location.
/// @param str The uniform name from who to get the uniform location
/// @return The location of the uniform variable.
int shader_uniform_location(Shader *shader, const char *str) {
	int uniform_location = glGetUniformLocation(shader->prog_id, str);
	if (uniform_location == -1) {
		fprintf(stderr, "[ARGUS]: error: '%s' uniform variable couldn't be found in the shader !"
						" The rendering process might produce unxepected results.", str);
	}
	return uniform_location;
}
