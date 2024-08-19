#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <winuser.h>
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

#include <SDL2/SDL_mixer.h>

#include "input.hpp"

#define window ((SDL_Window*)window)

using namespace Makai::Input

SDL_Event pollEvents() {
	SDL_Event ev;
	SDL_PollEvent(&ev);
	return ev;
}

constexpr SDL_Scancode convert(KeyCode const& key) {
	int16 code = (int16)(key);
	switch (key) {
		case KeyCode::KC_MODE:			code += 16;
		case KeyCode::KC_LEFT_CTRL:		code += 3;
		case KeyCode::KC_KEYPAD_COMMA:	code += 4;
		case KeyCode::KC_A:				code += 4;
		default:						code += 1;
	}
	return (SDL_Scancode)code;
}

constexpr KeyCode convert(SDL_Scancode const& key) {
	int16 code = (int16)(key);
	switch (key) {
		case SDL_SCANCODE_MODE:		code -= 16;
		case SDL_SCANCODE_LCTRL:	code -= 3;
		case SDL_SCANCODE_KP_COMMA:	code -= 4;
		case SDL_SCANCODE_A:		code -= 4;
		default:					code -= 1;
	}
	return (KeyCode)code;
}

constexpr SDL_GameControllerButton convert(ControllerButton const& btn) {
	return (SDL_GameControllerButton)btn;
}

constexpr ControllerButton convert(SDL_GameControllerButton const& btn) {
	return (ControllerButton)btn;
}

static void Manager::update() {
	// Get keyboard state
	int keyCount = 0;
	const unsigned char* state = SDL_GetKeyboardState(&keyCount);
	for (int i = 0; i < keyCount; i++) {
		// Jankify
		SDL_Scancode	button	= (SDL_Scancode)(i);
		KeyCode			code	= convert(button);
		// Get previous key state
		usize buttonState = 0;
		if (buffer[code]) buttonState = buffer[code];
		// If button is pressed...
		if(state[button]) {
			// If buffer not overflowing, increment buffer
			if(buttonState < 0xFFFF) buttonState++;
		}
		// Else, zero state
		else buttonState = 0;
		// Copy previous state to secondary buffer
		last[code]		= buffer[code];
		// Set buffer to button state
		buffer[code]	= buttonState;
	}
	// Get mouse structures
	MouseState& lm = mouse.local;
	MouseState& gm = mouse.global;
	MouseState& rm = mouse.relative;
	// Save last positional data
	lm.last = lm.pos;
	gm.last = gm.pos;
	rm.last = rm.pos;
	// Get position & button data
	uint32 btn = SDL_GetMouseState(&lm.pos.x, &lm.pos.y);
	SDL_GetGlobalMouseState(&gm.pos.x, &gm.pos.y);
	SDL_GetRelativeMouseState(&rm.pos.x, &rm.pos.y);
	// Update button data
	for (auto i = 0; i < (((int)MouseCode::MC_MAX_BUTTONS)-1); i++) {
		// Jankify
		MouseCode button	= (MouseCode)i;
		MouseCode code		= (MouseCode)i;
		// Get previous button state
		usize buttonState = 0;
		if (mouse.buffer[code]) buttonState = mouse.buffer[code];
		// If button is pressed.
		if(btn & SDL_BUTTON(i)) {
			// If buffer not overflowing, increment buffer
			if(buttonState < 0xFFFF) buttonState++;
		}
		// Else, zero state
		else buttonState = 0;
		// Copy previous state to secondary buffer
		mouse.last[code]	= mouse.buffer[code];
		// Set buffer to button state
		mouse.buffer[code]	= buttonState;
	}
}

inline usize Manager::getKeyState(KeyCode const& button) {
	if (
		!enabled
	||	!(mask & InputMask::IM_KEY)
	||	button == KeyCode::KC_UNKNOWN
	) return 0;
	return buffer[button];
}

inline usize Manager::getMouseState(MouseCode const& button) {
	if (
		!enabled
	||	!(mask & InputMask::IM_MOUSE)
	||	button == MouseCode::MC_UNKNOWN
	) return 0;
	return mouse.buffer[button];
}

inline usize Manager::getJoyState(JoyCode const& button) {
	if (
		!enabled
	||	!(mask & InputMask::IM_JOY)
	||	button == JoyCode::JC_UNKNOWN
	) return 0;
	return 0;
}

inline usize Manager::getLastKeyState(KeyCode const& button) {
	if (
		!enabled
	||	!(mask & InputMask::IM_KEY)
	||	button == KeyCode::KC_UNKNOWN
	) return 0;
	return last[button];
}

inline usize Manager::getLastMouseState(MouseCode const& button) {
	if (
		!enabled
	||	!(mask & InputMask::IM_MOUSE)
	||	button == MouseCode::MC_UNKNOWN
	) return 0;
	return mouse.last[button];
}

inline usize Manager::getLastJoyState(JoyCode const& button) {
	if (
		!enabled
	||	!(mask & InputMask::IM_JOY)
	||	button == JoyCode::JC_UNKNOWN
	) return 0;
	return 0;
}

