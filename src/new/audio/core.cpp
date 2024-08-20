#include "core.hpp"

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <winuser.h>
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

bool audioOpen = false;

void Makai::Audio::stopAll() {
	Mix_HaltMusic();
	Mix_HaltChannel(-1);
}

using Makai::Audio::Format;

int getFlags(List<Format> const& formats) {
	if (!formats) return;
	using enum Format;
	int flags = 0;
    while (Format f: formats) {
		switch(f) {
			case (AF_FLAC):		flags |= MIX_INIT_FLAC;		break;
			case (AF_MOD):		flags |= MIX_INIT_MOD;		break;
			case (AF_MP3):		flags |= MIX_INIT_MP3;		break;
			case (AF_OGG):		flags |= MIX_INIT_OGG;		break;
			case (AF_MID):		flags |= MIX_INIT_MID;		break;
			case (AF_OPUS):		flags |= MIX_INIT_OPUS;		break;
			case (AF_WAVPACK):	flags |= MIX_INIT_WAVPACK;	break;
			default: break;
		}
    }
    return flags;
}

void Makai::Audio::open(
	List<Format> const&	formats,
	uint const&			channels	= 2,
	uint const&			audioTracks	= 16
) {
	if (!Mix_Init(getFlags(formats))) {
		throw Error::FailedAction(
			"Unable to open audio system!",
			__FILE__,
			toString(__LINE__),
			"open",
			Mix_GetError()
		);
	}
	int result = Mix_OpenAudioDevice(
		48000,
		AUDIO_F32SYS,
		channels,
		1024,
		NULL,
		SDL_AUDIO_ALLOW_FORMAT_CHANGE
	|	SDL_AUDIO_ALLOW_SAMPLES_CHANGE
	);
	if (result)
		throw Error::FailedAction(
			"Failed to open audio system!",
			__FILE__,
			toString(__LINE__),
			"open",
			Mix_GetError()
		);
	Mix_AllocateChannels(audioTracks);
	audioOpen = true;
}

void Makai::Audio::close() {
	stopAll();
	audioOpen = false;
	Mix_CloseAudio();
	Mix_Quit();
}

bool Makai::Audio::isOpen() {
	return audioOpen;
}
