#include "playable.hpp"

#include "../file/get.hpp"

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <windows.h>
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

using namespace Makai::Audio;

List<AudioCallback*> updateQueue;

Playable::Playable() {
	updateQueue.push_back(&yield);
}

Playable::Playable(String const& path): Playable() {
	create(path);
}

Playable::~Playable() {
	DEBUGLN("Deleting playable object...");
	ERASE_IF(updateQueue, elem == &yield);
	destroy();
	DEBUGLN("Object deleted!");
}

void Playable::create(String const& path) {
	if (created) return;
	data = Makai::File::getBinaryFile(path);
	onCreate(SDL_RWFromConstMem(data.data(), data.size()));
	created = true;
};

void Playable::destroy() {
	if (!created) return;
	onDestroy();
	data.clear();
	created = false;
};

void Playable::update() {
	for(AudioCallback*& c : updateQueue) (*c)();
}
