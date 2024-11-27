#ifndef MAKAILIB_AUDIO_CORE_H
#define MAKAILIB_AUDIO_CORE_H

#include "../compat/ctl.hpp"

namespace Makai::Audio {
	void stopAll();

	enum class Format {
		AF_NONE,
		AF_MP3,
		AF_OGG,
		AF_MIDI,
		AF_FLAC,
		AF_MOD,
		AF_OPUS,
	//	AF_WAVPACK,
		AF_MAX_FORMATS
	};

	using Formats = List<Format>;

	void open(Formats const& formats, uint const channels = 2, uint const audioTracks = 16);

	void restart(Formats const& formats, uint const channels = 2, uint const audioTracks = 16);

	void close();

	bool isOpen();
}

#endif // MAKAILIB_AUDIO_CORE_H
