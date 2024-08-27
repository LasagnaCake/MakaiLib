#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <windows.h>
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

#include "popup.hpp"

using namespace Makai;

int Popup::showDialog(
	String const& title,
	String const& text,
	StringList const& options = Option::OK,
	PopupType const& type = PopupType::PT_INFORMATION
) {
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
	uint32 flags = 0;
	switch (type) {
		case Popup::PopupType::PT_ERROR:	flags = SDL_MESSAGEBOX_ERROR;		break;
		case Popup::PopupType::PT_INFO:		flags = SDL_MESSAGEBOX_INFORMATION;	break;
		case Popup::PopupType::PT_WARN:		flags = SDL_MESSAGEBOX_WARNING;		break;
	}
	const SDL_MessageBoxData messageBoxData = {
		flags, /* .flags */
		NULL, /* .window */
		title.c_str(), /* .title */
		text.c_str(), /* .message */
		SDL_arraysize(buttons), /* .numbuttons */
		buttons, /* .buttons */
		NULL
	};
	int buttonid;
	if (SDL_ShowMessageBox(&messageBoxData, &buttonid) < 0) {
		ERR_LOG("Could not show popup!");
		throw Error::FailedAction(
			"Failed to show popup!",
			__FILE__,
			toString(__LINE__),
			"show",
			SDL_GetError()
		);
	}
	return buttonid;
}

void Popup::showError(String const& what) {
	//PlaySound("SystemExclamation", NULL, SND_ASYNC);
	Popup::dialogBox(
		"ERROR!",
		what,
		Popup::Option::OK,
		Popup::PopupType::PT_ERROR
	);
}
