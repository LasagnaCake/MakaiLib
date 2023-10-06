#ifndef MAKAILIB_MAKAI_LIBRARY_H
#define MAKAILIB_MAKAI_LIBRARY_H

#include "../anchors.hpp"
#include "../graphical.hpp"
#include "../audio.hpp"

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <winuser.h>
#endif

#ifndef AUDIO_SAMPLE_FRAMES
#define AUDIO_SAMPLE_FRAMES 2048
#endif // AUDIO_SAMPLE_FRAMES

namespace Makai {
	namespace {
		using
		std::vector,
		std::function,
		std::string,
		std::unordered_map,
		Vector::Vector2,
		Vector::Vector3,
		Vector::Vector4;
	}

	SDL_Event pollEvents() {
		SDL_Event ev;
		SDL_PollEvent(&ev);
		return ev;
	}

	struct MouseState {
		int x, y;
	};

	struct MouseData {
		MouseState pos, last;
	};

	enum class MouseScancode {
		MS_LEFT,
		MS_MIDDLE,
		MS_RIGHT,
		MS_UNKNOWN
	};

	/**
	*************************
	*                       *
	*  Event Manager class  *
	*                       *
	*************************

	* A button has three states: "Unpressed", "Pressed" and "Held".
	*	- "Unpressed" is the default state of the button.
	*	- "Pressed" is when the button is (and/or continues to be) pressed.
	*	- "Held" is when the button has been kept pressed for a certain
	*	  amount of time, as set via the "threshold" variable.
	*/
	struct InputManager {
		/// Inclusions
		using enum MouseScancode;

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
				// Copy previous state to secondary buffer
				last[button]	= buffer[button];
				// Set buffer to button state
				buffer[button]	= buttonState;
			}
			// Get mouse structures
			MouseData&
				lm = mouse.local,
				gm = mouse.global
			;
			// Save last positional data
			lm.last = lm.pos;
			gm.last = gm.pos;
			// Get position & button data
			uint32 btn = SDL_GetMouseState(&lm.pos.x, &lm.pos.y);
			SDL_GetGlobalMouseState(&gm.pos.x, &gm.pos.y);
			// Update button data
			for (auto i = 0; i < (((int)MS_UNKNOWN)-1); i++) {
				// Jankify
				MouseScancode button = (MouseScancode)i;
				// Get previous button state
				unsigned int buttonState = 0;
				if (mouse.buffer[button]) buttonState = mouse.buffer[button];
				// If button is pressed.
				if(btn & SDL_BUTTON(i)) {
					// If buffer not overflowing, increment buffer
					if(buttonState < 0xffff) buttonState++;
				}
				// Else, zero state
				else buttonState = 0;
				// Copy previous state to secondary buffer
				mouse.last[button]		= mouse.buffer[button];
				// Set buffer to button state
				mouse.buffer[button]	= buttonState;
			}
		}

		/**
		* Returns the button's state.
		* 0		= Unpressed;
		* 1+	= Pressed;
		* Recommended if time pressed is required.
		*/
		inline unsigned int getButtonState(SDL_Scancode button) {
			if (!enabled || button == SDL_SCANCODE_UNKNOWN) return 0;
			return buffer[button];
		}

		/// Returns whether the button is pressed.
		inline bool isButtonDown(SDL_Scancode button) {
			if (!enabled || button == SDL_SCANCODE_UNKNOWN) return false;
			return buffer[button] > 0;
		}

		/// Returns if the button has just been pressed (state == 1).
		inline bool isButtonJustPressed(SDL_Scancode button) {
			if (!enabled || button == SDL_SCANCODE_UNKNOWN) return false;
			return getButtonState(button) == 1;
		}

		/// Returns if the button has just been released.
		inline bool isButtonJustReleased(SDL_Scancode button) {
			if (!enabled || button == SDL_SCANCODE_UNKNOWN) return false;
			return (isButtonChanged(button) && (getButtonState(button) == 0));
		}

		/// Returns if the button is held (state > threshold).
		inline bool isButtonHeld(SDL_Scancode button) {
			if (!enabled || button == SDL_SCANCODE_UNKNOWN) return false;
			return getButtonState(button) > threshold;
		}

		/// Returns if the button's state has changed.
		inline bool isButtonChanged(SDL_Scancode button) {
			if (!enabled || button == SDL_SCANCODE_UNKNOWN) return false;
			return last[button]	== buffer[button];
		}

		/// Returns the button that was most recently pressed.
		inline SDL_Scancode mostRecentButtonDown() {
			SDL_Scancode button		= SDL_SCANCODE_UNKNOWN;
			unsigned int duration	= Math::Max::UINT_V;
			for (auto [b, d] : buffer)
				if ((d) && d < duration) {
					button		= b;
					duration	= d;
				}
			return button;
		}

		/// Returns the button that was most recently just pressed.
		inline SDL_Scancode mostRecentButtonJustPressed() {
			for (auto [b, d] : buffer)
				if (d == 1)
					return b;
			return SDL_SCANCODE_UNKNOWN;
		}

		/// Returns the button that was most recently held.
		inline SDL_Scancode mostRecentButtonHeld() {
			SDL_Scancode button		= SDL_SCANCODE_UNKNOWN;
			unsigned int duration	= Math::Max::UINT_V;
			for (auto [b, d] : buffer)
				if (d > threshold && d < duration) {
					button		= b;
					duration	= d;
				}
			return button;
		}

		/// Returns the button that was most recently changed.
		inline SDL_Scancode mostRecentButtonChanged() {
			for (auto [b, d] : buffer)
				if (d != last[b])
					return b;
			return SDL_SCANCODE_UNKNOWN;
		}

		/// Returns the button that was most recently just released.
		inline SDL_Scancode mostRecentButtonJustReleased() {
			for (auto [b, d] : buffer)
				if (d != last[b] && d == 0)
					return b;
			return SDL_SCANCODE_UNKNOWN;
		}

		/// Returns all buttons currently pressed.
		inline vector<SDL_Scancode> getButtonsDown() {
			vector<SDL_Scancode> buttons;
			for (auto [b, d] : buffer)
				if (isButtonDown(b))
					buttons.push_back(b);
			return buttons;
		}

		/// Returns all buttons currently just pressed.
		inline vector<SDL_Scancode> getButtonsJustPressed() {
			vector<SDL_Scancode> buttons;
			for (auto [b, d] : buffer)
				if (isButtonJustPressed(b))
					buttons.push_back(b);
			return buttons;
		}

		/// Returns all buttons currently just released.
		inline vector<SDL_Scancode> getButtonsJustReleased() {
			vector<SDL_Scancode> buttons;
			for (auto [b, d] : buffer)
				if (isButtonJustReleased(b))
					buttons.push_back(b);
			return buttons;
		}

		/// Returns all buttons whose state changed.
		inline vector<SDL_Scancode> getButtonsChanged() {
			vector<SDL_Scancode> buttons;
			for (auto [b, d] : buffer)
				if (isButtonChanged(b))
					buttons.push_back(b);
			return buttons;
		}

		/// Returns all buttons currently held.
		inline vector<SDL_Scancode> getButtonsHeld() {
			vector<SDL_Scancode> buttons;
			for (auto [b, d] : buffer)
				if (isButtonHeld(b))
					buttons.push_back(b);
			return buttons;
		}

		/// Returns the mouse position relative to the window.
		inline Vector2 getWindowMousePosition() {
			return Vector2(mouse.local.pos.x, mouse.local.pos.y);
		}

		/// Returns the mouse position relative to the desktop.
		inline Vector2 getDesktopMousePosition() {
			return Vector2(mouse.global.pos.x, mouse.global.pos.y);
		}

		/// Returns which direction the mouse is moving towards.
		inline Vector2 getMouseDirection() {
			return getDesktopMousePosition() - Vector2(mouse.global.last.x, mouse.global.last.y);
		}

		/// Enables/Disables mouse capturing.
		inline void setMouseCapturing(bool enabled = false) {
			SDL_CaptureMouse(enabled ? SDL_TRUE : SDL_FALSE);
		}

		/**
		* Returns the mouse button's state.
		* 0		= Unpressed;
		* 1+	= Pressed;
		* Recommended if time pressed is required.
		*/
		inline unsigned int getButtonState(MouseScancode button) {
			if (!enabled || button == MS_UNKNOWN) return 0;
			return mouse.buffer[button];
		}

		/// Returns whether the mouse button is pressed.
		inline bool isButtonDown(MouseScancode button) {
			if (!enabled || button == MS_UNKNOWN) return false;
			return mouse.buffer[button] > 0;
		}

		/// Returns if the mouse button has just been pressed (state == 1).
		inline bool isButtonJustPressed(MouseScancode button) {
			if (!enabled || button == MS_UNKNOWN) return false;
			return getButtonState(button) == 1;
		}

		/// Returns if the mouse button has just been released.
		inline bool isButtonJustReleased(MouseScancode button) {
			if (!enabled || button == MS_UNKNOWN) return false;
			return (isButtonChanged(button) && (getButtonState(button) == 0));
		}

		/// Returns if the mouse button is held (state > threshold).
		inline bool isButtonHeld(MouseScancode button) {
			if (!enabled || button == MS_UNKNOWN) return false;
			return getButtonState(button) > threshold;
		}

		/// Returns if the mouse button's state has changed.
		inline bool isButtonChanged(MouseScancode button) {
			if (!enabled || button == MS_UNKNOWN) return false;
			return mouse.last[button] == mouse.buffer[button];
		}

		/// Returns the mouse button that was most recently pressed.
		inline MouseScancode mostRecentMouseButtonDown() {
			MouseScancode button	= MS_UNKNOWN;
			unsigned int duration	= Math::Max::UINT_V;
			for (auto [b, d] : mouse.buffer)
				if ((d) && d < duration) {
					button		= b;
					duration	= d;
				}
			return button;
		}

		/// Returns the mouse button that was most recently just pressed.
		inline MouseScancode mostRecentMouseButtonJustPressed() {
			for (auto [b, d] : mouse.buffer)
				if (d == 1)
					return b;
			return MS_UNKNOWN;
		}

		/// Returns the mouse button that was most recently held.
		inline MouseScancode mostRecentMouseButtonHeld() {
			MouseScancode button	= MS_UNKNOWN;
			unsigned int duration	= Math::Max::UINT_V;
			for (auto [b, d] : mouse.buffer)
				if (d > threshold && d < duration) {
					button		= b;
					duration	= d;
				}
			return button;
		}

		/// Returns the mouse button that was most recently changed.
		inline MouseScancode mostRecentMouseButtonChanged() {
			for (auto [b, d] : mouse.buffer)
				if (d != mouse.last[b])
					return b;
			return MS_UNKNOWN;
		}

		/// Returns the mouse button that was most recently just released.
		inline MouseScancode mostRecentMouseButtonJustReleased() {
			for (auto [b, d] : mouse.buffer)
				if (d != mouse.last[b] && d == 0)
					return b;
			return MS_UNKNOWN;
		}

		/// Returns all mouse buttons currently pressed.
		inline vector<MouseScancode> getMouseButtonsDown() {
			vector<MouseScancode> buttons;
			for (auto [b, d] : mouse.buffer)
				if (isButtonDown(b))
					buttons.push_back(b);
			return buttons;
		}

		/// Returns all mouse buttons currently just pressed.
		inline vector<MouseScancode> getMouseButtonsJustPressed() {
			vector<MouseScancode> buttons;
			for (auto [b, d] : mouse.buffer)
				if (isButtonJustPressed(b))
					buttons.push_back(b);
			return buttons;
		}

		/// Returns all mouse buttons currently just released.
		inline vector<MouseScancode> getMouseButtonsJustReleased() {
			vector<MouseScancode> buttons;
			for (auto [b, d] : mouse.buffer)
				if (isButtonJustReleased(b))
					buttons.push_back(b);
			return buttons;
		}

		/// Returns all mouse buttons whose state changed.
		inline vector<MouseScancode> getMouseButtonsChanged() {
			vector<MouseScancode> buttons;
			for (auto [b, d] : mouse.buffer)
				if (isButtonChanged(b))
					buttons.push_back(b);
			return buttons;
		}

		/// Returns all mouse buttons currently held.
		inline vector<MouseScancode> getMouseButtonsHeld() {
			vector<MouseScancode> buttons;
			for (auto [b, d] : mouse.buffer)
				if (isButtonHeld(b))
					buttons.push_back(b);
			return buttons;
		}

		/// Whether input is enabled.
		bool enabled = true;

		/// How long before the input is considered "held".
		unsigned int threshold = 2048;
	private:
		/// The internal buffer state.
		unordered_map<SDL_Scancode, unsigned int> buffer, last;
		struct {
			MouseData global;
			MouseData local;
			unordered_map<MouseScancode, unsigned int> buffer, last;
		} mouse;
	};

	namespace {
		void GLAPIENTRY glAPIMessageCallback(
			GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam
		) {
			DEBUGLN(
				"[GL CALLBACK"
				+ String(type == GL_DEBUG_TYPE_ERROR ? " (GL ERROR)" : "") + "] "
				+ "Type: " + toString(type) + ", "
				+ "Severity: " + toString(severity) + ", "
				+ "Message: " + String(message) + ", "
			);
		}
	}

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
			bool useMIDI = false,
			string bufferShaderPath = "shaders/framebuffer/compose.slf",
			string mainShaderPath = "shaders/base/base.slf"
		) {
			#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
			SetProcessDPIAware();
			#endif
			DEBUGLN(EntityClass::_ROOT == nullptr);
			// Save window resolution
			this->width = width;
			this->height = height;
			// Initialize SDL
			DEBUGLN("Starting SDL...");
			if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
				ERR_LOG(string("Unable to start SDL! (") + SDL_GetError() + ")");
				throw Error::FailedAction(string("SDL (") + SDL_GetError() + ")");
			}
			DEBUGLN("Started!");
			// Initialize YSE
			DEBUGLN("Starting Audio System...");
			if (!Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG | (useMIDI ? MIX_INIT_MID : 0))) {
				ERR_LOG(string("Unable to start Mixer! (") + Mix_GetError() + ")");
				throw Error::FailedAction(string("Mixer (") + Mix_GetError() + ")");
			}
			Audio::openSystem();
			DEBUGLN("Started!");
			// Create window and make active
			DEBUGLN("Creating window...");
			window = SDL_CreateWindow(
				windowTitle.c_str(),
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				width,
				height,
				SDL_WINDOW_OPENGL
			);
			SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
			SDL_GL_CreateContext(window);
			DEBUGLN("Created!");
			DEBUGLN("Starting GLEW...");
			// Try and initialize GLEW
			GLenum glewStatus = glewInit();
			if (glewStatus != GLEW_OK) {
				ERR_LOG("Error: glewInit: ");
				ERR_LOG(glewGetErrorString(glewStatus));
				throw Error::FailedAction(string("glewInit: ") + (const char*)glewGetErrorString(glewStatus));
			}
			if (!GLEW_VERSION_4_2) {
				ERR_LOG("Your computer does not support OpenGL 4.2+!");
				throw Error::InvalidValue(string("No OpenGL 4.2+"));
			}
			DEBUGLN("Started!");
			// Create default shader
			/*DEBUGLN("Creating default shader...");
			Shader::defaultShader.create();
			DEBUGLN("Created!");*/
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 16);
			SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 16);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
			//glViewport(0, 0, width, height);
			// This keeps the alpha from shitting itself
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			#ifdef $_ENABLE_OPENGL_DEBUG
			glEnable(GL_DEBUG_OUTPUT);
			#endif
			glDebugMessageCallback(glAPIMessageCallback, 0);
			glBlendFuncSeparatei(0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			// This keeps the alpha from shitting itself
			glBlendEquationSeparatei(0, GL_FUNC_ADD, GL_MAX);
			//glEnable(GL_CULL_FACE);
			//glBlendFuncSeparatei(0, DEFAULT_BLEND_FUNC);
			//glBlendEquationSeparatei(0, DEFAULT_BLEND_EQUATION);
			//glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
			// Setup camera
			DEBUGLN("Setting starting camera...");
			Scene::camera.aspect = Vector2(width, height);
			Scene::camera.fov = glm::radians(45.0f);
			DEBUGLN("creating default framebuffer...");
			// Create framebuffer
			framebuffer.create(width, height);
			// Fix alpha being a bitch
			framebuffer();
			//glDisable(GL_BLEND);
			//unsigned int fid = framebuffer.toFrameBufferData().id;
			//glBlendEquationSeparatei(fid, GL_FUNC_ADD, GL_MAX);
			//glBlendFuncSeparatei(fid, GL_ONE, GL_ONE, GL_ONE, GL_ONE);
			//glBlendEquationSeparatei(fid, GL_FUNC_ADD, GL_FUNC_ADD);
			glBindBuffer(GL_FRAMEBUFFER, 0);
			// Create layer buffer
			layerbuffer.create(width, height);
			// Create composition shader
			DEBUGLN("Creating shaders...");
			bufferShader.create(SLF::parseFile(bufferShaderPath));
			framebuffer.shader = bufferShader;
			layerbuffer.shader = bufferShader;
			// Create main shader
			SLF::SLFData data = SLF::parseFile(mainShaderPath);
			MAIN_SHADER.destroy();
			MAIN_SHADER.create(data);
			DEBUGLN(EntityClass::_ROOT ? "Root Exists!" : "Root does not exist!");
			if (!EntityClass::_ROOT) {
				DEBUGLN("Initializing root tree...");
				EntityClass::init();
			}
			DEBUGLN("All core systems initialized!");
		}

		virtual ~Program() {}

		/// Sets the window's title.
		inline void setWindowTitle(string windowTitle) {
			SDL_SetWindowTitle(window, windowTitle.c_str());
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
				if (EntityClass::_ROOT)
					EntityClass::_ROOT->yield(delta);
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
			size_t frameTicks = SDL_GetTicks() + frameDelta * 1000.0;
			size_t cycleTicks = SDL_GetTicks() + cycleDelta * 1000.0;
			// While program is running...
			while(shouldRun) {
				// Poll events and check if should close
				while (SDL_PollEvent(&event))
					if (event.type == SDL_QUIT)
						shouldRun = false;
				// Get deltas
				frameDelta	= 1.0/maxFrameRate;
				cycleDelta	= 1.0/maxCycleRate;
				// Clamp speed
				speed = Math::clamp(speed, 0.0f, 1.0f);
				// Get rates
				cycleRate = SDL_GetTicks() - cycleTicks;
				frameRate = SDL_GetTicks() - frameTicks;
				// If should process, then do so
				#ifndef $_PROCESS_RENDER_BEFORE_LOGIC
				if (cycleRate > ((cycleDelta * 1000.0 - 1) / speed)) {
					// Update audio system
					Audio::updateAudioSystem();
					// Update input manager
					input.update();
					// Get current time
					cycleTicks = SDL_GetTicks();
					// Increment cycle counter
					cycle++;
					// Do timer-related stuff
					timerFunc(cycleDelta);
					taskers.yield(cycleDelta);
					#ifndef $_FRAME_DEPENDENT_PROCESS
					// Do normal logic-related stuff
					logicFunc(cycleDelta);
					onLogicFrame(cycleDelta);
					// Destroy queued entities
					EntityClass::destroyQueued();
					#endif // FRAME_DEPENDENT_PROCESS
				}
				#endif
				if (frameRate > (frameDelta * 1000 - 1)) {
					// Get current time
					frameTicks = SDL_GetTicks();
					// increment frame counter
					frame++;
					// Update audio system
					// TODO: Audio System
					#ifdef $_FRAME_DEPENDENT_PROCESS
					// Do normal logic-related stuff
					logicFunc(frameDelta);
					onLogicFrame(frameDelta);
					// Destroy queued entities
					EntityClass::destroyQueued();
					#endif // FRAME_DEPENDENT_PROCESS
					// Render screen
					render();
				}
				#ifdef $_PROCESS_RENDER_BEFORE_LOGIC
				if (SDL_GetTicks() - (((float)(cycleDelta * 1000) - 1) / speed)) {
					// Update audio system
					Audio::updateAudioSystem();
					// Update input manager
					input.update();
					// Get current time
					cycleTicks = SDL_GetTicks();
					// Increment cycle counter
					cycle++;
					// Do timer-related stuff
					timerFunc(cycleDelta);
					taskers.yield(cycleDelta);
					#ifndef $_FRAME_DEPENDENT_PROCESS
					// Do normal logic-related stuff
					logicFunc(cycleDelta);
					onLogicFrame(cycleDelta);
					// Destroy queued entities
					EntityClass::destroyQueued();
					#endif // FRAME_DEPENDENT_PROCESS
				}
				#endif
			}
			// Terminate program
			DEBUGLN("\nClosing incoherent program...");
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
		void setWindowSize(Vector2 size){

		}

		/// Gets the current frame.
		size_t getCurrentFrame() {
			return frame;
		}

		/// Gets the current cycle.
		size_t getCurrentCycle() {
			return cycle;
		}

		size_t getCycleRate() {
			return cycleRate;
		}

		size_t getFrameRate() {
			return frameRate;
		}

		inline void renderReservedLayer() {
			Drawer::clearColorBuffer(color);
			glClear(GL_DEPTH_BUFFER_BIT);
			//framebuffer()->clearBuffers();
			Drawer::renderLayer(Math::Max::SIZET_V);
			//framebuffer.render(toFrameBufferData());
			SDL_GL_SwapWindow(window);
		}

		constexpr inline Drawer::FrameBuffer& getFrameBuffer() {
			return framebuffer;
		}

		constexpr inline Drawer::FrameBuffer& getLayerBuffer() {
			return layerbuffer;
		}

		constexpr inline Shader::Shader& getBufferShader() {
			return bufferShader;
		}

		inline Vector2 getWindowSize() {
			return Vector2(width, height);
		}

		inline Vector2 getWindowScale() {
			Vector2 ws = getWindowSize();
			return Vector2(ws.x / ws.y, 1);
		}

		/// Gets called when the program is opened.
		virtual void onOpen() {};

		/// Gets called whenever the program is rendering to the screen.

		/// Happens before the screen is rendered, before the frame buffer is cleared.
		virtual void onDrawBegin()		{};
		/// Happens before the screen is rendered, after the frame buffer is cleared.
		virtual void onPostFrameClear()	{};
		/// Gets called when the program begins rendering a layer, before the the layer buffer is cleared.
		virtual void onLayerDrawBegin(size_t layerID)	{};
		/// Gets called when the program begins rendering a layer, after the the layer buffer is cleared.
		virtual void onPostLayerClear(size_t layerID)	{};
		/// Gets called when the program ends rendering a layer, before the layer buffer is drawn to the screen.
		virtual void onPreLayerDraw(size_t layerID)		{};
		/// Gets called when the program ends rendering a layer, after the layer buffer is drawn to the screen.
		virtual void onLayerDrawEnd(size_t layerID)		{};
		/// Happens after the screen is rendered, before the frame buffer is drawn to the screen.
		virtual void onPreFrameDraw()	{};
		/// Happens after the screen is rendered, after the frame buffer is drawn to the screen.
		virtual void onDrawEnd()		{};

		/// Gets called every frame, along all other logic.
		virtual void onLogicFrame(float delta)	{};

		/// Gets called when the program is closing. Happens before Window is terminated.
		virtual void onClose()	{};

		/// Queues a texture to recieve a copy of the screen.
		void queueScreenCopy(Drawer::Texture2D* target) {
			screenQueue.push_back(target);
		}

		/// Removes a texture from the screen copy queue.
		void unqueueScreenCopy(Drawer::Texture2D* target) {
			ERASE_IF(screenQueue, elem == target);
		}


		/// The window's clear color.
		Vector4 color = Color::BLACK;

		/// The program's input manager.
		InputManager input;

		/// The program's maximum framerate.
		float maxFrameRate = 30.0;

		/// The program's maximum "cycles per second" count.
		float maxCycleRate = 60.0;

		/// The program's speed scale.
		float speed = 1.0f;

		/// The program's taskers.
		Tasking::MultiTasker taskers;

	protected:
		Drawer::FrameBufferData toFrameBufferData() {
			return Drawer::FrameBufferData{0};
		}

	private:
		size_t cycleRate = 0, frameRate = 0;

		List<Drawer::Texture2D*> screenQueue;

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
			// Close YSE
			DEBUGLN("Closing sound system...");
			Audio::closeSystem();
			Mix_Quit();
			EntityClass::_ROOT->deleteChildren();
			DEBUGLN("Sound system closed!");
			// Destroy buffers
			DEBUGLN("Destroying frame buffers...");
			framebuffer.destroy();
			layerbuffer.destroy();
			DEBUGLN("Frame buffers destroyed!");
			// Quit SDL
			DEBUGLN("Ending SDL...");
			SDL_Quit();
			DEBUGLN("SDL ended!");
			//exit(0);
		}

		/// Draws the window.
		void render() {
			// Clear screen
			Drawer::clearColorBuffer(color);
			glClear(GL_DEPTH_BUFFER_BIT);
			// Enable depth testing
			glEnable(GL_DEPTH_TEST);
			// Enable frame buffer
			framebuffer();
			// Call rendering start function
			onDrawBegin();
			// Clear frame buffer
			framebuffer.clearBuffers();
			// Set frontface order
			glFrontFace(GL_CW);
			// Call post frame clearing function
			onPostFrameClear();
			// Draw objects
			vector<size_t> rLayers = Drawer::layers.getAllGroups();
			for (auto layer : rLayers) {
				if (layer != Math::Max::SIZET_V && !Drawer::layers[layer].empty()) {
					// Clear target depth buffer
					framebuffer();
					// Enable layer buffer
					layerbuffer();
					// Reset layerbuffer's positions
					layerbuffer.trans	= VecMath::Transform3D();
					layerbuffer.uv		= VecMath::Transform3D();
					// Call onLayerDrawBegin function
					onLayerDrawBegin(layer);
					// Clear buffers
					layerbuffer.clearBuffers();
					// Call onLayerDrawBegin function
					onPostLayerClear(layer);
					// Render layer
					Drawer::renderLayer(layer);
					// Call onPreLayerDraw function
					onPreLayerDraw(layer);
					// Render layer buffer
					layerbuffer.render(framebuffer);
					// Call onLayerDrawEnd function
					onLayerDrawEnd(layer);
				}
			}
			// Call pre frame drawing function
			onPreFrameDraw();
			// Render frame buffer
			framebuffer.render(toFrameBufferData());
			// Copy screen to queued textures
			copyScreenToQueued();
			// Call rendering end function
			onDrawEnd();
			// Disable depth testing
			glDisable(GL_DEPTH_TEST);
			// Display window
			SDL_GL_SwapWindow(window);
		}

		void copyScreenToQueued() {
			if (!screenQueue.empty()) {
				auto& screen = *framebuffer.toFrameBufferData().screen;
				for (Drawer::Texture2D* target: screenQueue)
					target->make(screen);
			}
			screenQueue.clear();
		}

		/// Frame counter.
		size_t frame = 0;

		/// Cycle counter.
		size_t cycle = 0;

		/// Current execution state.
		bool shouldRun = true;

		/// The program's window.
		SDL_Window* window;
	};

	Vector2 getDeviceSize(unsigned int display = 0) {
		SDL_Rect bounds;
		DEBUGLN("Starting SDL...");
		if (!SDL_WasInit(SDL_INIT_VIDEO))
			if (SDL_Init(SDL_INIT_VIDEO) != 0) {
				ERR_LOG(string("Unable to start SDL! (") + SDL_GetError() + ")");
				throw Error::FailedAction(string("SDL (") + SDL_GetError() + ")");
			}
		if (SDL_GetDisplayBounds(display, &bounds))
			throw Error::FailedAction(
				"Couldn't get display bounds!\n\n",
				__FILE__,
				toString(__LINE__),
				"Makai::Program::getDeviceSize",
				String(SDL_GetError())
			);
		return Vector2(bounds.h - bounds.x, bounds.w - bounds.y);
	}

	namespace Resolution {
		#define RESOLUTION(W, H) Entry<Vector2>{#W" x "#H , Vector2(W, H)}
		const vector<Pair<string, Vector2>> set4x3 = {
			RESOLUTION(480, 320),
			RESOLUTION(640, 480),
			RESOLUTION(860, 640),
			RESOLUTION(960, 720),
			RESOLUTION(1280, 960),
			RESOLUTION(1600, 1200)
		};

		const vector<Pair<string, Vector2>> set16x9 = {
			RESOLUTION(640, 360),
			RESOLUTION(854, 480),
			RESOLUTION(1280, 720),
			RESOLUTION(1600, 900),
			RESOLUTION(1920, 1080),
			RESOLUTION(2560, 1440)
		};
		#undef RESOLUTION
	}
}

