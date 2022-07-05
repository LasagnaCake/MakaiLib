#ifndef MAKAILIB_MAKAI_LIBRARY_H
#define MAKAILIB_MAKAI_LIBRARY_H

#include <glad/glad.h>

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
#include <stdexcept>
#include <cstdlib>
#include <math.h>
#include <thread>
#include <map>

/*
*********************
*                   *
*  Graphical stuff  *
*                   *
*********************
*/

/*
#define OGRE_IGNORE_UNKNOWN_DEBUG
#include <OGRE/Ogre.h>
//*/

#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <graphical.hpp>

/*
***************************
*                         *
*  Engine-specific stuff  *
*                         *
***************************
*/
/*
#include <collection/algebra.hpp>

#ifdef _$_FLOAT_AS_DOUBLE
#define float double
#endif // _$_FLOAT_AS_DOUBLE

#include <collection/entity/entity.hpp>
#include <collection/vectorn.hpp>
#include <collection/cyclical.hpp>
#include <collection/matrices.hpp>
#include <collection/tween.hpp>
#include <collection/event.hpp>

#ifdef _$_FLOAT_AS_DOUBLE
#undef float
#endif // _$_FLOAT_AS_DOUBLE
*/
/*
*	[+]=[   [Headers END]   ]=[+]
*/

#ifdef _$_LUA_STYLE_PROGRAMMING
#define END		}
#define AND		&&
#define OR		||
#define NOT		!
#define THEN	)
#define EXECUTE	{
#define TAKE	(
#define NEXT	;
#define LESS	<
#define MORE	>
#define IS		==
#define EQUALS	=
#endif // _$_LUA_STYLE_PROGRAMMING

#define $main	[&](float delta) -> void

namespace Makai {
	namespace {
		using
		std::vector,
		std::function,
		std::string,
		std::map/*,
		Vector::Vector3,
		Vector::Vector4*/;
	}
	/**
	***********************
	*                     *
	*  Program structure  *
	*                     *
	***********************
	*/

	typedef function<void(float)>	OnFrameFunc;

	class InputManager {
	public:
		/// Returns whether the button is pressed.
		inline bool getButtonDown(int button);
		/**
		* Returns the button's state.
		* 0 = Released;
		* 1 = Pressed ("Tapped");
		* 2 = Held;
		* 3 = Repeating;
		*/
		int getButtonState(int button);
		/// The window to read inputs from.
		GLFWwindow* window;
	private:
		map<int, bool> isHeld;
	};

	struct Program {
	public:
		/// Initializes the program.
		void init(unsigned int width, unsigned int height, std::string windowTitle);
		/// Runs the program.
		void run();
		/// Closes the program.
		void close();
		/// Sets the window's background (clear) color.
		//void setClearColor(Vector4 color);
		/// Returns whether the program should be running.
		inline bool	running();
		/// The minimum frame rate.
		size_t frameRateMinimum = 120;
		/// Gets called every frame, along all other logic.
		OnFrameFunc		onFrame;
		/// Gets called when the program is closing. Happens before GLFW is terminated.
		//Event::Signal	onClose;
		/// The program's input manager.
		InputManager	input;
	private:
		/// Properly finishes program execution.
		void terminate();
		/// The program's title.
		string windowTitle;
		/// The program's window.
		GLFWwindow* window;
		/// Whether execution is to be halted.
		bool forceStop = false;
	} mainProgram;
}

#endif // MAKAILIB_MAKAI_H
