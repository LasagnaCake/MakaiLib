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
		std::runtime_error,
		Vector::VecV2,
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

		void update() {
			// Get keyboard state
			int keyCount = 0;
			const unsigned char* state = SDL_GetKeyboardState(&keyCount);
			for (auto i = 0; i < keyCount; i++) {
				// Jankify
				SDL_Scancode button = (SDL_Scancode)i;
				// Get previous key state
				unsigned int buttonState = 0;
				if (buffer[button]) buttonState = buffer[button];
				// If button is pressed...
				if(state[button]) {
					// If buffer not overflowing, increment buffer
					if(buttonState < 0xffff) buttonState++;
				}
				// Else, zero state
				else buttonState = 0;
				// Set buffer to button state
				buffer[button] = buttonState;
			}
		}

		/// Returns whether the button is pressed.
		inline bool getButtonDown(SDL_Scancode button) {
			return buffer[button] > 0;
		}

		/**
		* Returns the button's state.
		* 0		= Released;
		* 1+	= Pressed;
		* Recommended if time pressed is required.
		*/
		inline unsigned int getButtonState(SDL_Scancode button) {
			return buffer[button];

		}
		/// Whether input is enabled.
		bool enabled = true;
	private:
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
			string windowTitle,
			unsigned int fps = 60,
			bool fullscreen = false
		) {
			$debug("Starting SDL...");
			// Initialize SDL
			SDL_Init(SDL_INIT_VIDEO);
			$debug("Started!");
			$debug("Creating window...");
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
			$debug("Created!");
			$debug("Starting GLEW...");
			// Try and initialize GLEW
			GLenum glew_status = glewInit();
			if (glew_status != GLEW_OK) {
				$errlog("Error: glewInit: " << glewGetErrorString(glew_status));
				throw runtime_error(string("Error: glewInit"));
			}
			if (!GLEW_VERSION_2_0) {
				$errlog("Your computer does not support OpenGL 2+!");
				throw runtime_error(string("Error: No OpenGL 2+"));
			}
			$debug("Started!");
			// Create default shader
			$debug("Creating default shader...");
			Shader::defaultShader.create();
			$debug("Created!");
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		/// Sets the window's title.
		void setWindowTitle(string windowTitle) {
		}

		void setAlphaBlendMode(GLEnum type) {
			glBlendFunc(GL_SRC_ALPHA, type);
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
				input.update();
				// Start thread
				std::thread physics(physFunc, 1);
				// Do your own stuff
				logicFunc(1);
				onFrame();
				// Wait for thread to be done processing
				physics.join();
				// [[ Render code BEGIN ]]
				render();
				// [[ Render code END ]]
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
		void setWindowSize(VecV2 size){

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

		/// The window's clear color.
		VecV4 color = VecV4(0, 0, 0, 1);

		/// The program's input manager.
		InputManager input;
	private:
		/// Properly finishes program execution.
		void terminate() {
			// Call final function
			onClose();
			//
			// Quit SDL
			SDL_Quit();
		}

		/// Draws the window.
		void render() {
			// Clear screen
			glClearColor(color.x, color.y, color.z, color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			// Execute own drawing function
			onDraw();
			// Display window
			SDL_GL_SwapWindow(window);
		}

		/// Current execution state.
		bool shouldRun = true;

		/// The program's window.
		SDL_Window* window;
	};
}

#endif // MAKAILIB_MAKAI_H
