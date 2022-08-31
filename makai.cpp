// #include <irrlicht.h>
#include <stdexcept>

#include "src/program.hpp"

#include <windows.h>

//#define SIDE_B

// TODO: Fix shitty vector rotation math (It's worse)
// TODO: Fix framebuffer
// TODO: Fix UV & Color data not passing to shader
// TODO: Fix life
// TODO: Automate plane transformation

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
	Scene::camera.eye = Vector::Vector3(-5, 2, -1);
	Scene::camera.at = Vector::Vector3(0, 0, 10);
	Scene::camera.up = Vector::Vector3(0, 1, 0);

	SLF::SLFData data = SLF::parseFile("shaders/base/base.slf");

	for (auto d: data)
		$debug(d);

	Shader::defaultShader.destroy();
	Shader::defaultShader.create(data);
	Shader::Shader fog(FileLoader::loadTextFile("shaders/postProcessing/fog.frag"), GL_FRAGMENT_SHADER);
	/*Shader::defaultShader.create(
		FileLoader::loadTextFile("shaders/base/base.vert"),
		FileLoader::loadTextFile("shaders/base/base.frag")
	);*/

	$debug("Size of float:");
	$debug(sizeof(float));
	$debug("Size of raw vertex:");
	$debug(sizeof(Drawer::RawVertex));
	$debug("Ratio:");
	$debug(sizeof(Drawer::RawVertex)/sizeof(float));

	RenderData::Renderable testRenderable;

	RenderData::PlaneReference* p[10];
	for (size_t i = 0; i < 10; i++) {
		p[i] = testRenderable.createPlaneReference();
		p[i]->local.position.z = 5*i + 5;
	}
	/*p->transform(
		VecMath::Transform3D(
			Vector::Vector3(0, 0, 5),
			Vector::Vector3(0, 0, 0),
			Vector::Vector3(1, 1, 1)
		)
	);*/
	p[0]->setColor(
		Vector::Vector4(1,0,0,1),
		Vector::Vector4(1,0,0,1),
		Vector::Vector4(0),
		Vector::Vector4(0)
	);
	//testRenderable.triangles.push_back(new RenderData::Triangle());

	prog.onFrame = $func() {
		if (prog.input.getButtonDown(SDL_SCANCODE_ESCAPE))
			prog.close();
		/*Scene::camera.eye = Vector::Vector3(
			sin(frame/300.0f) * 3,
			2,
			cos(frame/300.0f) - 2
		);*/
		for (size_t i = 0; i < 10; i++) {
			float sinC = sin(frame/200.0f + i/1.0) * 2;
			//p[i]->local.position.x = sinC;
			p[i]->local.rotation = Vector::Vector3(frame/100.0, 0, 0);
		}
	};

	prog.onDraw = $func() {
		//testRenderable.triangles[0]->verts[0].position.x = sinC;
		//testRenderable.triangles[0]->verts[0].position.y = cosC;
		frame++;
	};
	prog.run();
	return 0;
}
#else
#endif
