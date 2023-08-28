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

		/// Whether input is enabled.
		bool enabled = true;

		/// How long before the input is considered "held".
		unsigned int threshold = 2048;
	private:
		/// The internal buffer state.
		unordered_map<SDL_Scancode, unsigned int> buffer;
		unordered_map<SDL_Scancode, unsigned int> last;
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
			bool useMIDI = false,
			string bufferShaderPath = "shaders/framebuffer/compose.slf",
			string mainShaderPath = "shaders/base/base.slf"
		) {
			#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
			SetProcessDPIAware();
			#endif
			$debug(EntityClass::$_ROOT == nullptr);
			// Save window resolution
			this->width = width;
			this->height = height;
			// Initialize SDL
			$debug("Starting SDL...");
			if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
				$errlog(string("Unable to start SDL! (") + SDL_GetError() + ")");
				throw Error::FailedAction(string("SDL (") + SDL_GetError() + ")");
			}
			$debug("Started!");
			// Initialize YSE
			$debug("Starting Audio System...");
			if (!Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG | (useMIDI ? MIX_INIT_MID : 0))) {
				$errlog(string("Unable to start Mixer! (") + Mix_GetError() + ")");
				throw Error::FailedAction(string("Mixer (") + Mix_GetError() + ")");
			}
			Audio::openSystem();
			$debug("Started!");
			// Create window and make active
			$debug("Creating window...");
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
			$debug("Created!");
			$debug("Starting GLEW...");
			// Try and initialize GLEW
			GLenum glew_status = glewInit();
			if (glew_status != GLEW_OK) {
				$errlog("Error: glewInit: " << glewGetErrorString(glew_status));
				throw Error::FailedAction(string("glewInit"));
			}
			if (!GLEW_VERSION_4_2) {
				$errlog("Your computer does not support OpenGL 4.2+!");
				throw Error::InvalidValue(string("No OpenGL 4.2+"));
			}
			$debug("Started!");
			// Create default shader
			/*$debug("Creating default shader...");
			Shader::defaultShader.create();
			$debug("Created!");*/
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 16);
			SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 16);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
			//glViewport(0, 0, width, height);
			// This keeps the alpha from shitting itself
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			glBlendFuncSeparatei(0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			// This keeps the alpha from shitting itself
			glBlendEquationSeparatei(0, GL_FUNC_ADD, GL_MAX);
			//glEnable(GL_CULL_FACE);
			//glBlendFuncSeparatei(0, DEFAULT_BLEND_FUNC);
			//glBlendEquationSeparatei(0, DEFAULT_BLEND_EQUATION);
			//glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
			// Setup camera
			$debug("Setting starting camera...");
			Scene::camera.aspect = Vector2(width, height);
			Scene::camera.fov = glm::radians(45.0f);
			$debug("creating default framebuffer...");
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
			$debug("Creating shaders...");
			bufferShader.create(SLF::parseFile(bufferShaderPath));
			framebuffer.shader = bufferShader;
			layerbuffer.shader = bufferShader;
			// Create main shader
			SLF::SLFData data = SLF::parseFile(mainShaderPath);
			$mainshader.destroy();
			$mainshader.create(data);
			$debug(EntityClass::$_ROOT ? "Root Exists!" : "Root does not exist!");
			if (!EntityClass::$_ROOT) {
				$debug("Initializing root tree...");
				EntityClass::init();
			}
			$debug("All core systems initialized!");
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
				if (EntityClass::$_ROOT)
					EntityClass::$_ROOT->yield(delta);
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
				frameDelta	= 1.0/maxFrameRate;
				cycleDelta	= 1.0/maxCycleRate;
				// Clamp speed
				speed = Math::clamp(speed, 0.0f, 1.0f);
				// Get rates
				cycleRate = SDL_GetTicks() - cycleTicks;
				frameRate = SDL_GetTicks() - frameTicks;
				// If should process, then do so
				#ifndef $_PROCESS_RENDER_BEFORE_LOGIC
				if (cycleRate > (((float)(cycleDelta * 1000) - 1) / speed)) {
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
					$ecl destroyQueued();
					#endif // FRAME_DEPENDENT_PROCESS
				}
				#endif
				if (frameRate > (frameDelta * 1000) - 1) {
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
					$ecl destroyQueued();
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
					$ecl destroyQueued();
					#endif // FRAME_DEPENDENT_PROCESS
				}
				#endif
			}
			// Terminate program
			$debug("\nClosing incoherent program...");
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
			$debug("Closing sound system...");
			Audio::closeSystem();
			Mix_Quit();
			EntityClass::$_ROOT->deleteChildren();
			$debug("Sound system closed!");
			// Destroy buffers
			$debug("Destroying frame buffers...");
			framebuffer.destroy();
			layerbuffer.destroy();
			$debug("Frame buffers destroyed!");
			// Quit SDL
			$debug("Ending SDL...");
			SDL_Quit();
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
				if (layer == Math::Max::SIZET_V) continue;
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
			// Call pre frame drawing function
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

		/// Cycle counter.
		size_t cycle = 0;

		/// Current execution state.
		bool shouldRun = true;

		/// The program's window.
		SDL_Window* window;
	};

	Vector2 getDeviceSize(unsigned int display = 0) {
		SDL_Rect bounds;
		$debug("Starting SDL...");
		if (!SDL_WasInit(SDL_INIT_VIDEO))
			if (SDL_Init(SDL_INIT_VIDEO) != 0) {
				$errlog(string("Unable to start SDL! (") + SDL_GetError() + ")");
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
	#define $mki Makai::
	#define $res $mki Resolution::
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
		for $each(b, buttons) {
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
			$errlog("Could not show messagebox!");
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

#define $event(EVENT) ($mki pollEvents().type == EVENT)

#endif // MAKAILIB_MAKAI_H
