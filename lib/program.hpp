#ifndef MAKAILIB_MAKAI_LIBRARY_H
#define MAKAILIB_MAKAI_LIBRARY_H

#include "anchors.hpp"
#include "graphical.hpp"

namespace Makai {
	namespace {
		using
		std::vector,
		std::function,
		std::string,
		std::map,
		Vector::VecV3,
		Vector::VecV4;
	}
	/**
	*************************
	*                       *
	*  Event Manager class  *
	*                       *
	*************************
	*/
	/*
	class EventManager{
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
				// Get key, state and current state
				auto inputKey		= event.KeyInput.Key;
				auto pressedDown	= event.KeyInput.PressedDown;
				auto currentState	= keyState[inputKey];
				// Get current key state (released, pressed, held)
				// If key is pressed...
				if (pressedDown) {
					// If state counter is not close to overflowing...
					if (currentState < 255)
						// Increment state counter
						currentState++;
				// Else, set counter to zero
				} else currentState = 0;
				// Set current key state
				keyState[inputKey] = currentState;
			}
			return false;
		}
		// This is used to check whether a key is being held down
		virtual bool IsKeyDown(EKEY_CODE keyCode) const {
			return keyState[keyCode];
		}

		/**
		* Returns the key's state.
		* 0		= Released;
		* 1		= Pressed ("Tapped");
		* 2+	= Held;
		*//*
		inline unsigned char getKeyState(EKEY_CODE keyCode) {
			return keyState[keyCode];
		}
	private:
		/// The current state of each key.
		unsigned char keyState[KEY_KEY_CODES_COUNT];
	};*/

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
			window.create(sf::VideoMode(width, height), windowTitle);
		}

		/// Sets the window's title.
		void setWindowTitle(std::string windowTitle) {
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
			while(window.isOpen() && shouldRun) {
				// Process events
				sf::Event event;
				while (window.pollEvent(event)) {
					// Close window : exit
					shouldRun = !(event.type == sf::Event::Closed);
				}
				// Start thread
				std::thread physics(physFunc, fixedDelta);
				// Do your own stuff
				logicFunc(fixedDelta);
				onFrame();
				// Wait for thread to be done processing
				physics.join();
				// Draw screen
				VecV4 color = windowColor.clamped(VecV4(0.0), VecV4(1.0)) * 255;
				window.clear(sf::Color(color.x, color.y, color.z, color.w));
				onDraw();
				window.display();
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
			return (shouldRun);
		}

		/// Sets the program's window size.
		void setWindowSize(){

		}

		/// Gets called whenever the program is rendering to the screen. Happens after 3D render, but before GUI render.
		Event::Signal	onDraw	= $func {};


		/// Gets called every frame, along all other logic.
		Event::Signal	onFrame = $func {};


		/// Gets called when the program is closing. Happens before Window is terminated.
		Event::Signal	onClose = $func {};

		/// Gets the program window.
		sf::RenderWindow* getWindow() {
			return &window;
		}

		/// The program's window output.
		struct {
		} out;

		/// The window's color.
		VecV4 windowColor = VecV4(.5, .5, .5, 1.0);
	private:
		/// Properly finishes program execution.
		void terminate() {
			// Call final function
			onClose();
			// Close window if open
			if (window.isOpen()) window.close();
		}
		/// Current execution state.
		bool shouldRun = true;
		/// The program window.
		sf::RenderWindow window;
	};
}

#endif // MAKAILIB_MAKAI_H
