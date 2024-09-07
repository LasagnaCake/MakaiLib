#include <GL/gl3w.h>
#include <GL/gl.h>

#include "drawable.hpp"

using namespace Makai::Graph;

void Drawable::doRender() {if (active) draw();}

Drawable::Drawable(usize const& layer, bool const& manual) {
	if(!manual) setAuto(layer);
	manualMode = manual;
}

Drawable::~Drawable() {
	DEBUGLN("Removing from rendering layers...");
	if(!manualMode)
		Renderer::layers.removeFromAll(&render);
	DEBUGLN("Finalizing...\n");
}

Drawable& Drawable::setManual() {
	if(!manualMode)
		Renderer::layers.removeFromAll(&render);
	manualMode = true;
	return *this;
}

Drawable& Drawable::setAuto(usize const& renderLayer) {
	if(manualMode)
		Renderer::layers.addObject(&render, renderLayer);
	manualMode = false;
	return *this;
}

Drawable& Drawable::setRenderLayer(usize const& renderLayer) {
	Renderer::layers.removeFromAll(&render);
	Renderer::layers.addObject(&render, renderLayer);
	manualMode = false;
	return *this;
}

Drawable& Drawable::addToRenderLayer(usize const& renderLayer) {
	Renderer::layers.addObject(&render, renderLayer);
	manualMode = false;
	return *this;
}

Drawable& Drawable::removeFromRenderLayer(usize const& renderLayer) {
	Renderer::layers.removeFromGroup(&render, renderLayer);
	if (!Renderer::layers.getGroups(&render).size())
		manualMode = true;
	return *this;
}

DrawableObject::DrawableObject(usize const& layer, bool const& manual): Drawable(layer, manual) {
	DEBUGLN("Drawable object created!");
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
}

DrawableObject::~DrawableObject() {
	DEBUGLN("Deleting buffers...");
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void DrawableObject::display(
	Vertex* const&		vertices,
	usize const&		count,
	CullMode const&		culling,
	FillMode const&		fill,
	DisplayMode const&	mode,
	usize const&		instances
) {
	// Set blend mode
	useBlendMode();
	// Set point size, if applicable
	if (pointSize)	glPointSize(*pointSize);
	else			glEnable(GL_PROGRAM_POINT_SIZE);
	// Set line width
	glLineWidth(lineWidth);
	// Set VBO as active
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Copy IVB to VBO
	glBufferData(
		GL_ARRAY_BUFFER,
		count * sizeof(Vertex),
		vertices,
		GL_STATIC_DRAW
	);
	// Set VAO as active
	glBindVertexArray(vao);
	// Define vertex data in VBO
	Vertex::setAttributes();
	// Set VAO as active
	glBindVertexArray(vao);
	// Enable attribute pointers
	Vertex::enableAttributes();
	// Set face culling
	setCullMode(culling);
	// Set fill mode
	setFillMode(fill);
	// Draw object to screen
	glDrawArraysInstanced(getGLDisplayMode(mode), 0, count, instances);
	// Disable culling
	setCullMode(CullMode::OCM_NONE);
	// Disable attributes
	Vertex::disableAttributes();
	// Unbind vertex array
	glBindVertexArray(0);
	// Disable program point size
	glDisable(GL_PROGRAM_POINT_SIZE);
}

void DrawableObject::prepare() {
	// Render with object's shader
	shader();
	// Get transformation matrices
	Matrix4x4 cameraWorldActor(Global::camera.matrix() * Global::space * trans);
	// Set transformation matrices
	shader["vertMatrix"](Global::camera.projection() * cameraWorldActor);
	shader["normalsMatrix"](cameraWorldActor.transposed().inverted());
	Global::world.use(shader);
}
