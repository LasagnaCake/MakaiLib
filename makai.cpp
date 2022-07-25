// #include <irrlicht.h>
#include <stdexcept>

#include "lib/program.hpp"
#ifndef _MAKAI_DEBUG_
#if (_WIN32 || _WIN64)
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
#endif // _WINDOWS
#else
int main() {
#endif // _MAKAI_DEBUG_
	/*
	***********************
	*                     *
	*  Main Program Code  *
	*                     *
	***********************
	*//*
	//Makai::Program prog(640, 480, "OK");
	//Makai::Program prog(720, 640, "OK");
	Makai::Program prog(960, 720, "OK");
	//Makai::Program prog(1280, 960, "OK");
	prog.windowColor = Vector::Vector4(Vector::Vector3(0),255) / 255;
	sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(300, 300);
	prog.onDraw = $func {
		prog.getWindow()->draw(shape);
	};
	prog.run();
	return 0;*/
}
