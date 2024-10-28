#include "source.hpp"

#include "../file/get.hpp"

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <windows.h>
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

using namespace Makai; using namespace Makai::Audio;

List<AudioCallback*> updateQueue;

Source::Source() {
	updateQueue.pushBack(&yield);
}

Source::Source(String const& path): Source() {
	create(path);
}

Source::~Source() {
	DEBUGLN("Deleting playable object...");
	updateQueue.eraseLike(&yield);
	destroy();
	DEBUGLN("Object deleted!");
}

void Source::create(String const& path) {
	if (created) return;
	data = Makai::File::getBinary(path);
	onCreate(SDL_RWFromConstMem(data.data(), data.size()));
	created = true;
};

void Source::destroy() {
	if (!created) return;
	onDestroy();
	data.clear();
	created = false;
};

void Source::update() {
	for(AudioCallback*& c : updateQueue) (*c)();
}

bool Source::exists() {
	return created;
}
