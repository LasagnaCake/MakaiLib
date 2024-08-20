#ifndef MAKAILIB_AUDIO_CORE_H
#define MAKAILIB_AUDIO_CORE_H

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
		AF_WAVPACK,
		AF_MAX_FORMATS
	};

	void open(List<Format> const& formats, uint const& channels = 2, uint const& audioTracks = 16);

	void close();

	bool isOpen();
}

#endif // MAKAILIB_AUDIO_CORE_H
