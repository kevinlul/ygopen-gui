#include "button.hpp"

#include <glm/gtc/matrix_transform.hpp> // glm::translate

#include "../drawing/api.hpp"
#include "../drawing/primitive.hpp"

namespace YGOpen
{

namespace GUI
{

// Colors for the button
static const Drawing::Color CONTENT_TOP = {0.220f, 0.247f, 0.278f, 1.0f};
static const Drawing::Color CONTENT_BOTTOM = {0.298f, 0.341f, 0.373f, 1.0f};
static const Drawing::Color LINE_LEFT = {0.286f, 0.286f, 0.286f, 1.0f};
static const Drawing::Color LINE_TOP = {0.333f, 0.333f, 0.333f, 1.0f};
static const Drawing::Color LINE_BOTTOM = {0.200f, 0.200f, 0.200f, 1.0f};
static const Drawing::Color LINE_RIGHT = {0.251f, 0.251f, 0.251f, 1.0f};

// Primitive data
// NOTE: to prevent costly copies, vertices are static but not constant
// and are changed by the button Resize method, as a result these variables
// are not thread-safe, however, i dont expect to have the GUI be
// multithreaded, so in practice thats not an issue.

// Button's shadow
constexpr auto SHADOW_DRAW_MODE = Drawing::PDM_TRIANGLE_STRIP;
constexpr float SHADOW_SIZE = 4.0f;
static Drawing::Vertices shadowVertices =
{
	// Outermost corners
	{ -SHADOW_SIZE, -SHADOW_SIZE, 0.0f},
	{ 1.0f + SHADOW_SIZE, -SHADOW_SIZE, 0.0f},
	{ -SHADOW_SIZE, 1.0f + SHADOW_SIZE, 0.0f},
	{ 1.0f + SHADOW_SIZE, 1.0f + SHADOW_SIZE, 0.0f},
	// Innermost corners
	{ 0.0f, 0.0f, 0.0f},
	{ 1.0f, 0.0f, 0.0f},
	{ 0.0f, 1.0f, 0.0f},
	{ 1.0f, 1.0f, 0.0f},
};
static const Drawing::Colors SHADOW_COLORS =
{
	// Outermost corners
	{0.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	// Innermost corners
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
};
static const Drawing::Indices SHADOW_INDICES =
{
	0, 4, 1, 5, 3, 7, 2, 6, 0, 4,
};

// Button's content
constexpr auto CONTENT_DRAW_MODE = Drawing::PDM_TRIANGLE_STRIP;
static Drawing::Vertices contentVertices =
{
	// Corners
	{ 0.0f, 0.0f, 0.0f},
	{ 1.0f, 0.0f, 0.0f},
	{ 0.0f, 1.0f, 0.0f},
	{ 1.0f, 1.0f, 0.0f},
};
static const Drawing::Colors CONTENT_COLORS =
{
	CONTENT_TOP, CONTENT_TOP,
	CONTENT_BOTTOM, CONTENT_BOTTOM,
};

// Button's lines
constexpr auto LINES_DRAW_MODE = Drawing::PDM_LINES;
static Drawing::Vertices linesVertices =
{
	// Left line
	{ 0.0f, 0.0f, 0.0f},
	{ 0.0f, 1.0f, 0.0f},
	// Top Line
	{ 0.0f, 0.0f, 0.0f},
	{ 1.0f, 0.0f, 0.0f},
	// Bottom Line
	{ 0.0f, 1.0f, 0.0f},
	{ 1.0f, 1.0f, 0.0f},
	// Right Line
	{ 1.0f, 0.0f, 0.0f},
	{ 1.0f, 1.0f, 0.0f},
};
static const Drawing::Colors LINES_COLORS =
{
	LINE_LEFT, LINE_LEFT,
	LINE_TOP, LINE_TOP,
	LINE_BOTTOM, LINE_BOTTOM,
	LINE_RIGHT, LINE_RIGHT,
};

Button::Button()
{
	shadow = Drawing::API::NewPrimitive();
	content = Drawing::API::NewPrimitive();
	lines = Drawing::API::NewPrimitive();
	
	// Set up constant data
	shadow->SetDrawMode(SHADOW_DRAW_MODE);
	shadow->SetColors(SHADOW_COLORS);
	
	content->SetDrawMode(CONTENT_DRAW_MODE);
	content->SetColors(CONTENT_COLORS);
	
	lines->SetDrawMode(LINES_DRAW_MODE);
	lines->SetColors(LINES_COLORS);
}

void Button::Resize(const Drawing::Matrix& mat, const SDL_Rect& rect)
{
	const float w = static_cast<float>(rect.w);
	const float h = static_cast<float>(rect.h);
	
	// Update shadow vertices
	shadowVertices[1].x = (shadowVertices[5].x = w) + SHADOW_SIZE;
	shadowVertices[2].y = (shadowVertices[6].y = h) + SHADOW_SIZE;
	shadowVertices[3].x = (shadowVertices[7].x = w) + SHADOW_SIZE;
	shadowVertices[3].y = (shadowVertices[7].y = h) + SHADOW_SIZE;	
	
	// Update content vertices
	contentVertices[1].x = contentVertices[3].x = w;
	contentVertices[2].y = contentVertices[3].y = h;
	
	// Update lines vertices
	linesVertices[1].y = linesVertices[4].y = linesVertices[5].y =
	linesVertices[7].y = h;
	linesVertices[3].x = linesVertices[5].x = linesVertices[6].x =
	linesVertices[7].x = w;
	
	// Set up vertices in primitives
	shadow->SetVertices(shadowVertices);
	shadow->SetIndices(SHADOW_INDICES);
	content->SetVertices(contentVertices);
	lines->SetVertices(linesVertices);
	
	// Move to right position
	auto projModel = mat * glm::translate(glm::mat4(1.0f),
	                                      glm::vec3(rect.x, rect.y, 0.0f));
	
	// Set up matrices for use with the shader
	shadow->SetMatrix(projModel);
	content->SetMatrix(projModel);
	lines->SetMatrix(projModel);
}

void Button::Draw()
{
	shadow->Draw();
	content->Draw();
	lines->Draw();
}

} // GUI

} // YGOpen