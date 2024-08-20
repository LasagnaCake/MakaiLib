#include "drawable.hpp"

#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

using namespace Makai::Graph;

void Drawable::doRender() {if (active) draw();}

uint convert(ObjectDisplayMode const& mode) {
	switch (mode) {
		case ObjectDisplayMode::ODM_POINTS:						return GL_POINTS;
		case ObjectDisplayMode::ODM_LINE_STRIP:					return GL_LINE_STRIP;
		case ObjectDisplayMode::ODM_LINE_LOOP:					return GL_LINE_LOOP;
		case ObjectDisplayMode::ODM_LINES:						return GL_LINES;
		case ObjectDisplayMode::ODM_TRIANGLE_STRIP:				return GL_TRIANGLE_STRIP;
		case ObjectDisplayMode::ODM_TRIANGLE_FAN:				return GL_TRIANGLE_FAN;
		case ObjectDisplayMode::ODM_TRIANGLES:					return GL_TRIANGLES;
		case ObjectDisplayMode::ODM_LINES_ADJACENCY:			return GL_LINES_ADJACENCY;
		case ObjectDisplayMode::ODM_LINE_STRIP_ADJACENCY:		return GL_LINE_STRIP_ADJACENCY;
		case ObjectDisplayMode::ODM_TRIANGLES_ADJACENCY:		return GL_TRIANGLES_ADJACENCY;
		case ObjectDisplayMode::ODM_TRIANGLE_STRIP_ADJACENCY:	return GL_TRIANGLE_STRIP_ADJACENCY;
		case ObjectDisplayMode::ODM_PATCHES:					return GL_PATCHES;
	}
}

Drawable::Drawable(usize const& layer = 0, bool const& manual = false) {
	if(!manual) setAuto(layer);
	manualMode = manual;
}

virtual Drawable::~Drawable() {
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

DrawableObject::DrawableObject(usize const& layer = 0, bool const& manual = false): Drawable(layer, manual) {
	DEBUGLN("Drawable object created!");
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
}

virtual DrawableObject::~DrawableObject() {
	DEBUGLN("Deleting buffers...");
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void DrawableObject::display(Vertex* const& vertices, usize const& count, ObjectDisplayMode const& mode = ObjectDisplayMode::ODM_TRIANGLES) {
	// If no instances, add one
	if (material.instances.empty())
		material.instances.push_back(Vector3(0, 0, 0));
	// Set blend mode
	setBlendMode();
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
	switch (material.culling) {
		case ObjectCulling::OC_NONE:	glDisable(GL_CULL_FACE);						break;
		case ObjectCulling::OC_FRONT:	glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);	break;
		case ObjectCulling::OC_BACK:	glEnable(GL_CULL_FACE); glCullFace(GL_BACK);	break;
	}
	// Set fill mode
	switch (material.fill) {
		case ObjectFill::OF_FILL:	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		case ObjectFill::OF_POINT:	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		case ObjectFill::OF_LINE:	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	// Draw object to screen
	glDrawArraysInstanced(convert(mode), 0, count, material.instances.size());
	// Disable culling
	glDisable(GL_CULL_FACE);
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
	Matrix4x4 cameraWorldActor(Global::camera.matrix() * Scene::world * trans);
	// Set transformation matrices
	shader["vertMatrix"](Global::camera.projection() * cameraWorldActor);
	shader["normalsMatrix"](cameraWorldActor.transposed().inverted());
}
