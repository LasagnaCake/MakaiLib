#ifndef MAKAILIB_MAKAI_LIBRARY_H
#define MAKAILIB_MAKAI_LIBRARY_H

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

#include <irrlicht.h>
#include <graphical.hpp>

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

#define $func	[&]() -> void

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

namespace Makai {
	namespace {
		using namespace irr;

		using namespace core;
		using namespace scene;
		using namespace video;
		using namespace io;
		using namespace gui;

		using
		std::vector,
		std::function,
		std::string,
		std::map,
		Vector::Vector3,
		Vector::Vector4;
	}
	/**
	*************************
	*                       *
	*  Event Manager class  *
	*                       *
	*************************
	*/

	class EventManager : public IEventReceiver {
	public:
		EventManager() {
			for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
				keyState[i] = 0;
		}

		// This is the one method that we have to implement
		virtual bool OnEvent(const SEvent& event)
		{
			// If event is a keyboard event...
			if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
				// Get key
				auto inputKey = event.KeyInput.Key;
				// Get current key state (released, pressed, held)
				unsigned char state = 0;
				if (event.KeyInput.PressedDown)
					state = (
						(event.KeyInput.PressedDown)
					+	(keyState[inputKey] != 0)
					);
				// Set current key state
				keyState[inputKey] = state;
			}
			return false;
		}
		// This is used to check whether a key is being held down
		virtual bool IsKeyDown(EKEY_CODE keyCode) const {
			return keyState[keyCode];
		}

		/**
		* Returns the key's state.
		* 0 = Released;
		* 1 = Pressed ("Tapped");
		* 2 = Held;
		*/
		inline unsigned char getKeyState(EKEY_CODE keyCode) {
			return keyState[keyCode];
		}
	private:
		/// The current state of each key.
		unsigned char keyState[KEY_KEY_CODES_COUNT];
	};

	/**
	*******************
	*                 *
	*  Program class  *
	*                 *
	*******************
	*/
	struct Program {
	public:
		/// Initializes the program.
		Program(unsigned int width, unsigned int height, std::string windowTitle, bool fullscreen = false) {
			#define $supported($DRIVER) irr::IrrlichtDevice::isDriverSupported($DRIVER)
			// The available driver to be used
			video::E_DRIVER_TYPE driver = video::EDT_SOFTWARE;
			// Try and get a video driver
			if ($supported(video::EDT_OPENGL))
				driver = video::EDT_OPENGL;
			else if($supported(video::EDT_BURNINGSVIDEO))
				driver = video::EDT_BURNINGSVIDEO;
			#undef $supported
			// Try and create an irrlicht device (window)
			window = createDevice(
				driver,								// The selected video driver
				dimension2d<u32>(width, height),	// Window dimensions
				32,									// Color depth (32 bits)
				fullscreen,							// Fullscreen or Windowed
				false,								// Use stencil buffer (for shadows)
				fullscreen,							// Enable VSync
				&event								// Event manager
			);
			if (!window)
				throw std::runtime_error("Window could not be created!");
			// Set window title
			setWindowTitle(windowTitle);
			// Get device's implementations
			out.gui		= window->getGUIEnvironment();
			out.cursor	= window->getCursorControl();
			out.scene	= window->getSceneManager();
			out.driver	= window->getVideoDriver();
			out.file	= window->getFileSystem();
			out.logger	= window->getLogger();
		}

		/// Sets the window's title.
		void setWindowTitle(std::string windowTitle) {
			// Set window title
			std::wstring winTitle = std::wstring(windowTitle.begin(), windowTitle.end());
			window->setWindowCaption(winTitle.c_str());
		}

		/// Runs the program.
		void run(){
			auto fixedDelta = 1/60;
			// The physics process
			auto physFunc	= [&](float delta)-> void {

			};
			// The logical process
			auto logicFunc	= [&](float delta)-> void {
				Tween::yieldAllTweens();
				Event::yieldAllTimers();
				EntityClass::$_ROOT.yield(delta);
			};
			// While program is running...
			while(window->run() && shouldRun) {
				// Pause irrlicht execution
				window->yield();
				// Start thread
				std::thread physics(physFunc, fixedDelta);
				// Do your own stuff
				logicFunc(fixedDelta);
				onFrame();
				// Wait for thread to be done processing
				physics.join();
				// Draw screen
				Vector4 color = windowColor.clamped(Vector4(0.0), Vector4(1.0)) * 255;
				out.driver->beginScene(true, true, SColor(color.w,color.x,color.y,color.z));
				out.scene->drawAll();
				out.gui->drawAll();
				out.driver->endScene();
			}
			// Terminate program
			terminate();
		}


		/// Closes the program.
		void close(){
			shouldRun = false;
		}


		/// Returns whether the program is currently running.
		inline bool	running(){
			return (!shouldRun);
		}

		/// Sets the program's window size.
		void setWindowSize(){

		}


		/// Gets called every frame, along all other logic.
		Event::Signal	onFrame = $func {};


		/// Gets called when the program is closing. Happens before Window is terminated.
		Event::Signal	onClose = $func {};


		/// The program's window output.
		struct {
			IVideoDriver*		driver;
			ISceneManager*		scene;
			IGUIEnvironment*	gui;
			IFileSystem*		file;
			ICursorControl*		cursor;
			ILogger*			logger;
		} out;

		/// The program's event manager.
		EventManager event;

		/// The window's color.
		Vector4 windowColor = Vector4(Vector3(0.45), 1.0);
	private:
		/// Properly finishes program execution.
		void terminate() {
			// Call final function
			onClose();
			// End execution
			window->drop();
		}
		/// Current execution state.
		bool shouldRun = true;
		/// The program window.
		IrrlichtDevice* window;
	};
}

#endif // MAKAILIB_MAKAI_H
