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
#include <map>

/*
***************************
*                         *
*  Engine-specific stuff  *
*                         *
***************************
*/

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

/*
*********************
*                   *
*  Graphical stuff  *
*                   *
*********************
*/

#define OGRE_IGNORE_UNKNOWN_DEBUG

#include <OGRE/Ogre.h>
#include <GLFW/glfw3.h>
#include <graphical.hpp>

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
		std::map;
	}
	/**
	***********************
	*                     *
	*  Program structure  *
	*                     *
	***********************
	*/

	typedef function<void(float)>	OnFrameFunc;

	struct InputManager {
		/// Sets the target window.
		inline void setWindow(GLFWwindow* window);
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
	private:
		/// The window to read inputs from.
		GLFWwindow* window;
		map<int, bool> isHeld;
	};

	struct Program {
	public:
		/// The minimum frame rate.
		size_t frameRateMinimum = 120;
		/// Initializes the program.
		void init(unsigned int width, unsigned int height, std::string windowTitle = "WINDOW");
		/// Runs the program.
		void run();
		/// Closes the program.
		void close();
		/// Gets called every frame, along all other logic.
		OnFrameFunc		onFrame;
		/// Gets called when the program is closing. Happens before GLFW is terminated.
		Event::Signal	onClose;
		/// Returns whether the program should be running.
		inline bool		running();
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
	} $_MAIN;
}
