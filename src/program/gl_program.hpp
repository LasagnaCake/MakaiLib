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
			bool fullscreen = false,
			string composeShaderPath = "shaders/framebuffer/compose.slf"
		) {
			// Save window resolution
			this->width = width;
			this->height = height;
			// Initialize SDL
			$debug("Starting SDL...");
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
			SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			// Setup camera
			$debug("Setting starting camera...");
			Scene::camera.aspect = Vector2(width, height);
			Scene::camera.fov = glm::radians(45.0f);
			$debug("creating default framebuffer...");
			// Create framebuffer
			framebuffer.create(width, height);
			layerbuffer.create(width, height);
			// Create composition shader
			$debug("Creating composition shader...");
			compose.create(SLF::parseFile(composeShaderPath));
			framebuffer.shader = &compose;
			layerbuffer.shader = &compose;
			Shader::defaultShader["textured"](false);
		}

		/// Sets the window's title.
		inline void setWindowTitle(string windowTitle) {
			SDL_SetWindowTitle(window, windowTitle.c_str());
		}

		inline void setAlphaBlendMode(GLuint type) {
			glBlendFunc(GL_SRC_ALPHA, type);
		}

		/// Runs the program.
		void run(){
			// Call on open function
			onOpen();
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
			// Time recorded last frame
			float lastTime = (float)SDL_GetTicks();
			// While program is running...
			while(shouldRun) {
				// Get delta
				float curTime = (float)SDL_GetTicks();
				float delta = curTime - lastTime;
				float deltaTime = delta/maxFrameRate;
				// increment frame counter
				frame += 1;
				// Poll events and check if should close
				while (SDL_PollEvent(&event))
					if (event.type == SDL_QUIT)
						shouldRun = false;
				// Update input manager
				input.update();
				// Start thread
				std::thread physics(physFunc, deltaTime);
				// Do your own stuff
				logicFunc(deltaTime);
				onLogicFrame();
				// Wait for thread to be done processing
				physics.join();
				// [[ Render code BEGIN ]]
				// Render screen
				render();
				// Display window
				SDL_GL_SwapWindow(window);
				// [[ Render code END ]]
				// Get time
				lastTime = (float)SDL_GetTicks() * 1.0;
				// Destroy queued entities
				EntityClass::destroyQueued();
				// If running faster than expected, slow down
				if (delta < maxFrameRate)
					SDL_Delay((1.0 - deltaTime) * 1000.0 / maxFrameRate);
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

		/// Gets the current frame.
		size_t getFrameCounter() {
			return frame;
		}

		inline Drawer::FrameBuffer& getFrameBuffer() {
			return framebuffer;
		}

		inline Drawer::FrameBuffer& getLayerBuffer() {
			return layerbuffer;
		}

		inline Vector2 getScreenSize() {
			return Vector2(width, height);
		}

		/// Gets called when the program is opened.
		virtual void onOpen() {};

		/// Gets called whenever the program is rendering to the screen.
		/// Happens before any object is rendered.
		virtual void onDrawBegin()	{};
		/// Happens after all objects are rendered.
		virtual void onDrawEnd()	{};

		/// Gets called every frame, along all other logic.
		virtual void onLogicFrame()	{};

		/// Gets called when the program is closing. Happens before Window is terminated.
		virtual void onClose()	{};

		/// Gets called when the program starts rendering a layer.
		virtual void onLayerDrawBegin(size_t layerID) {};
		virtual void onLayerDrawEnd(size_t layerID) {};

		/// The program's window output.
		struct {
		} out;

		/// The window's clear color.
		Vector4 color = Color::BLACK;

		/// The program's input manager.
		InputManager input;

		/// The program's maximum framerate
		float maxFrameRate = 60.0;

	protected:
		Drawer::FrameBufferData getFBData() {
			return Drawer::FrameBufferData{0, color, width, height};
		}

	private:
		/// The program's compose shader.
		Shader::Shader compose;

		/// The program's main framebuffer.
		Drawer::FrameBuffer framebuffer;
		Drawer::FrameBuffer layerbuffer;

		/// The window's resolution.
		unsigned int width, height;

		/// Properly finishes program execution.
		void terminate() {
			// Call final function
			onClose();
			// Destroy framebuffers
			compose.destroy();
			framebuffer.destroy();
			// Quit SDL
			SDL_Quit();
		}

		/// Draws the window.
		void render() {
			// Clear screen
			Drawer::clearColorBuffer(color);
			glClear(GL_DEPTH_BUFFER_BIT);
			// Enable depth testing
			glEnable(GL_DEPTH_TEST);
			// Call rendering start function
			onDrawBegin();
			// Enable & clear frame buffer
			framebuffer()->clearBuffer();
			// Draw objects
			vector<size_t> rLayers = Drawer::layers.getAllGroups();
			for (auto layer : rLayers) {
				// Call onLayerDrawBegin function
				onLayerDrawBegin(layer);
				// Enable layer buffer
				layerbuffer()->clearBuffer();
				// Render layer
				Drawer::renderLayer(layer);
				// Render layer buffer
				layerbuffer.render(framebuffer);
				// Call onLayerDrawEnd function
				onLayerDrawEnd(layer);
			}
			// Render frame buffer
			framebuffer.render(getFBData());
			// Call rendering end function
			onDrawEnd();
			// Disable depth testing
			glDisable(GL_DEPTH_TEST);
		}
		/// Frame counter.
		size_t frame = 0;

		/// Current execution state.
		bool shouldRun = true;

		/// The program's window.
		SDL_Window* window;
	};
}

#endif // MAKAILIB_MAKAI_H
