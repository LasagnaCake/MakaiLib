/*
*******************
*                 *
*  General stuff  *
*                 *
*******************
*/

#include <string>
#include <vector>
#include <functional>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <math.h>
#include <thread>

/*
***************************
*                         *
*  Engine-specific stuff  *
*                         *
***************************
*/

#ifdef _$_FLOAT_AS_DOUBLE
#define float double
#endif // _$_FLOAT_AS_DOUBLE

#include <entity/entity.hpp>
#include <vectorn.hpp>
#include <cyclical.hpp>
#include <algebra.hpp>
#include <matrices.hpp>
#include <tween.hpp>
#include <event.hpp>

#ifdef _$_FLOAT_AS_DOUBLE
#undef float
#endif // _$_FLOAT_AS_DOUBLE

/*
*********************
*                   *
*  Graphical stuff  *
*                   *
*********************
*/

#include <Ogre.h>
#include <GLFW/glfw3.h>

/*
#define IF		if (
#define END		}
#define ELSE	} else
#define AND		&&
#define OR		||
#define NOT		!
#define THEN	)
#define EXECUTE	{
//*/

#define $main [&](float delta) -> void

namespace Makai {
	namespace {
		using
		std::vector,
		std::function,
		std::string;
	}
	/**
	***********************
	*                     *
	*  Program structure  *
	*                     *
	***********************
	*/

	typedef function<void(float)>	OnFrameFunc;

	struct Program {
	public:
		void init(unsigned int, unsigned int, string);
		void run();
		void close();
		OnFrameFunc		onFrame;
		Event::Signal	onClose;
		inline bool running();
	private:
		void terminate();
		string windowTitle;
		GLFWwindow* window;
		bool forceStop = false;
	} $_PROGRAM;
}
