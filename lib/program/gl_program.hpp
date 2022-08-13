#ifndef MAKAILIB_MAKAI_LIBRARY_H
#define MAKAILIB_MAKAI_LIBRARY_H

#include "../anchors.hpp"
#include "../graphical.hpp"

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
	struct InputManager {
		/// Empty Constructor.
		InputManager() {}

		/// Returns whether the button is pressed.
		inline bool getButtonDown(SDL_Scancode button) {
			if (!enabled) return false;
			const unsigned char* state = SDL_GetKeyboardState(nullptr);
			return state[button];
		}

		/**
		* Returns the button's state.
		* 0		= Released;
		* 1+	= Pressed;
		* Recommended if time pressed is required.
		*/
		unsigned int getButtonState(SDL_Scancode button) {
			if (!enabled) return 0;
			// Get previous key state
			unsigned int buttonState = 0;
			if (buffer[button]) buttonState = buffer[button];
			// If button is pressed...
			if(getButtonDown(button)) {
				// If buffer not overflowing, increment buffer
				if(buttonState < 0xffff) buttonState++;
			}
			// Else, zero state
			else buttonState = 0;
			// Set buffer to button state
			buffer[button] = buttonState;
			// return button state
			return buttonState;

		}
		/// Whether input is enabled.
		bool enabled = true;
	private:
		/// The key states.
		unsigned char* state;
		/// The internal buffer state.
		map<SDL_Scancode, unsigned int> buffer;
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
		Program (
			unsigned int width,
			unsigned int height,
			std::string windowTitle,
			unsigned int fps = 60,
			bool fullscreen = false
		) {
			// Initialize SDL
			SDL_Init(SDL_INIT_VIDEO);
			// Create window and make active
			window = SDL_CreateWindow(
				windowTitle.c_str(),
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				width,
				height,
				SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0)
			);
			SDL_GL_CreateContext(window);
			// Try and initialize GLEW
			GLenum glew_status = glewInit();
			if (glew_status != GLEW_OK) {
				$errlog("Error: glewInit: " << glewGetErrorString(glew_status));
				throw std::runtime_error(std::string("Error: glewInit"));
			}
		}

		/// Sets the window's title.
		void setWindowTitle(std::string windowTitle) {
		}

		/// Runs the program.
		void run(){
			// The physics process
			auto physFunc	= [&](float delta)-> void {

			};
			// The logical process
			auto logicFunc	= [&](float delta)-> void {
				Tween::yieldAllTweens();
				Event::yieldAllTimers();
				EntityClass::$_ROOT.yield(delta);
			};
			// SDL's events
			SDL_Event event;
			// While program is running...
			while(shouldRun) {
				// Poll events and check if should close
				while (SDL_PollEvent(&event))
					if (event.type == SDL_QUIT)
						shouldRun = false;
				// Update input manager
				// input.update();
				// Start thread
				std::thread physics(physFunc, 1);
				// Do your own stuff
				logicFunc(1);
				onFrame();
				// Wait for thread to be done processing
				physics.join();
				// Render code BEGIN

				onDraw();
				// Render code END
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

		/// Gets called whenever the program is rendering to the screen. Happens last.
		Event::Signal	onDraw	= $func() {};


		/// Gets called every frame, along all other logic.
		Event::Signal	onFrame = $func() {};


		/// Gets called when the program is closing. Happens before Window is terminated.
		Event::Signal	onClose = $func() {};

		/// The program's window output.
		struct {
		} out;

		/// The window's color.
		VecV4 windowColor = VecV4(.5, .5, .5, 1.0);

		/// The program's input manager.
		InputManager input;
	private:
		/// Properly finishes program execution.
		void terminate() {
			// Call final function
			onClose();
		}
		/// Current execution state.
		bool shouldRun = true;

		/// The program's window.
		SDL_Window* window;
	};
}

#endif // MAKAILIB_MAKAI_H
