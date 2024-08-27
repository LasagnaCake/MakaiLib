#ifndef MAKAI_CORE_INPUT_MANAGER_H
#define MAKAI_CORE_INPUT_MANAGER_H

#include "buttons.hpp"
#include "../extern.hpp"
#include "../display.hpp"

namespace Makai::Input {
	struct MouseState {
		ScreenPosition pos, last;
	};

	enum class InputMask: ubyte {
		IM_NONE				= 0b000,
		IM_KEY				= 0b001,
		IM_MOUSE			= 0b010,
		IM_JOY				= 0b100,
		IM_KEY_AND_MOUSE	= 0b011,
		IM_JOY_AND_MOUSE	= 0b110,
		IM_JOY_AND_JOY		= 0b101,
		IM_ALL				= 0b111,
	};

	/**
	* WARNING: Controller input is currently unimplemented!
	*/
	class Manager {
		using KeyBuffer			= HashMap<KeyCode, usize>;
		using MouseButtonBuffer	= HashMap<MouseCode, usize>;

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
		Vector2 getWindowMousePosition();

		/// Returns the mouse position relative to the desktop.
		Vector2 getDesktopMousePosition();

		/// Returns which direction the mouse is moving towards.
		Vector2 getMouseDirection();

		/// Enables/Disables mouse capturing on the "main window" (first program created).
		static void setMouseCapturing(bool const& enabled = true, bool const& hideCursor = true);

		/// Shows/Hides the cursor while in the window.
		static void setCursorVisibility(bool const& enabled = true);

		/**
		* Returns the button's state.
		* 0		= Unpressed;
		* 1+	= Pressed;
		* Recommended if time pressed is required.
		*/
		//[[gnu::warning("Controller input is currently unimplemented!")]]
		usize getButtonState(Button const& button);

		/// Returns whether the button is pressed.
		//[[gnu::warning("Controller input is currently unimplemented!")]]
		bool isButtonDown(Button const& button);

		/// Returns if the button has just been pressed (state == 1).
		//[[gnu::warning("Controller input is currently unimplemented!")]]
		bool isButtonJustPressed(Button const& button);

		/// Returns if the button has just been released.
		//[[gnu::warning("Controller input is currently unimplemented!")]]
		bool isButtonJustReleased(Button const& button);

		/// Returns if the button is held (state > threshold).
		//[[gnu::warning("Controller input is currently unimplemented!")]]
		bool isButtonHeld(Button const& button);

		/// Returns if the button's state has changed.
		//[[gnu::warning("Controller input is currently unimplemented!")]]
		bool hasButtonChanged(Button const& button);

		/// Returns all buttons who are currently pressed.
		//[[gnu::warning("Controller input is currently unimplemented!")]]
		List<Button> getButtonsDown();

		/// Returns all buttons whose state just changed.
		//[[gnu::warning("Controller input is currently unimplemented!")]]
		List<Button> getButtonsChanged();

		/// Returns all buttons who have been recently just pressed.
		//[[gnu::warning("Controller input is currently unimplemented!")]]
		List<Button> getButtonsJustPressed();

		/// Returns all buttons who have been recently just released.
		//[[gnu::warning("Controller input is currently unimplemented!")]]
		List<Button> getButtonsJustReleased();

		/// Returns all buttons who are currently held.
		//[[gnu::warning("Controller input is currently unimplemented!")]]
		List<Button> getButtonsHeld();

		/**
		* Returns the button's state.
		* 0		= Unpressed;
		* 1+	= Pressed;
		* Recommended if time pressed is required.
		*/
		usize getButtonState(String const& button);

		/// Returns whether the button is pressed.
		bool isButtonDown(String const& button);

		/// Returns if the button has just been pressed (state == 1).
		bool isButtonJustPressed(String const& button);

		/// Returns if the button has just been released.
		bool isButtonJustReleased(String const& button);

		/// Returns if the button is held (state > threshold).
		bool isButtonHeld(String const& button);

		/// Returns if the button's state has changed.
		bool hasButtonChanged(String const& button);
		/*
		/// Returns the button that was most recently pressed.
		Button mostRecentButtonDown();

		/// Returns the button that was most recently just pressed.
		Button mostRecentButtonJustPressed();

		/// Returns the button that was most recently held.
		Button mostRecentButtonHeld();

		/// Returns the button that was most recently changed.
		Button mostRecentButtonChanged();

		/// Returns the button that was most recently just released.
		Button mostRecentButtonJustReleased();
		*/
		/// Returns if there is a button binding with the given name.
		bool isBound(String const& name);

		/// Returns the bindings the given button is associated with.
		StringList getNamesForButton(Button const& button);

		inline static ButtonMap binds;

		/// Whether input is enabled.
		static inline bool enabled = true;

		inline static InputMask mask = InputMask::IM_ALL;

		/// How long before the input is considered "held".
		inline static usize threshold = 2048;

		static void refreshCapture();

		static void setTargetWindow(Extern::Resource const& target) {
			if (target && !window)
				window = target;
		}

		static void clearTargetWindow(Extern::Resource const& target) {
			if (target && window == target)
				window = nullptr;
		}

		static bool isTargetWindow(Extern::Resource const& target) {
			return target && window == target;
		}

	private:
		usize getKeyState(KeyCode const& button);
		usize getJoyState(JoyCode const& button);
		usize getMouseState(MouseCode const& button);

		usize getLastKeyState(KeyCode const& button);
		usize getLastJoyState(JoyCode const& button);
		usize getLastMouseState(MouseCode const& button);

		usize getLastButtonState(Button const& button);

		inline static bool mouseCaptured	= false;
		inline static bool mouseVisible		= true;

		inline static Extern::Resource window = nullptr;

		/// The internal buffer state.
		inline static KeyBuffer		buffer;
		inline static KeyBuffer		last;
		inline static MouseBuffer	mouse;
	};
}

#endif // MAKAI_CORE_INPUT_MANAGER_H
