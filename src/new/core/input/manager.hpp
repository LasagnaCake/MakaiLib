#ifndef MAKAI_CORE_INPUT_MANAGER_H
#define MAKAI_CORE_INPUT_MANAGER_H

#include "buttons.hpp"
#include "../program.hpp"

namespace Makai::Input {
	struct MouseState {
		ScreenPosition pos, last;
	};

	struct InputMask {
		bool key	: 1	= true;
		bool mouse	: 1	= true;
		bool joy	: 1	= true;
	};

	enum class InputMask: byte {
		IM_NONE				= 0b000,
		IM_KEY				= 0b001,
		IM_MOUSE			= 0b010,
		IM_JOY				= 0b100,
		IM_KEY_AND_MOUSE	= 0b011,
		IM_JOY_AND_MOUSE	= 0b110,
		IM_JOY_AND_JOY		= 0b101,
		IM_ALL				= 0b111,
	};

	class Manager {
		typedef HashMap<KeyCode, usize>		KeyBuffer;
		typedef HashMap<MouseCode, usize>	MouseButtonBuffer;

		struct MouseBuffer {
			MouseState global;
			MouseState local;
			MouseState relative;
			MouseButtonBuffer buffer, last;
		};

	public:
		/// Empty Constructor.
		Manager() {}

		/**
		* Updates the input manager.
		* Not required to call every cycle, as the program already handles it.
		*/
		// Should this be privated? Probably not...
		static void update();

		/// Returns the mouse position relative to the window.
		inline Vector2 getWindowMousePosition();

		/// Returns the mouse position relative to the desktop.
		inline Vector2 getDesktopMousePosition();

		/// Returns which direction the mouse is moving towards.
		inline Vector2 getMouseDirection();

		/// Enables/Disables mouse capturing on the "main window" (first program created).
		inline static void setMouseCapturing(bool const& enabled = true, bool const& hideCursor = true);

		/// Shows/Hides the cursor while in the window.
		inline static void setCursorVisibility(bool const& enabled = true);

		/**
		* Returns the button's state.
		* 0		= Unpressed;
		* 1+	= Pressed;
		* Recommended if time pressed is required.
		*/
		[[gnu::warning("Controller input is currently unimplemented!")]]
		inline usize getButtonState(Button const& button);

		/// Returns whether the button is pressed.
		[[gnu::warning("Controller input is currently unimplemented!")]]
		inline bool isButtonDown(Button const& button);

		/// Returns if the button has just been pressed (state == 1).
		[[gnu::warning("Controller input is currently unimplemented!")]]
		inline bool isButtonJustPressed(Button const& button);

		/// Returns if the button has just been released.
		[[gnu::warning("Controller input is currently unimplemented!")]]
		inline bool isButtonJustReleased(Button const& button);

		/// Returns if the button is held (state > threshold).
		[[gnu::warning("Controller input is currently unimplemented!")]]
		inline bool isButtonHeld(Button const& button);

		/// Returns if the button's state has changed.
		[[gnu::warning("Controller input is currently unimplemented!")]]
		inline bool hasButtonChanged(Button const& button);

		/// Returns all buttons who are currently pressed.
		[[gnu::warning("Controller input is currently unimplemented!")]]
		inline List<Button> getButtonsPressed();

		/// Returns all buttons whose state just changed.
		[[gnu::warning("Controller input is currently unimplemented!")]]
		inline List<Button> getButtonsChanged();

		/// Returns all buttons who have been recently just pressed.
		[[gnu::warning("Controller input is currently unimplemented!")]]
		inline List<Button> getButtonsJustPressed();

		/// Returns all buttons who have been recently just released.
		[[gnu::warning("Controller input is currently unimplemented!")]]
		inline List<Button> getButtonsJustReleased();

		/**
		* Returns the button's state.
		* 0		= Unpressed;
		* 1+	= Pressed;
		* Recommended if time pressed is required.
		*/
		inline usize getButtonState(String const& button);

		/// Returns whether the button is pressed.
		inline bool isButtonDown(String const& button);

		/// Returns if the button has just been pressed (state == 1).
		inline bool isButtonJustPressed(String const& button);

		/// Returns if the button has just been released.
		inline bool isButtonJustReleased(String const& button);

		/// Returns if the button is held (state > threshold).
		inline bool isButtonHeld(String const& button);

		/// Returns if the button's state has changed.
		inline bool hasButtonChanged(String const& button);
		/*
		/// Returns the button that was most recently pressed.
		inline Button mostRecentButtonDown();

		/// Returns the button that was most recently just pressed.
		inline Button mostRecentButtonJustPressed();

		/// Returns the button that was most recently held.
		inline Button mostRecentButtonHeld();

		/// Returns the button that was most recently changed.
		inline Button mostRecentButtonChanged();

		/// Returns the button that was most recently just released.
		inline Button mostRecentButtonJustReleased();
		*/
		/// Returns if there is a button binding with the given name.
		inline bool isBound(String const& name);

		/// Returns the bindings the given button is associated with.
		inline StringList getNamesForButton(Button const& button);

		inline static ButtonMap binds;

		/// Whether input is enabled.
		bool enabled = true;

		static InputMask mask = IM_ALL;

		/// How long before the input is considered "held".
		usize threshold = 2048;

	private:
		inline usize getKeyState(KeyCode const& button);
		inline usize getJoyState(JoyCode const& button);
		inline usize getMouseState(MouseCode const& button);

		inline usize getLastKeyState(KeyCode const& button);
		inline usize getLastJoyState(JoyCode const& button);
		inline usize getLastMouseState(MouseCode const& button);

		inline usize getLastButtonState(Button const& button);

		inline static void refreshCapture();

		inline static bool
			mouseCaptured	= false,
			mouseVisible	= true
		;

		static void* window;

		/// The internal buffer state.
		inline static KeyBuffer		buffer;
		inline static KeyBuffer		last;
		inline static MouseBuffer	mouse;

		friend class ::Makai::App;
	};
}

#endif // MAKAI_CORE_INPUT_MANAGER_H
