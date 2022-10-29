#ifndef MAKAILIB_MAKAI_LIBRARY_H
#define MAKAILIB_MAKAI_LIBRARY_H

#include "../anchors.hpp"
#include "../graphical.hpp"

#include <wtypes.h>

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

	SDL_Event pollEvents() {
		SDL_Event ev;
		SDL_PollEvent(&ev);
		return ev;
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
			if (!enabled) return false;
			return buffer[button] > 0;
		}

		/**
		* Returns the button's state.
		* 0		= Released;
		* 1+	= Pressed;
		* Recommended if time pressed is required.
		*/
		inline unsigned int getButtonState(SDL_Scancode button) {
			if (!enabled) return 0;
			return buffer[button];
		}

		/// Returns if the button has just been pressed (state == 1).
		inline bool isButtonJustPressed(SDL_Scancode button) {
			if (!enabled) return false;
			return getButtonState(button) == 1;
		}

		/// Returns if the button is held (state > threshold).
		inline bool isButtonHeld(SDL_Scancode button) {
			if (!enabled) return false;
			return getButtonState(button) > threshold;
		}

		/// Whether input is enabled.
		bool enabled = true;

		/// How long before the input is considered "held".
		unsigned int threshold = 2048;
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
			bool fullscreen = false,
			string bufferShaderPath = "shaders/framebuffer/compose.slf"
		) {
			// Save window resolution
			this->width = width;
			this->height = height;
			// Initialize SDL
			$debug("Starting SDL...");
			if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
				$errlog(string("Unable to start SDL! (") + SDL_GetError() + ")");
				throw runtime_error(string("Error: SDL (") + SDL_GetError() + ")");
			}
			TTF_Init();
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
			renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
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
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 8);
			glEnable(GL_BLEND);
			//glViewport(0, 0, width, height);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			// This keeps the alpha from shitting itself
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			//glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
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
			bufferShader.create(SLF::parseFile(bufferShaderPath));
			framebuffer.shader = bufferShader;
			layerbuffer.shader = bufferShader;
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
			// The physics process
			auto physFunc	= [&](float delta)-> void {
				EntityClass::$_ROOT.yield(delta);
			};
			// The logical process
			auto logicFunc	= [&](float delta)-> void {
				Tween::yieldAllTweens();
				Event::yieldAllTimers();
			};
			// Clear screen
			Drawer::clearColorBuffer(color);
			glClear(GL_DEPTH_BUFFER_BIT);
			// Render reserved layer
			renderReservedLayer();
			// Render simple frame
			SDL_GL_SwapWindow(window);
			// Call on open function
			onOpen();
			// SDL's events
			SDL_Event event;
			// Time recorded last frame
			float lastTime = (float)SDL_GetTicks();
			// While program is running...
			while(shouldRun) {
				// Get delta
				float curTime = (float)SDL_GetTicks();
				float delta = curTime - lastTime;
				float deltaTime = 1.0/maxFrameRate;
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
				taskers.yield();
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
				$ecl destroyQueued();
				// If running faster than expected, slow down
				// TODO: fix this
				/*if (delta < maxFrameRate)
					SDL_Delay((maxFrameRate - delta) * 1000);*/
			}
			// Terminate program
			$debug("Closing incoherent program...");
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
		size_t getCurrentFrame() {
			return frame;
		}

		inline void renderReservedLayer() {
			Drawer::clearColorBuffer(color);
			glClear(GL_DEPTH_BUFFER_BIT);
			//framebuffer()->clearBuffers();
			Drawer::renderLayer(Math::maxSizeT);
			//framebuffer.render(toFrameBufferData());
			SDL_GL_SwapWindow(window);
		}

		inline Drawer::FrameBuffer& getFrameBuffer() {
			return framebuffer;
		}

		inline Drawer::FrameBuffer& getLayerBuffer() {
			return layerbuffer;
		}

		inline Shader::Shader& getBufferShader() {
			return bufferShader;
		}

		inline Vector2 getWindowSize() {
			return Vector2(width, height);
		}

		inline Vector2 getWindowScale() {
			return getWindowSize().normalized();
		}

		/// Gets called when the program is opened.
		virtual void onOpen() {};

		/// Gets called whenever the program is rendering to the screen.
		/// Happens before any object is rendered.
		virtual void onDrawBegin()		{};
		virtual void onPreFrameDraw()	{};
		/// Happens after all objects are rendered.
		virtual void onDrawEnd()		{};

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

		Tasking::MultiTasker taskers;

	protected:
		Drawer::FrameBufferData toFrameBufferData() {
			return Drawer::FrameBufferData{0};
		}

	private:
		Shader::Shader bufferShader;

		/// The program's main framebuffer.
		Drawer::FrameBuffer framebuffer;
		Drawer::FrameBuffer layerbuffer;

		/// The window's resolution.
		unsigned int width, height;

		/// Properly finishes program execution.
		void terminate() {
			// Call final function
			onClose();
			// Destroy buffers
			$debug("Destroying buffers...");
			framebuffer.destroy();
			layerbuffer.destroy();
			$debug("Buffers destroyed!");
			// Quit SDL
			$debug("Ending SDL...");
			SDL_Quit();
			TTF_Quit();
			$debug("SDL ended!");
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
			framebuffer()->clearBuffers();
			// Draw objects
			vector<size_t> rLayers = Drawer::layers.getAllGroups();
			for (auto layer : rLayers) {
				if (layer == Math::maxSizeT) continue;
				// Call onLayerDrawBegin function
				onLayerDrawBegin(layer);
				// Clear target depth buffer
				framebuffer();
				// Enable layer buffer
				layerbuffer()->clearBuffers();
				// Render layer
				Drawer::renderLayer(layer);
				// Render layer buffer
				layerbuffer.render(framebuffer);
				// Call onLayerDrawEnd function
				onLayerDrawEnd(layer);
			}
			// ?????
			onPreFrameDraw();
			// Render frame buffer
			framebuffer.render(toFrameBufferData());
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

		/// The Window's renderer.
		SDL_Renderer* renderer;
	};

	Vector2 getDeviceSize() {
		RECT desktop;
		// Get a handle to the desktop window
		const HWND hDesktop = GetDesktopWindow();
		// Get the size of screen to the variable desktop
		GetWindowRect(hDesktop, &desktop);
		// The top left corner will have coordinates (0,0)
		// and the bottom right corner will have coordinates
		// (horizontal, vertical)
		return Vector2(desktop.right, desktop.bottom);
	}
}

