#include "render.h"


/// @brief Renders a text VAO.
/// @param glyphs The glyphs to use to render text from.
/// @param shaders The main shaders array.
/// @param vao The text VAO.
/// @param color The color of the text
/// @note If vao == NULL, nothing will be drawn.
void render_text(Glyphs *glyphs, VAO *vao, Color color) {
	if (!vao) return;
	shader_use(shaders[SHADER_TEXT]);
		vao_bind(vao);
			glUniform3f(
				shader_uniform_location(shaders[SHADER_TEXT], "color"), 
				color.r, color.g, color.b
			);
			glyphs_bind(glyphs);
				glDrawArrays(GL_TRIANGLES, 0, vao->size);
			glyphs_bind(NULL);
		vao_bind(NULL);
	shader_use(NULL);
}

/// @brief Renders a shape from a VAO with a given transparency.
/// @param vao VAO of the shape to render.
/// @param transparency The transparency of the shape.
/// @note If vao == NULL, nothing will be drawn.
void render_shape(VAO *vao, float transparency) {
	if (!vao) return;
	shader_use(shaders[SHADER_SHAPE]);
		vao_bind(vao);
			glUniform1f(shader_uniform_location(shaders[SHADER_SHAPE], "transparency"), transparency);
			glDrawArrays(GL_TRIANGLES, 0, vao->size);
		vao_bind(NULL);
	shader_use(NULL);
}

/// @brief Renders a curve from a VAO with a given transparency.
/// @param vao VAO of the curve to render.
void render_curve(VAO *vao, Color color, bool continuous) {
	if (!vao) return;
	shader_use(shaders[SHADER_CURVE]);
		vao_bind(vao);
			glUniform3f(
				shader_uniform_location(shaders[SHADER_TEXT], "color"), 
				color.r, color.g, color.b
			);
			glDrawArrays(continuous ? GL_LINE_STRIP : GL_LINES, 0, vao->size);
		vao_bind(NULL);
	shader_use(NULL);
}


/// @brief Renders a texture from a VAO.
/// @param vao VAO of the texture to render.
/// @param texture The texture to use.
void render_texture(VAO *vao, Texture *texture) {
	printf("%p\n", texture);
	if (!vao) return;
	shader_use(shaders[SHADER_TEXTURE]);
		vao_bind(vao);
			texture_bind(texture);
			glDrawArrays(GL_TRIANGLES, 0, vao->size);
			texture_bind(NULL);
		vao_bind(NULL);
	shader_use(NULL);
}
