// #include <irrlicht.h>
#include <stdexcept>

#include "lib/program.hpp"

#include <windows.h>

//#define SIDE_B

#ifndef SIDE_B
int main() {
	/*
	***********************
	*                     *
	*  Main Program Code  *
	*                     *
	***********************
	*/
	//Makai::Program prog(640, 480, "OK");
	//Makai::Program prog(720, 640, "OK");
	Makai::Program prog(960, 720, "OK");
	//Makai::Program prog(1280, 960, "OK");
	prog.windowColor = Vector::Vector4(Vector::Vector3(.5), 1);
	prog.onFrame = $func() {
		if (prog.input.getButtonDown(SDL_SCANCODE_ESCAPE))
			prog.close();
		$debug(prog.input.getButtonState(SDL_SCANCODE_W));
	};
	prog.run();
	return 0;
}
#else
using namespace std;
/* ADD GLOBAL VARIABLES HERE LATER */

bool init_resources(void) {
  /* FILLED IN LATER */
  return true;
}

void render(SDL_Window* window) {
  /* FILLED IN LATER */
}

void free_resources() {
  /* FILLED IN LATER */
}

void mainLoop(SDL_Window* window) {
	while (true) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				return;
		}
		render(window);
	}
}

int main() {
/* SDL-related initialising functions */
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("My First Triangle",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_GL_CreateContext(window);

	/* Extension wrangler initialising */
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << endl;
		return EXIT_FAILURE;
	}

	/* When all init functions run without errors,
	   the program can initialise the resources */
	if (!init_resources())
		return EXIT_FAILURE;

	/* We can display something if everything goes OK */
	mainLoop(window);

	/* If the program exits in the usual way,
	   free resources and exit with a success */
	free_resources();
	return EXIT_SUCCESS;
}
#endif
