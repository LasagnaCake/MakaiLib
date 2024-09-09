#include "glapi.cpp"

#include "core.hpp"

bool opened = false;

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
