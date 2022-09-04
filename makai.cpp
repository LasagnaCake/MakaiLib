// #include <irrlicht.h>
#include <stdexcept>

#include "src/program.hpp"

#include <windows.h>

//#define SIDE_B

// [x] TODO: Fix rotation fuckery
//		^ Maybe try and cheat with glm::rotate or glm::rotate_vector
// [ ] TODO: Fix framebuffer
// [ ] TODO: Fix UV & Color data not passing to shader
// [X] TODO: Automate plane transformation

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
	Scene::camera.eye = Vector::Vector3(0, 10, 0);
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
	$debug("Size of vertex classes:");
	$debug(sizeof(Drawer::RawVertex));
	$debug(sizeof(Drawer::Vertex));
	$debug("Ratio:");
	$debug(sizeof(Drawer::RawVertex)/sizeof(float));
	$debug(sizeof(Drawer::Vertex)/sizeof(float));

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
		Scene::camera.eye.z -= (
			prog.input.getButtonDown(SDL_SCANCODE_S) -
			prog.input.getButtonDown(SDL_SCANCODE_W)
		)*(1.0/60.0);
		Scene::camera.eye.x -= (
			prog.input.getButtonDown(SDL_SCANCODE_D) -
			prog.input.getButtonDown(SDL_SCANCODE_A)
		)*(1.0/60.0);
		Scene::camera.at.z -= (
			prog.input.getButtonDown(SDL_SCANCODE_S) -
			prog.input.getButtonDown(SDL_SCANCODE_W)
		)*(1.0/60.0);
		Scene::camera.at.x -= (
			prog.input.getButtonDown(SDL_SCANCODE_D) -
			prog.input.getButtonDown(SDL_SCANCODE_A)
		)*(1.0/60.0);
		Scene::camera.at.y -= (
			prog.input.getButtonDown(SDL_SCANCODE_DOWN) -
			prog.input.getButtonDown(SDL_SCANCODE_UP)
		)*(1.0/60.0);
		/*Scene::camera.eye = Vector::Vector3(
			sin(frame/300.0f) * 3,
			2,
			cos(frame/300.0f) - 2
		);*/
		testRenderable.transform.local.position.z = 20;
		for (size_t i = 0; i < 10; i++) {
			float sinC = sin(frame/600.0f + i/1.0);
			//p[i]->local.position.x = sinC;
			p[i]->local.rotation.z = frame / 300.0f;
			//p[i]->local.rotation.y = frame / 300.0f;
			//Scene::camera.up.x =  sinC / 3;
			//testRenderable.transform.local.rotation.x = frame / 200.0f;
			//testRenderable.transform.local.rotation.y = frame / 200.0f;
			//testRenderable.transform.local.rotation.z = frame / 600.0f;
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
