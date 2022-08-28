// #include <irrlicht.h>
#include <stdexcept>

#include "src/program.hpp"

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

	SLF::SLFData data = SLF::parseFile("shaders/postProcessing.slf");

	for (auto d: data)
		$debug(d);

	Shader::ShaderList test = Shader::getShaderList(data);

	//Shader::Shader test(data);

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
#endif
