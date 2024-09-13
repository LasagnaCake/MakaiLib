#include "glapi.cpp"

#include "core.hpp"

using namespace Makai::Graph::API;

constexpr GLenum convert(Facility const& facility) {
	switch (facility) {
		case Facility::GAF_DEPTH_TEST:	return GL_DEPTH_TEST;
		case Facility::GAF_BLEND:		return GL_BLEND;
		case Facility::GAF_DEBUG:		return GL_DEBUG_OUTPUT;
	}
	return 0;
}

bool opened = false;

inline String glDebugSource(GLenum const& source) {
	switch (source) {
		case GL_BUFFER:				return "BUFFER";
		case GL_SHADER:				return "SHADER";
		case GL_PROGRAM:			return "PROGRAM";
		case GL_VERTEX_ARRAY:		return "VERTEX ARRAY";
		case GL_QUERY:				return "QUERY";
		case GL_PROGRAM_PIPELINE:	return "PROGRAM PIPELINE";
		case GL_TRANSFORM_FEEDBACK:	return "TRANSFORM FEEDBACK";
		case GL_SAMPLER:			return "SAMPLER";
		case GL_TEXTURE:			return "TEXTURE";
		case GL_RENDERBUFFER:		return "RENDER BUFFER";
		case GL_FRAMEBUFFER:		return "FRAME BUFFER";
		default: return "UNKNOWN";
	}
}

inline String glDebugSeverity(GLenum const& severity) {
	switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:				return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM:			return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH:			return "HIGH";
		case GL_DEBUG_SEVERITY_NOTIFICATION:	return "NOTIFICATION";
		default: return "UNKNOWN";
	}
}

inline String glDebugType(GLenum const& type) {
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:				return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	return "DEPRECATED BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	return "UNDEFINED BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY:			return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE:			return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER:				return "MARKER";
		case GL_DEBUG_TYPE_PUSH_GROUP:			return "PUSH GROUP";
		case GL_DEBUG_TYPE_POP_GROUP:			return "POP GROUP";
		case GL_DEBUG_TYPE_OTHER:				return "OTHER";
		default: return "UNKNOWN";
	}
}

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
		"<api>\n"
		, "Source: "	, glDebugSource(source)		, "\n"
		, "Type: "		, glDebugType(type)			, "\n"
		, "Severity: "	, glDebugSeverity(severity)	, "\n"
		, "Message: "	, String(message)			, "\n"
		, "</api>"
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
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_FALSE);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

bool Makai::Graph::API::hasRequiredVersion() {
	if (!opened) return false;
	#if (MAKAILIB_GL_LOADER == MAKAILIB_USE_GL3W)
	return gl3wIsSupported(4, 2);
	#elif (MAKAILIB_GL_LOADER == MAKAILIB_USE_GLAD)
	return GLAD_GL_VERSION_4_2;
	#endif
}

String Makai::Graph::API::name() {
	return "OpenGL 4.2";
}

void Makai::Graph::API::toggle(Facility const& fac, bool const& state) {
	if (state)	glEnable(convert(fac));
	else		glDisable(convert(fac));
}

void Makai::Graph::API::clear(Buffer const& buffer) {
	switch (buffer) {
		case Buffer::GAB_COLOR:		glClear(GL_COLOR_BUFFER_BIT);
		case Buffer::GAB_DEPTH:		glClear(GL_DEPTH_BUFFER_BIT);
		case Buffer::GAB_STENCIL:	glClear(GL_STENCIL_BUFFER_BIT);
	}
}

void Makai::Graph::API::setClearColor(Vector4 const& color) {
	glClearColor(color.r, color.g, color.b, color.a);
}

void Makai::Graph::API::setFrontFace(bool const& clockwise) {
	glFrontFace(clockwise ? GL_CW : GL_CCW);
}
