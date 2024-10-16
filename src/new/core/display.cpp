#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <windows.h>
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

#include "display.hpp"

Vector2 Makai::getDisplaySize(usize const& display) {
	SDL_Rect bounds;
	DEBUGLN("Starting SDL...");
	if (!SDL_WasInit(SDL_INIT_VIDEO))
		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			throw Error::FailedAction(
				"Unable to start SDL!",
				__FILE__,
				toString(__LINE__),
				"getDisplaySize",
				String(SDL_GetError())
			);
		}
	if (SDL_GetDisplayBounds(display, &bounds))
		throw Error::FailedAction(
			"Couldn't get display bounds!",
			__FILE__,
			toString(__LINE__),
			"getDisplaySize",
			String(SDL_GetError())
		);
	return Vector2(bounds.h - bounds.x, bounds.w - bounds.y);
}
