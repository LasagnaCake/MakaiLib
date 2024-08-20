#include "sound.hpp"

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <winuser.h>
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

using namespace Makai::Audio;

void Sound::stopAll(usize const& fade = 0) {
	if (fade)
		Mix_FadeOutChannel(-1, fade);
	else
		Mix_HaltChannel(-1);
}

void Sound::pauseAll() {
	Mix_Pause(-1);
}

void Sound::resumeAll() {
	Mix_Resume(-1);
}

void Sound::setMasterVolume(schar const& volume) {
	Mix_Volume(-1, volume);
}

schar Sound::getMasterVolume() {
	return Mix_Volume(-1, -1);
}

void Sound::play(
	int const&		loops		= 0,
	size_t const&	fadeInTime	= 0,
	bool const&		force		= false
) {
	if (!exists() || delay > 0) return;
	if (active() && !force) return;
	if (fadeInTime)
		channel = Mix_FadeInChannel(active() ? channel : -1, source, loops, fadeInTime);
	else
		channel = Mix_PlayChannel(active() ? channel : -1, source, loops);
}

void Sound::playOnceThisFrame(
	int const&		loops		= 0,
	size_t const&	fadeInTime	= 0,
	bool const&		force		= false
) {
	if (!exists() || delay > 0) return;
	if (active() && !force) return;
	if (!canPlayThisFrame) return;
	canPlayThisFrame = false;
	if (fadeInTime)
		channel = Mix_FadeInChannel(getChannel(), source, loops, fadeInTime);
	else
		channel = Mix_PlayChannel(getChannel(), source, loops);
}

void Sound::playOnceAndWait(
	int const&		loops		= 0,
	size_t const&	fadeInTime	= 0,
	bool const&		force		= false,
	size_t const&	cycles		= 0
) {
	if (!exists() || delay > 0) return;
	if (active() && !force) return;
	if (!canPlayThisFrame) return;
	canPlayThisFrame = false;
	delay = cycles;
	if (fadeInTime)
		channel = Mix_FadeInChannel(getChannel(), source, loops, fadeInTime);
	else
		channel = Mix_PlayChannel(getChannel(), source, loops);
}

void Sound::stop(size_t const& fadeOutTime = 0) {
	if (!exists()) return;
	if (!active()) return;
	if (fadeOutTime)
		Mix_FadeOutChannel(channel, fadeOutTime);
	else
		Mix_HaltChannel(channel);
}

void Sound::pause() {
	if (!exists()) return;
	if (!active()) return;
	Mix_Pause(channel);
}

void Sound::resume() {
	if (!exists()) return;
	if (!active()) return;
	Mix_Resume(channel);
}

void Sound::setVolume(uchar const& volume) {
	if (!exists()) return;
	Mix_VolumeChunk(source, volume);
}

int Sound::getVolume() {
	if (!exists()) return -1;
	return Mix_VolumeChunk(source, -1);
}

void Sound::onCreate(Extern::Resource const& data) final override {
	if (!isOpen()) throw Error::FailedAction("Failed to load file: Audio system is closed!");
	source = Mix_LoadWAV_RW(data, true);
	if (!source)
		throw Error::FailedAction(
			"Could not load file!",
			__FILE__,
			toString(__LINE__),
			"onCreate",
			String(Mix_GetError())
		);
}

void Sound::onDestroy() final override {
	DEBUGLN("Destroying sound source...");
	if (isOpen()) {
		if (active())
			Mix_HaltChannel(channel);
		Mix_FreeChunk(source);
	}
	DEBUGLN("Sound source deleted!");
}

void Sound::onUpdate() final override {
	canPlayThisFrame = true;
	if (!active()) {
		channel = -1;
		return;
	}
	if (delay > 0) delay--;
}

int Sound::getChannel() {
	return active() ? channel : -1;
}

bool Sound::active() {
	if (!exists())		return false;
	if (channel == -1)	return false;
	return (
		Mix_Playing(channel)
	&&	Mix_GetChunk(channel) == source
	);
}
