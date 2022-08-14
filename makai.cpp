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
	prog.color = Vector::Vector4(Vector::Vector3(.5), 1);
	prog.onFrame = $func() {
		if (prog.input.getButtonDown(SDL_SCANCODE_ESCAPE))
			prog.close();
	};
	GLfloat triVerts[] = {
	    0.0,  0.8,
	   -0.8, -0.8,
	    0.8, -0.8,
	};
	GLint attribCoord2d;
	const char* attribName = "coord2d";
	GLuint program = Shader::defaultShader.getID();
	attribCoord2d = glGetAttribLocation(program, attribName);
	if (attribCoord2d == -1) {
		$errlog(std::string("Could not bind attribute ") + attribName);
		return -1;
	}
	prog.onDraw = $func() {
		triVerts[0] = sin(prog.input.getButtonState(SDL_SCANCODE_W)/60.0)/2.0;
		Shader::defaultShader();
		glEnableVertexAttribArray(attribCoord2d);
		/* Describe our vertices array to OpenGL (it can't guess its format automatically) */
		glVertexAttribPointer(
			attribCoord2d,		// attribute
			2,					// number of elements per vertex, here (x,y)
			GL_FLOAT,			// the type of each element
			GL_FALSE,			// take our values as-is
			0,					// no extra data between each position
			triVerts			// pointer to the C array
		);

		/* Push each element in buffer_vertices to the vertex shader */
		glDrawArrays(GL_TRIANGLES, 0, 3);
		triVerts[0] = -sin(prog.input.getButtonState(SDL_SCANCODE_W)/60.0)/2.0;
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(attribCoord2d);
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
