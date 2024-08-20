#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

#include "framebuffer.cpp"

using namespace Makai::Graph;

Base::DrawBuffer::DrawBuffer(uint const& width, uint const& height) {
	create(width, height);
}

Base::DrawBuffer::~DrawBuffer() {
	destroy();
}

virtual Base::DrawBuffer& Base::DrawBuffer::destroy() {
	if (!created) return *this;
	else created = false;
	glDeleteFramebuffers(1, &id);
	return *this;
}

virtual Base::DrawBuffer& Base::DrawBuffer::create(uint const& width, uint const& height) {
	if (created) return *this;
	else created = true;
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	this->width = width;
	this->height = height;
	disable();
	return *this;
}

virtual Base::DrawBuffer& Base::DrawBuffer::enable() const {
	if (!created) return *this;
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	return *this;
}

Base::DrawBuffer& Base::DrawBuffer::operator()() const {
	return enable();
}

virtual Base::DrawBuffer& Base::DrawBuffer::disable() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return *this;
}

inline bool Base::DrawBuffer::exists() const {return created;}

inline uint Base::DrawBuffer::getWidth() const	{return width;	}
inline uint Base::DrawBuffer::getHeight() const	{return height;	}
inline uint Base::DrawBuffer::getID() const		{return id;		}


Base::FrameBuffer::FrameBuffer(
	unsigned int const& width,
	unsigned int const& height
) {
	create(width, height);
}

Base::FrameBuffer::~FrameBuffer() {
	destroy();
}

Base::FrameBuffer& Base::FrameBuffer::destroy() override {
	if (!exists()) return *this;
	buffer.screen.destroy();
	buffer.depth.destroy();
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	BaseBuffer::destroy();
	return *this;
}

Base::FrameBuffer& Base::FrameBuffer::create(uint const& width, uint const& height) override {
	if (exists()) return *this;
	Base::DrawBuffer::create(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, getID());
	buffer.screen.create(width, height, GL_FLOAT, GL_RGBA, GL_LINEAR, GL_LINEAR, NULL, GL_RGBA16F);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		buffer.screen.getID(),
		0
	);
	buffer.depth.create(width, height, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL, GL_LINEAR, GL_LINEAR, NULL, GL_DEPTH24_STENCIL8);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D,
		buffer.depth.getID(),
		0
	);
	// Setup display rectangle
	rect[0] = {-1, +1, 0, 0, 1};
	rect[1] = {+1, +1, 0, 1, 1};
	rect[2] = {-1, -1, 0, 0, 0};
	rect[3] = {+1, -1, 0, 1, 0};
	// Create buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	setBlendFunction(DEFAULT_BLEND_FUNC);
	setBlendEquation(DEFAULT_BLEND_EQUATION);
	disable();
	return *this;
}

Base::FrameBuffer& Base::FrameBuffer::enable() const override {
	if (!exists()) return *this;
	BaseBuffer::enable();
	this->clearDepthBuffer();
	return *this;
}

Base::FrameBuffer Base::FrameBuffer::toFrameBufferData() const {
	if (!exists())
		return FrameBufferData{};
	return FrameBufferData{
		getID(),
		getWidth(),
		getHeight(),
		&buffer.screen,
		&buffer.depth
	};
}

FrameBuffer& Base::FrameBuffer::clearBuffers() const {
	this->clearColorBuffer();
	this->clearDepthBuffer();
	return *this;
}

Base::FrameBuffer& Base::FrameBuffer::clearColorBuffer() const {
	Vector4& color = material.background;
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
	return *this;
}

Base::FrameBuffer& Base::FrameBuffer::clearDepthBuffer() const {
	glClear(GL_DEPTH_BUFFER_BIT);
	return *this;
}

virtual Base::FrameBuffer& Base::FrameBuffer::render(FrameBufferData const& target) const {
	if (!exists()) return *this;
	// Set target buffer
	glBindFramebuffer(GL_FRAMEBUFFER, target.id);
	// set blend func & equation data
	setBlendMode();
	// Set VBO as active
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Copy vertices to VBO
	glBufferData(
		GL_ARRAY_BUFFER,
		RAW_VERTEX_BYTE_SIZE * 4,
		rect,
		GL_STATIC_DRAW
	);
	// Set VAO as active
	glBindVertexArray(vao);
	// Define vertex data in VBO
	Vertex::setAttributes();
	// Get shader
	// Activate shader
	shader();
	// Set shader textures
	buffer.depth(30);
	buffer.screen(31);
	// Set camera's near and far plane
	shader["near"](Scene::camera.zNear);
	shader["far"](Scene::camera.zFar);
	// Set texture locations
	shader["depth"](30);
	shader["screen"](31);
	// Set transformation matrix
	shader["posMatrix"](Matrix4x4(trans));
	shader["uvMatrix"](Matrix4x4(uv));
	Vector2 const resolution = Vector2(getWidth(), getHeight());
	shader["resolution"](resolution);
	shader["screenVUSpace"](screenVUSpace);
	shader["pixelVU"](resolution / screenVUSpace);
	// Enable attribute pointers
	Vertex::enableAttributes();
	// Set VAO as active
	glBindVertexArray(vao);
	// Set polygon rendering mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	// Draw object to screen
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// Unbind vertex array
	glBindVertexArray(0);
	// Disable attributes
	Vertex::disableAttributes();
	disable();
	return *this;
}

Base::FrameBuffer& Base::FrameBuffer::render(FrameBuffer& targetBuffer) const {
	if (!exists()) return *this;
	if (!targetBuffer.exists()) return *this;
	return render(targetBuffer.toFrameBufferData());
}

Base::FrameBuffer& Base::FrameBuffer::disable() const override {
	Base::DrawBuffer::disable();
	return *this;
}
