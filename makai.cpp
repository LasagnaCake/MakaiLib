#include <irrlicht.h>
#include <stdexcept>

#define _BURNINGS_

//#include <makai.hpp>
#include "lib/program.hpp"
#include <windows.h>

int main() {
	/*
	***************************
	*                         *
	*  Console Disabler Code  *
	*                         *
	***************************
	*/

	#ifndef _MAKAI_DEBUG_
	#if (_WIN32 || _WIN64)
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	#endif // _WINDOWS
	#endif // _MAKAI_DEBUG_

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
	prog.windowColor = Vector::Vector4(100,100,140,255) / 255;
	prog.out.gui->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10,10,300,300), true);
	irr::scene::ISceneNode* n = prog.out.scene->addCubeSceneNode();
	irr::scene::ICameraSceneNode* cam = prog.out.scene->addCameraSceneNode(0, irr::core::vector3df(0,30,-40), irr::core::vector3df(0,5,0));
	if (n)
	{
		n->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		irr::scene::ISceneNodeAnimator* anim = prog.out.scene->createFlyCircleAnimator(irr::core::vector3df(0,0,30), 20.0f);
		if (anim)
		{
			cam->addAnimator(anim);
			anim->drop();
		}
	}
	prog.onFrame = $func {};
	prog.run();
	return 0;
}