namespace Popup {
	namespace {
		const SDL_MessageBoxColorScheme defaultMessageBoxColorScheme = {
			{ /* .colors (.r, .g, .b) */
				/* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
				{ 255,   0,   0 },
				/* [SDL_MESSAGEBOX_COLOR_TEXT] */
				{   0, 255,   0 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
				{ 255, 255,   0 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
				{   0,   0, 255 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
				{ 255,   0, 255 }
			}
		};
	}

	namespace Option {
		const StringList OK				= {"Ok"};
		const StringList YES			= {"Yes"};
		const StringList YES_NO			= {"Yes", "No"};
		const StringList YES_NO_CANCEL	= {"Yes", "No", "Cancel"};
	}

	/**
	* Invokes a dialog box with a given number of buttons.
	* Returns the selected button (by index);
	*/
	int dialogBox(
		String title,
		String text,
		StringList options,
		Uint32 type = SDL_MESSAGEBOX_INFORMATION,
		SDL_Window* window = NULL,
		SDL_MessageBoxColorScheme colorScheme = defaultMessageBoxColorScheme
	) {
		size_t buttonCount = options.size();
		SDL_MessageBoxButtonData buttons[options.size()];
		size_t idx = buttonCount - 1;
		for $each(b, buttons) {
			b = SDL_MessageBoxButtonData {
				0,
				idx,
				options[idx].c_str()
			};
			idx--;
		}
		const SDL_MessageBoxData messageboxdata = {
			type, /* .flags */
			window, /* .window */
			title.c_str(), /* .title */
			text.c_str(), /* .message */
			SDL_arraysize(buttons), /* .numbuttons */
			buttons, /* .buttons */
			&colorScheme /* .colorScheme */
		};
		int buttonid;
		if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
			$errlog("Could not show messagebox!");
			throw std::runtime_error("Error: SDL MessageBox");
		}
		return buttonid;
	}
}

#define $mki Makai::

#define $event(EVENT) ($mki pollEvents().type == EVENT)

#endif // MAKAILIB_MAKAI_H
