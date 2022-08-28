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
		Scene::camera = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, sin(prog.input.getButtonDown(SDL_SCANCODE_W)), 1.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
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

	$debug("Size of float:");
	$debug(sizeof(float));
	$debug("Size of raw vertex:");
	$debug(sizeof(Drawer::RawVertex));
	$debug("Ratio:");
	$debug(sizeof(Drawer::RawVertex)/sizeof(float));
	$debug(sizeof(triVerts));

	Vector::Vector3 triPos[] = {
		Vector::Vector3(1.0f, 1.0f, 0.1f),
		Vector::Vector3(1.0f, -1.0f, 0.1f),
		Vector::Vector3(-1.0f,1.0f, 0.1f)
	};

	testRenderable.triangles.push_back(RenderData::Triangle3D(triPos));

	prog.onDraw = $func() {

	};
	prog.run();
	return 0;
}
#else
#endif
