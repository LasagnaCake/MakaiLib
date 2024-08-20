#include "music.hpp"

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <winuser.h>
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

using namespace Makai::Audio;

namespace {
	Mix_Music*		current	= nullptr;
	AudioCallback*	queued	= nullptr;
}

void Music::update() {
	if (!Mix_PlayingMusic()) {
		current = nullptr;
		if (queued) {
			(*queued)();
			queued = nullptr;
		}
	}
}

void Music::stop(usize const& fade = 0) {
	if (fadeOutTime)
		Mix_FadeOutMusic(fade);
	else
		Mix_HaltMusic();
}

void Music::pause() {
	Mix_PauseMusic();
}

void Music::resume() {
	Mix_ResumeMusic();
}

void Music::setVolume(schar const& volume) {
	Mix_VolumeMusic(volume);
}

schar Music::getVolume() {
	return Mix_VolumeMusic(-1);
}

virtual Music::~Music() final {
	//destroy();
}

void Music::play(int const& loops = 0, usize const& fadeInTime = 0) {
	if (!exists()) return;
	this->loops = loops;
	current	= source;
	if (fadeInTime)
		Mix_FadeInMusic(source, loops, fadeInTime);
	else
		Mix_PlayMusic(source, loops);
}

String Music::getTitle() {
	return String(Mix_GetMusicTitle(source));
}

Music::MetaData Music::getMetaData() {
	const char* data;
	return Music::MetaData {
		(data = Mix_GetMusicTitleTag(source)) ? data : "???",
		(data = Mix_GetMusicArtistTag(source)) ? data : "???",
		(data = Mix_GetMusicAlbumTag(source)) ? data : "???",
		(data = Mix_GetMusicCopyrightTag(source)) ? data : "???"
	};
}

void Music::switchInto(usize const& fadeOutTime, usize const& fadeInTime) {
	if (!exists()) return;
	this->fadeInTime = fadeInTime;
	queueMusic();
	stopMusic(fadeOutTime);
}

void Music::switchInto(usize const& fadeOutTime, usize const& fadeInTime, int const& loops) {
	if (!exists()) return;
	this->fadeInTime	= fadeInTime;
	this->loops			= loops;
	queueMusic();
	stopMusic(fadeOutTime);
}

void Music::queueMusic() {
	if (!exists()) return;
	queued = &onQueue;
}

inline Extern::Resource Music::getSource() {
	return source;
}

void Music::onCreate(Extern::Resource const& data) final override {
	if (!isOpen()) throw Error::FailedAction("Failed to load file: Audio system is closed!");
	source = Mix_LoadMUS_RW(data, true);
	if (!source)
		throw Error::FailedAction(
			"Could not load file!",
			__FILE__,
			toString(__LINE__),
			"onCreate",
			String(Mix_GetError())
		);
}

void Music::onDestroy() final override {
	DEBUGLN("Deleting music source...");
	if (isOpen()) {
		if (queued == &onQueue)
			queued = nullptr;
		if (Mix_PlayingMusic() && current == source)
			Mix_HaltMusic();
		Mix_FreeMusic(source);
	}
	DEBUGLN("Music source deleted!");
}

void Music::onUpdate() final override {
	if (current != source) return;
}

static void Music::update() {
	if (!Mix_PlayingMusic()) {
		current = nullptr;
		if (queued) {
			(*queued)();
			queued = nullptr;
		}
	}
}