inline bool isStateDown(usize const& state) {
	return state > 0;
}

inline bool isStateJustPressed(usize const& state) {
	return state == 1;
}

inline bool isStateJustReleased(usize const& state, bool const& justChanged) {
	return (justChanged && (state == 0));
}

inline bool isStateHeld(usize const& state, usize const& threshold) {
	return state > threshold;
}

inline bool hasStateChanged(usize const& state, usize const& last) {
	return state != last;
}
/*
/// Returns the button that was most recently pressed.
inline KeyboardButton mostRecentKeyboardButtonDown() {
	KeyboardButton button	= SDL_SCANCODE_UNKNOWN;
	usize duration	= Math::Max::UINT_V;
	for (auto [b, d] : buffer)
		if ((d) && d < duration) {
			button		= b;
			duration	= d;
		}
	return button;
}

/// Returns the button that was most recently just pressed.
inline KeyboardButton mostRecentKeyboardButtonJustPressed() {
	for (auto [b, d] : buffer)
		if (d == 1)
			return b;
	return SDL_SCANCODE_UNKNOWN;
}

/// Returns the button that was most recently held.
inline KeyboardButton mostRecentKeyboardButtonHeld() {
	KeyboardButton button	= SDL_SCANCODE_UNKNOWN;
	usize duration	= Math::Max::UINT_V;
	for (auto [b, d] : buffer)
		if (d > threshold && d < duration) {
			button		= b;
			duration	= d;
		}
	return button;
}

/// Returns the button that was most recently changed.
inline KeyboardButton mostRecentKeyboardButtonChanged() {
	for (auto [b, d] : buffer)
		if (d != last[b])
			return b;
	return SDL_SCANCODE_UNKNOWN;
}

/// Returns the button that was most recently just released.
inline KeyboardButton mostRecentKeyboardButtonJustReleased() {
	for (auto [b, d] : buffer)
		if (d != last[b] && d == 0)
			return b;
	return SDL_SCANCODE_UNKNOWN;
}

/// Returns all buttons currently pressed.
inline List<KeyboardButton> getKeyboardButtonsDown() {
	List<KeyboardButton> buttons;
	buttons.reserve(buffer.size());
	for (auto [b, d] : buffer)
		if (isButtonDown(b))
			buttons.push_back(b);
	return buttons;
}

/// Returns all buttons currently just pressed.
inline List<KeyboardButton> getKeyboardButtonsJustPressed() {
	List<KeyboardButton> buttons;
	buttons.reserve(buffer.size());
	for (auto [b, d] : buffer)
		if (isButtonJustPressed(b))
			buttons.push_back(b);
	return buttons;
}

/// Returns all buttons currently just released.
inline List<KeyboardButton> getKeyboardButtonsJustReleased() {
	List<KeyboardButton> buttons;
	buttons.reserve(buffer.size());
	for (auto [b, d] : buffer)
		if (isButtonJustReleased(b))
			buttons.push_back(b);
	return buttons;
}

/// Returns all buttons whose state changed.
inline List<KeyboardButton> getKeyboardButtonsChanged() {
	List<KeyboardButton> buttons;
	for (auto [b, d] : buffer)
		if (isButtonChanged(b))
			buttons.push_back(b);
	return buttons;
}

/// Returns all buttons currently held.
inline List<KeyboardButton> getKeyboardButtonsHeld() {
	List<KeyboardButton> buttons;
	buttons.reserve(buffer.size());
	for (auto [b, d] : buffer)
		if (isButtonHeld(b))
			buttons.push_back(b);
	return buttons;
}
*/

inline usize Manager::getButtonState(Button const& button) {
	if (!enabled) return 0;
	switch (button.type) {
		case ButtonCodeType::BCT_KEY:	return getKeyState(button.code.key);
		case ButtonCodeType::BCT_MOUSE:	return getMouseState(button.code.mouse);
		case ButtonCodeType::BCT_KEY:	return getJoyState(button.code.joy);
	}
	return 0;
}

inline usize Manager::getLastButtonState(Button const& button) {
	if (!enabled) return 0;
	switch (button.type) {
		case ButtonCodeType::BCT_KEY:	return getLastKeyState(button.code.key);
		case ButtonCodeType::BCT_MOUSE:	return getLastMouseState(button.code.mouse);
		case ButtonCodeType::BCT_KEY:	return getLastJoyState(button.code.joy);
	}
	return 0;
}

inline bool Manager::isButtonDown(Button const& button) {
	if (!enabled) return false;
	return isStateDown(getButtonState(button));
}

inline bool Manager::isButtonJustPressed(Button const& button) {
	if (!enabled) return false;
	return isStateJustPressed(getButtonState(button));
}

inline bool Manager::isButtonJustReleased(Button const& button) {
	if (!enabled) return false;
	return isStateJustReleased(getButtonState(button), hasButtonJustChanged(button));
}

