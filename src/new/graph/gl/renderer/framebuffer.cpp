#include "../glapiloader.cc"

#include "framebuffer.hpp"
#include "../global.hpp"

namespace ImageSlot {
	constexpr uint8 SCREEN	= 30;
	constexpr uint8 DEPTH	= 31;
}

using namespace Makai; using namespace Makai::Graph;

Graph::Base::DrawBuffer::DrawBuffer(uint const& width, uint const& height) {
	create(width, height);
}

Graph::Base::DrawBuffer::~DrawBuffer() {
	destroy();
}

Graph::Base::DrawBuffer& Graph::Base::DrawBuffer::destroy() {
	if (!created) return *this;
	else created = false;
	glDeleteFramebuffers(1, &id);
	return *this;
}

Graph::Base::DrawBuffer& Graph::Base::DrawBuffer::create(uint const& width, uint const& height) {
	if (created) return *this;
	else created = true;
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	this->width = width;
	this->height = height;
	disable();
	return *this;
}

Graph::Base::DrawBuffer& Graph::Base::DrawBuffer::enable() {
	if (!created) return *this;
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	return *this;
}

Graph::Base::DrawBuffer& Graph::Base::DrawBuffer::operator()() {
	return enable();
}

Graph::Base::DrawBuffer& Graph::Base::DrawBuffer::disable() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return *this;
}

bool Graph::Base::DrawBuffer::exists() const	{return created;}

uint Graph::Base::DrawBuffer::getWidth() const	{return width;	}
uint Graph::Base::DrawBuffer::getHeight() const	{return height;	}
uint Graph::Base::DrawBuffer::getID() const		{return id;		}


Graph::Base::FrameBuffer::FrameBuffer(
	unsigned int const& width,
	unsigned int const& height
): FrameBuffer() {
	create(width, height);
}

Graph::Base::FrameBuffer::~FrameBuffer() {
	destroy();
}

Graph::Base::FrameBuffer& Graph::Base::FrameBuffer::destroy() {
	if (!exists()) return *this;
	buffer.screen.destroy();
	buffer.depth.destroy();
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	Base::DrawBuffer::destroy();
	return *this;
}

Graph::Base::FrameBuffer& Graph::Base::FrameBuffer::create(uint const& width, uint const& height) {
	if (exists()) return *this;
	Base::DrawBuffer::create(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, getID());
	buffer.screen.create(
		width,
		height,
		Image2D::ComponentType::CT_FLOAT,
		Image2D::ImageFormat::IF_RGBA,
		Image2D::FilterMode::FM_SMOOTH,
		Image2D::FilterMode::FM_SMOOTH,
		NULL,
		Image2D::ComponentLayout::CL_RGBA_16F
	);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		buffer.screen.getID(),
		0
	);
	buffer.depth.create(
		width,
		height,
		Image2D::ComponentType::CT_UINT_24_8,
		Image2D::ImageFormat::IF_DS,
		Image2D::FilterMode::FM_SMOOTH,
		Image2D::FilterMode::FM_SMOOTH,
		NULL,
		Image2D::ComponentLayout::CL_D24_S8
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D,
		buffer.depth.getID(),
		0
	);
	// Setup display rectangle
	rect[0] = Vertex(-1, +1, 0, 0, 1);
	rect[1] = Vertex(+1, +1, 0, 1, 1);
	rect[2] = Vertex(-1, -1, 0, 0, 0);
	rect[3] = Vertex(+1, -1, 0, 1, 0);
	// Create buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	disable();
	return *this;
}

Graph::Base::FrameBuffer& Graph::Base::FrameBuffer::enable() {
	if (!exists()) return *this;
	Base::DrawBuffer::enable();
	this->clearDepthBuffer();
	return *this;
}

FrameBufferData Graph::Base::FrameBuffer::data() {
	if (!exists())
		return FrameBufferData{};
	return FrameBufferData{
		getID(),
		getWidth(),
		getHeight(),
		buffer.screen,
		buffer.depth
	};
}

Graph::Base::FrameBuffer& Graph::Base::FrameBuffer::clearBuffers() {
	this->clearColorBuffer();
	this->clearDepthBuffer();
	return *this;
}

Graph::Base::FrameBuffer& Graph::Base::FrameBuffer::clearColorBuffer() {
	// Why does this line cause a problem?
	API::setClearColor(clearColor);
	API::clear(API::Buffer::GAB_COLOR);
	return *this;
}

Graph::Base::FrameBuffer& Graph::Base::FrameBuffer::clearDepthBuffer() {
	API::clear(API::Buffer::GAB_DEPTH);
	return *this;
}

Graph::Base::FrameBuffer& Graph::Base::FrameBuffer::render(FrameBufferData const& target) {
	#ifdef MAKAILIB_DEBUG
	API::Debug::Context ctx("FrameBuffer::render");
	#endif // MAKAILIB_DEBUG
	if (!exists()) return *this;
	// Set target buffer
	glBindFramebuffer(GL_FRAMEBUFFER, target.id);
	// set blend func & equation data
	useBlendMode();
	// Set VBO as active
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Copy vertices to VBO
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(Vertex) * 4,
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
	buffer.depth(ImageSlot::DEPTH);
	buffer.screen(ImageSlot::SCREEN);
	// Set camera's near and far plane
	shader["near"](Global::camera.zNear);
	shader["far"](Global::camera.zFar);
	// Set texture locations
	shader["depth"](ImageSlot::DEPTH);
	shader["screen"](ImageSlot::SCREEN);
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

Graph::Base::FrameBuffer& Graph::Base::FrameBuffer::render(FrameBuffer& targetBuffer) {
	if (!exists()) return *this;
	if (!targetBuffer.exists()) return *this;
	return render(targetBuffer.data());
}

Graph::Base::FrameBuffer& Graph::Base::FrameBuffer::disable() {
	Base::DrawBuffer::disable();
	return *this;
}
