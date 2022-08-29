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
	float frame = 0;
	Scene::camera.eye = Vector::Vector3(0, 0, -1);
	Scene::camera.up = Vector::Vector3(0, 1, 0);
	prog.onFrame = $func() {
		if (prog.input.getButtonDown(SDL_SCANCODE_ESCAPE))
			prog.close();
		Scene::camera.at = Vector::Vector3(
			sin(frame/300.0f)/4.0,
			0.0,
			cos(frame/300.0f)/4.0
		);
		Scene::camera.eye.y = sin(prog.input.getButtonState(SDL_SCANCODE_W)/120.0)/10.0;
	};

	SLF::SLFData data = SLF::parseFile("shaders/base/base.slf");

	for (auto d: data)
		$debug(d);

	Shader::defaultShader.destroy();
	Shader::defaultShader.create(data);
	/*Shader::defaultShader.create(
		FileLoader::loadTextFile("shaders/base/base.vert"),
		FileLoader::loadTextFile("shaders/base/base.frag")
	);*/
	RenderData::Renderable testRenderable;

	$debug("Size of float:");
	$debug(sizeof(float));
	$debug("Size of raw vertex:");
	$debug(sizeof(Drawer::RawVertex));
	$debug("Ratio:");
	$debug(sizeof(Drawer::RawVertex)/sizeof(float));

	Vector::Vector3 triPos[] = {
		Vector::Vector3(1.0f, 1.0f, 0.0f),
		Vector::Vector3(1.0f, -1.0f, 0.0f),
		Vector::Vector3(-1.0f,1.0f, 0.0f)
	};

	testRenderable.triangles.push_back(new RenderData::Triangle(triPos));
	triPos[0].x = -1.0f;
	triPos[0].y = -1.0f;
	testRenderable.triangles.push_back(new RenderData::Triangle(triPos));
	//testRenderable.triangles.push_back(new RenderData::Triangle());
	testRenderable.transform.local.position.z = 5.0f;
	prog.onDraw = $func() {
		float sinC = (sin(frame/300.0f) + 1) / 4 + 0.25;
		float cosC = (cos(frame/300.0f) + 1) / 4 + 0.25;
		//testRenderable.triangles[0]->verts[0].position.x = sinC;
		//testRenderable.triangles[0]->verts[0].position.y = cosC;
		frame++;
	};
	prog.run();
	return 0;
}
#else
#endif