inline bool Manager::isButtonHeld(Button const& button) {
	if (!enabled || !isBound(button)) return false;
	return isStateHeld(getButtonState(button), threshold);
}

inline bool Manager::hasButtonChanged(Button const& button) {
	if (!enabled || !isBound(button)) return false;
	return hasStateChanged(getButtonState(button), getLastButtonState(button));
}

inline bool Manager::getButtonState(String const& button) {
	if (!enabled || !isBound(button))
		return 0;
	usize state = 0, current = 0;
	for (Button& btn: binds[button])
		if ((current = getButtonState(btn)) > state)
			state = current;
	return state;
}

inline bool Manager::isButtonDown(String const& button) {
	if (!enabled || !isBound(button)) return false;
	return isStateDown(getButtonState(button));
}

inline bool Manager::isButtonJustPressed(String const& button) {
	if (!enabled || !isBound(button)) return false;
	return isStateJustPressed(getButtonState(button));
}

inline bool Manager::isButtonJustReleased(String const& button) {
	if (!enabled || !isBound(button)) return false;
	return isStateJustReleased(getButtonState(button), hasButtonJustChanged(button));
}

inline bool Manager::isButtonHeld(String const& button) {
	if (!enabled || !isBound(button)) return false;
	return isStateHeld(getButtonState(button), threshold);
}

inline bool Manager::hasButtonChanged(String const& button) {
	if (!enabled || !isBound(button))
		return false;
	for (Button& btn: binds[button])
		if (hasButtonChanged(btn)) return true;
	return false;
}

inline bool Manager::isBound(String const& name) {
	return binds.contains(name);
}

inline StringList Manager::getNamesForButton(Button const& button) {
	StringList names;
	for (auto& [name, buttons]: binds) {
		for (Button& btn: buttons) {
			if (button == btn) {
				names.push_back(name);
				break;
			}
		}
	}
	return names;
}

inline List<Button> Manager::getButtonsDown() {
	List<Button> buttons;
	buttons.reserve(buffer.size() + mouse.buffer.size);
	for (auto [b, d] : buffer)
		if (isButtonDown(b))
			buttons.push_back(b);
	for (auto [b, d] : mouse.buffer)
		if (isButtonDown(b))
			buttons.push_back(b);
}

inline List<Button> Manager::getButtonsJustPressed() {
	List<Button> buttons;
	buttons.reserve(buffer.size() + mouse.buffer.size);
	for (auto [b, d] : buffer)
		if (isButtonJustPressed(b))
			buttons.push_back(b);
	for (auto [b, d] : mouse.buffer)
		if (isButtonJustPressed(b))
			buttons.push_back(b);
}

inline List<Button> Manager::getButtonsJustReleased() {
	List<Button> buttons;
	buttons.reserve(buffer.size() + mouse.buffer.size);
	for (auto [b, d] : buffer)
		if (isButtonJustReleased(b))
			buttons.push_back(b);
	for (auto [b, d] : mouse.buffer)
		if (isButtonJustReleased(b))
			buttons.push_back(b);
}

inline List<Button> Manager::getButtonsChanged() {
	List<Button> buttons;
	buttons.reserve(buffer.size() + mouse.buffer.size);
	for (auto [b, d] : buffer)
		if (hasButtonChanged(b))
			buttons.push_back(b);
	for (auto [b, d] : mouse.buffer)
		if (hasButtonChanged(b))
			buttons.push_back(b);
}

inline List<Button> Manager::getButtonsHeld() {
	List<Button> buttons;
	buttons.reserve(buffer.size() + mouse.buffer.size);
	for (auto [b, d] : buffer)
		if (isButtonHeld(b))
			buttons.push_back(b);
	for (auto [b, d] : mouse.buffer)
		if (isButtonHeld(b))
			buttons.push_back(b);
}

inline static void Manager::refreshCapture() {
	DEBUGLN("Refreshing mouse capture states...");
	setMouseCapturing(mouseCaptured, !mouseVisible);
}

inline static void Manager::setMouseCapturing(bool const& enabled = true, bool const& hideCursor = true) {
	if (!window) return;
	SDL_SetWindowGrab(window, enabled ? SDL_TRUE : SDL_FALSE);
	SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
	setCursorVisibility(!hideCursor);
	mouseCaptured = enabled;
}

inline static void Manager::setCursorVisibility(bool const& enabled = true) {
	if (!window) return;
	SDL_ShowCursor(enabled ? SDL_TRUE : SDL_FALSE);
	mouseVisible = enabled;
}

/// Returns the mouse position relative to the window.
inline Vector2 Manager::getWindowMousePosition() {
	return Vector2(mouse.local.pos.x, mouse.local.pos.y);
}

/// Returns the mouse position relative to the desktop.
inline Vector2 Manager::getDesktopMousePosition() {
	return Vector2(mouse.global.pos.x, mouse.global.pos.y);
}

/// Returns which direction the mouse is moving towards.
inline Vector2 Manager::getMouseDirection() {
	return Vector2(mouse.relative.pos.x, mouse.relative.pos.y);
}
