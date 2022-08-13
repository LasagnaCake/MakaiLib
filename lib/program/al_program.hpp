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
	*//*
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
		Program(unsigned int width, unsigned int height, std::string windowTitle, unsigned int fps = 60, bool fullscreen = false) {
			// Initialize alegro
			$debug("Starting Allegro...");
			$enforce("Allegro", al_init());
			// Initialize primitives
			$debug("Initializing primitives addon...");
			$enforce("Primitives", al_init_primitives_addon());
			// Enable keyboard
			$debug("Installing keyboard...");
			$enforce("Keyboard", al_install_keyboard());
			// Create frame counter
			$debug("Setting frame counter...");
			sys.timer = al_create_timer(1.0 / fps);
			// Create event queue
			$debug("Creating event queue...");
			sys.queue = al_create_event_queue();
			// Create default font
			$debug("Creating default font...");
			defaultFont = al_create_builtin_font();
			$debug("Setting display options...");
			al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
			al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
			al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
			// Create window
			$debug("Creating window...");
			sys.window = al_create_display(width, height);
			// Register events
			$debug("Registering events:");
			$debug("	keyboard...");
			al_register_event_source(sys.queue, al_get_keyboard_event_source());
			$debug("	display...");
			al_register_event_source(sys.queue, al_get_display_event_source(sys.window));
			$debug("	frame counter...");
			al_register_event_source(sys.queue, al_get_timer_event_source(sys.timer));
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
			// Start timer
			$debug("Starting frame counter...");
			al_start_timer(sys.timer);
			// Whether to draw frame
			bool redraw = true;
			// Whether program was closed
			bool shouldClose = false;
			// While program is running...
			while(shouldRun && !shouldClose) {
				// Wait for events
				al_wait_for_event(sys.queue, &sys.event);
				// Process events:
				switch (sys.event.type) {
				case ALLEGRO_EVENT_TIMER:
					redraw = true;
					break;
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					shouldClose = true;
				}
				// Start thread
				std::thread physics(physFunc, fixedDelta);
				// Do your own stuff
				logicFunc(fixedDelta);
				onFrame();
				// Wait for thread to be done processing
				physics.join();
				// If should draw screen...
				if(redraw && al_is_event_queue_empty(sys.queue)) {
					// Clear screen
					al_clear_to_color(al_map_rgba_f(windowColor.x, windowColor.y, windowColor.z, windowColor.w));
					// Draw internal frame
					onDraw();
					// Flip buffers
					al_flip_display();
					// Disable drawing until next frame
					redraw = false;
				}
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
		Event::Signal	onDraw	= $func() {};


		/// Gets called every frame, along all other logic.
		Event::Signal	onFrame = $func() {};


		/// Gets called when the program is closing. Happens before Window is terminated.
		Event::Signal	onClose = $func() {};

		/// The program's window output.
		struct {
		} out;

		/// The window's default font.
		ALLEGRO_FONT* defaultFont;

		/// The window's color.
		VecV4 windowColor = VecV4(.5, .5, .5, 1.0);
	private:
		/// Properly finishes program execution.
		void terminate() {
			// Call final function
			onClose();
			// Destroy allegro stuff
			al_destroy_font(defaultFont);
			al_destroy_display(sys.window);
			al_destroy_timer(sys.timer);
			al_destroy_event_queue(sys.queue);
		}
		struct {
			/// The program's window.
			ALLEGRO_DISPLAY* window;
			ALLEGRO_TIMER* timer;
			ALLEGRO_EVENT_QUEUE* queue;
			ALLEGRO_EVENT event;
		}sys;
		/// Current execution state.
		bool shouldRun = true;
	};
}

#endif // MAKAILIB_MAKAI_H
