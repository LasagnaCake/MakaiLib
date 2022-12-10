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
		std::unordered_map,
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
		unordered_map<SDL_Scancode, unsigned int> buffer;
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
			string bufferShaderPath = "shaders/framebuffer/compose.slf",
			bool useMIDI = false
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
			if (TTF_Init() != 0) {
				$errlog(string("Unable to start TTF! (") + TTF_GetError() + ")");
				throw runtime_error(string("Error: TTF (") + TTF_GetError() + ")");
			};
			if (!Mix_Init(MIX_INIT_OGG | (useMIDI ? MIX_INIT_MID : 0))) {
				$errlog(string("Unable to start Mixer! (") + Mix_GetError() + ")");
				throw runtime_error(string("Error: Mixer (") + Mix_GetError() + ")");
			}
			$debug("Started!");
			$debug("Creating window...");
			// Create window and make active
			window = SDL_CreateWindow(
				windowTitle.c_str(),
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				width,
				height,
				SDL_WINDOW_OPENGL
			);
			SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			SDL_GL_CreateContext(window);
			$debug("Created!");
			$debug("Starting GLEW...");
			// Try and initialize GLEW
			GLenum glew_status = glewInit();
			if (glew_status != GLEW_OK) {
				$errlog("Error: glewInit: " << glewGetErrorString(glew_status));
				throw runtime_error(string("Error: glewInit"));
			}
			if (!GLEW_VERSION_4_2) {
				$errlog("Your computer does not support OpenGL 4.2+!");
				throw runtime_error(string("Error: No OpenGL 4.2+"));
			}
			$debug("Started!");
			// Create default shader
			/*$debug("Creating default shader...");
			Shader::defaultShader.create();
			$debug("Created!");*/
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
			// The timer process
			auto timerFunc	= [&](float delta)-> void {
				Tween::yieldAllTweens(1.0);
				Event::yieldAllTimers(1.0);
			};
			// The logical process
			auto logicFunc	= [&](float delta)-> void {
				EntityClass::$_ROOT.yield(delta);
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
			// Delta time
			float frameDelta = 1.0/maxFrameRate;
			float cycleDelta = 1.0/maxCycleRate;
			// Last time of execution
			size_t frameTicks = SDL_GetTicks() + frameDelta * 1000;
			size_t cycleTicks = SDL_GetTicks() + cycleDelta * 1000;
			// While program is running...
			while(shouldRun) {
				// Poll events and check if should close
				while (SDL_PollEvent(&event))
					if (event.type == SDL_QUIT)
						shouldRun = false;
				// Get deltas
				frameDelta = 1.0/maxFrameRate;
				cycleDelta = 1.0/maxCycleRate;
				// If should process, then do so
				if (SDL_GetTicks() - cycleTicks > cycleDelta * 1000) {
					// Update input manager
					input.update();
					// Get current time
					cycleTicks = SDL_GetTicks();
					// Do timer-related stuff
					timerFunc(cycleDelta);
					taskers.yield(cycleDelta);
					#ifdef $_FRAME_INDEPENDENT_PROCESS
					// Do normal logic-related stuff
					logicFunc(cycleDelta);
					onLogicFrame(cycleDelta);
					#endif // FRAME_DEPENDENT_PROCESS
				}
				if (SDL_GetTicks() - frameTicks > frameDelta * 1000) {
					// Get current time
					frameTicks = SDL_GetTicks();
					// increment frame counter
					frame += 1;
					#ifndef $_FRAME_INDEPENDENT_PROCESS
					// Do normal logic-related stuff
					logicFunc(frameDelta);
					onLogicFrame(frameDelta);
					#endif // FRAME_DEPENDENT_PROCESS
					// Render screen
					render();
					// Destroy queued entities
					$ecl destroyQueued();
				}
			}
			// Terminate program
			$debug("Closing incoherent program...");
			terminate();
		}

		void setFullscreen(bool state = false) {
			SDL_SetWindowFullscreen(window, state);
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
		/// Gets called when the program starts rendering a layer.
		/// Happens before the layer is cleared.
		virtual void onLayerDrawBegin(size_t layerID) {};
		/// Gets called when the program ends rendering a layer.
		virtual void onLayerDrawEnd(size_t layerID) {};
		/// Happens before the screen is rendered.
		virtual void onPreFrameDraw()	{};
		/// Happens after all objects are rendered.
		virtual void onDrawEnd()		{};

		/// Gets called every frame, along all other logic.
		virtual void onLogicFrame(float delta)	{};

		/// Gets called when the program is closing. Happens before Window is terminated.
		virtual void onClose()	{};

		/// The program's window output.
		struct {
		} out;

		/// The window's clear color.
		Vector4 color = Color::BLACK;

		/// The program's input manager.
		InputManager input;

		/// The program's maximum framerate.
		float maxFrameRate = 30.0;

		/// The program's maximum "cycles per second" count.
		float maxCycleRate = 60;


		/// The program's taskers.
		Tasking::MultiTasker taskers;

		/// Gets the window's renderer.
		inline SDL_Renderer* getRenderer() {
			return renderer;
		}

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
			Mix_Quit();
			$debug("SDL ended!");
			//exit(0);
		}

		/// Draws the window.
		void render() {
			// Clear screen
			Drawer::clearColorBuffer(color);
			glClear(GL_DEPTH_BUFFER_BIT);
			// Enable depth testing
			glEnable(GL_DEPTH_TEST);
			// Enable & clear frame buffer
			framebuffer()->clearBuffers();
			// Call rendering start function
			onDrawBegin();
			// Draw objects
			vector<size_t> rLayers = Drawer::layers.getAllGroups();
			for (auto layer : rLayers) {
				if (layer == Math::maxSizeT) continue;
				// Clear target depth buffer
				framebuffer();
				// Call onLayerDrawBegin function
				onLayerDrawBegin(layer);
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
			// Display window
			SDL_GL_SwapWindow(window);
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

	Vector2 getDeviceSize(unsigned int display = 0) {
		SDL_Rect bounds;
		$debug("Starting SDL...");
		if (!SDL_WasInit(SDL_INIT_VIDEO))
			if (SDL_Init(SDL_INIT_VIDEO) != 0) {
				$errlog(string("Unable to start SDL! (") + SDL_GetError() + ")");
				throw runtime_error(string("Error: SDL (") + SDL_GetError() + ")");
			}
		if (SDL_GetDisplayBounds(display, &bounds))
			throw std::runtime_error("Couldn't get display bounds!\n\n" + String(SDL_GetError()));
		return Vector2(bounds.h - bounds.x, bounds.w - bounds.y);
	}

	namespace {
		using Dictionary::Entry;
	}

	namespace Resolution {
		#define RESOLUTION(W, H) Entry<string, Vector2>{#W" × "#H , Vector2(W, H)}
		const vector<Entry<string, Vector2>> set4x3 = {
			RESOLUTION(480, 320),
			RESOLUTION(640, 480),
			RESOLUTION(860, 640),
			RESOLUTION(960, 720),
			RESOLUTION(1280, 960),
			RESOLUTION(1600, 1200)
		};

		const vector<Entry<string, Vector2>> set16x9 = {
			RESOLUTION(640, 360),
			RESOLUTION(854, 480),
			RESOLUTION(1280, 720),
			RESOLUTION(1600, 900),
			RESOLUTION(1920, 1080),
			RESOLUTION(2560, 1440)
		};
		#undef RESOLUTION
	}
	#define $mki Makai::
	#define $res $mki Resolution::
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

	void errorDialog(std::string what) {
		//PlaySound("SystemExclamation", NULL, SND_ASYNC);
		Popup::dialogBox(
			"Error!",
			what,
			Popup::Option::OK,
			SDL_MESSAGEBOX_ERROR
		);
	}
}

#define $event(EVENT) ($mki pollEvents().type == EVENT)

#endif // MAKAILIB_MAKAI_H
