#ifndef DRAWING_GL_CORE_PRIMITIVE_HPP
#define DRAWING_GL_CORE_PRIMITIVE_HPP
#include <array>
#include <memory>
#include "../gl_include.hpp"
#include "../primitive.hpp"
#include "../types.hpp"
#include "../gl_shared/common.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLShared
{
class Program;
class Texture;
} // GLShared

namespace GLCore
{

class Primitive : public IPrimitive
{
public:
	Primitive(const GLShared::Program& program,
	          const GLShared::Program& texProgram);
	virtual ~Primitive();
	
	void SetDrawMode(const PrimitiveDrawMode& pdm) override;
	void SetVertices(const Vertices& vertices) override;
	void SetColors(const Colors& colors) override;
	void SetIndices(const Indices& indices) override;
	
	void SetMatrix(const glm::mat4& matrix) override;
	void SetBrightness(const float val) override;
	
	void SetTexCoords(const TexCoords& texCoords) override;
	void SetTexture(const Texture& texture) override;
	
	void Draw() override;
private:
	const GLShared::Program& program;
	const GLShared::Program& texProgram;
	bool drawByIndex{false};
	GLuint vao{};
	std::array<GLuint, GLShared::ATTR_COUNT> vbo{};
	GLenum mode{GL_TRIANGLES};
	GLsizei drawCount{0};
	glm::mat4 mat{1.0f};
	float brightness{1.0f};
	std::shared_ptr<GLShared::Texture> tex;
};

} // GLCore

} // Detail

} // Drawing

#endif // DRAWING_GL_CORE_PRIMITIVE_HPP
