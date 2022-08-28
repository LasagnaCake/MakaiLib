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

	SLF::SLFData data = SLF::parseFile("shaders/base/base.slf");

	for (auto d: data)
		$debug(d);

	Shader::defaultShader.destroy();
	Shader::defaultShader.create(data);
	RenderData::Renderable testRenderable;

	Vector::Vector3 triPos[] = {
		Vector::Vector3(1.0f, 1.0f, 1.0f),
		Vector::Vector3(1.0f, -1.0f, 1.0f),
		Vector::Vector3(-1.0f,1.0f, 1.0f)
	};

	testRenderable.triangles.push_back(RenderData::Triangle3D(triPos));

	prog.onDraw = $func() {

	};
	prog.run();
	return 0;
}
#else
#endif