namespace Popup {
	namespace Option {
		const StringList OK				= {"Ok"};
		const StringList YES			= {"Yes"};
		const StringList YES_NO			= {"Yes", "No"};
		const StringList YES_NO_CANCEL	= {"Yes", "No", "Cancel"};
	}

	namespace DialogBoxColorScheme {
		constexpr const SDL_MessageBoxColorScheme LIGHT = {
			{ /* .colors (.r, .g, .b) */
				/* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
				{ 255, 255, 255 },
				/* [SDL_MESSAGEBOX_COLOR_TEXT] */
				{   0,   0,   0 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
				{ 127, 127, 127 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
				{ 255, 255, 255 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
				{ 127, 127, 127 }
			}
		};
		constexpr const SDL_MessageBoxColorScheme DARK = {
			{ /* .colors (.r, .g, .b) */
				/* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
				{  32,  32,  32 },
				/* [SDL_MESSAGEBOX_COLOR_TEXT] */
				{ 255, 255, 255 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
				{  64,  64,  64 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
				{  32,  32,  32 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
				{  64,  64,  64 }
			}
		};
	}

	/**
	* Invokes a dialog box with a given number of buttons.
	* Returns the selected button (by index).
	* Returns -1 if no button was selected.
	*/
	int dialogBox(
		String const& title,
		String const& text,
		StringList const& options = Option::OK,
		Uint32 type = SDL_MESSAGEBOX_INFORMATION,
		SDL_Window* window = NULL,
		SDL_MessageBoxColorScheme const& colorScheme = DialogBoxColorScheme::LIGHT
	) {
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
		SetProcessDPIAware();
		#endif
		size_t buttonCount = options.size();
		SDL_MessageBoxButtonData buttons[options.size()];
		size_t idx = buttonCount - 1;
		for EACH(b, buttons) {
			b = SDL_MessageBoxButtonData {
				0,
				idx,
				options[idx].c_str()
			};
			idx--;
		}
		const SDL_MessageBoxData messageBoxData = {
			type, /* .flags */
			window, /* .window */
			title.c_str(), /* .title */
			text.c_str(), /* .message */
			SDL_arraysize(buttons), /* .numbuttons */
			buttons, /* .buttons */
			&colorScheme /* .colorScheme */
		};
		int buttonid;
		if (SDL_ShowMessageBox(&messageBoxData, &buttonid) < 0) {
			ERR_LOG("Could not show messagebox!");
			throw Error::FailedAction("SDL MessageBox");
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

#define SDL_EVENT(EVENT) (Makai::pollEvents().type == EVENT)

#endif // MAKAILIB_MAKAI_H
