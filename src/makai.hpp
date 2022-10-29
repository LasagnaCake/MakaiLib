#ifndef MAKAILIB_LIBRARY_HEADER_H
#define MAKAILIB_LIBRARY_HEADER_H

#include "anchors.hpp"
#include "program.hpp"
#include "graphical.hpp"
#include "gamedata.hpp"

typedef std::string			String;
typedef	std::vector<String>	StringList;

namespace Makai {
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

	namespace Resolution {
		#define RESOLUTION(W, H) {#W" × "#H , Vector2(W, H)}
		const map<string, Vector2> set4x3 = {
			RESOLUTION(480, 320),
			RESOLUTION(640, 480),
			RESOLUTION(960, 720),
			RESOLUTION(800, 600),
			RESOLUTION(1200, 900),
			RESOLUTION(1600, 1200)
		};
		#undef RESOLUTION
	}

	int queryInputFromUser(
		string title,
		string text,
		StringList options,
		Uint32 type = SDL_MESSAGEBOX_INFORMATION,
		SDL_MessageBoxColorScheme colorScheme = defaultMessageBoxColorScheme,
		SDL_Window* window = NULL
	) {
		SDL_MessageBoxButtonData buttons[options.size()];
		size_t idx = 0;
		for $each(b, buttons) {
			b = SDL_MessageBoxButtonData {
				0,
				idx,
				options[idx].c_str()
			};
			idx += 1;
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
			throw runtime_error("Error: SDL MessageBox");
		}
		return buttonid;
	}
}

#define $res $mki Resolution::

#endif // MAKAILIB_LIBRARY_HEADER_H
