// #include <irrlicht.h>
#include <stdexcept>

#include "src/program.hpp"

#include <windows.h>

//#define SIDE_B

// [ ] TODO: Fix framebuffer

class GameApp: public Makai::Program{
	void onCreate() override {
	}

	void onLogicFrame() override {
	}

	void onDrawBegin() override {
	}

	void onDrawEnd() override {
	}

	void onClose() override {
	}
};

#ifndef SIDE_B
int main() {
	/*
	***********************
	*                     *
	*  Main Program Code  *
	*                     *
	***********************
	*/

	Makai::Program prog(960, 720, "[TEST]");

	// [[ Main Program Code END ]]

	prog.color = Color::GRAY;

	Scene::camera.eye = Vector::Vector3(0, 10, 0);
	Scene::camera.at = Vector::Vector3(0, 0, 10);
	Scene::camera.up = Vector::Vector3(0, 1, 0);

	SLF::SLFData data = SLF::parseFile("shaders/base/base.slf");
	Shader::defaultShader.destroy();
	Shader::defaultShader.create(data);

	// [[ Main Program Code END ]]

	prog.run();

	return 0;
}
#else
#endif
