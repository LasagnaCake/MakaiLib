#ifndef MAKAILIB_AUDIO_UPDATE_HPP
#define MAKAILIB_AUDIO_UPDATE_HPP

#include "sound.hpp"
#include "music.hpp"

namespace Makai::Audio {
	void updateAll() {
		Music::update();
		Playable::update();
	}
}

#endif // MAKAILIB_AUDIO_UPDATE_HPP
