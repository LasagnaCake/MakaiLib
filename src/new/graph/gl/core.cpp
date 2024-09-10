#include "glapi.cpp"

#include "core.hpp"

using namespace Makai::Graph::API;

constexpr GLenum convert(Facility const& facility) {
	switch (facility) {
		case Facility::GAF_DEPTH_TEST:	return GL_DEPTH_TEST;
		case Facility::GAF_BLEND:		return GL_BLEND;
		case Facility::GAF_DEBUG:		return GL_DEBUG_OUTPUT;
	}
}

bool opened = false;

void GLAPIENTRY glAPIMessageCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
) {
	DEBUGLN(
		"[GL CALLBACK"
		, String(type == GL_DEBUG_TYPE_ERROR ? " (GL ERROR)" : "")	, "] "
		, "Type: "		, type				, ", "
		, "Severity: "	, severity			, ", "
		, "Message: "	, String(message)	, ""
	);
}

void Makai::Graph::API::open() {
	if (opened) return;
	#if (MAKAILIB_GL_LOADER == MAKAILIB_USE_GL3W)
	if (gl3wInit() != GL3W_OK)
		throw Error::FailedAction(
			"Failed to initialize GL3W!",
			__FILE__,
			toString(__LINE__),
			"Makai::Graph::open"
		);
	#elif (MAKAILIB_GL_LOADER == MAKAILIB_USE_GLAD)
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        throw Error::FailedAction(
			"Failed to initialize GLAD!",
			__FILE__,
			toString(__LINE__),
			"Makai::Graph::open"
		);
	#endif
	opened = true;
	glDebugMessageCallback(glAPIMessageCallback, 0);
}

bool Makai::Graph::API::hasRequiredVersion() {
	if (!opened) return false;
	#if (MAKAILIB_GL_LOADER == MAKAILIB_USE_GL3W)
	return gl3wIsSupported(4, 2);
	#elif (MAKAILIB_GL_LOADER == MAKAILIB_USE_GLAD)
	return GLAD_GL_VERSION_4_2;
	#endif
}

String Makai::Graph::API::versionString() {
	return "OpenGL 4.2";
}

void Makai::Graph::API::toggle(Facility const& fac, bool const& state) {
	if (state)	glEnable(convert(fac));
	else		glDisable(convert(fac));
}

void Makai::Graph::API::clear(Buffer const& buffer) {
	switch (buffer) {
		case Buffer::GAB_COLOR: glClear(GL_COLOR_BUFFER_BIT);
		case Buffer::GAB_DEPTH: glClear(GL_DEPTH_BUFFER_BIT);
	}
}

void Makai::Graph::API::setClearColor(Vector4 const& color) {
	glClearColor(color.r, color.g, color.b, color.a);
}

void Makai::Graph::API::setFrontFace(bool const& clockwise) {
	glFrontFace(clockwise ? GL_CW : GL_CCW);
}
