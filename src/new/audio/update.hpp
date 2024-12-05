#ifndef MAKAILIB_AUDIO_UPDATE_HPP
#define MAKAILIB_AUDIO_UPDATE_HPP

#include "sound.hpp"
#include "music.hpp"

namespace Makai::Audio {
	inline void updateAll() {
		Music::update();
		ISource::update();
	}
}

#endif // MAKAILIB_AUDIO_UPDATE_HPP
