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

	void open(Format* const& formats, usize const& sz, uint const& channels = 2, uint const& audioTracks = 16);

	template<usize N>
	void open(Decay::AsType<Format[N]> const& formats, uint const& channels = 2, uint const& audioTracks = 16) {
		open(formats, N, channels, audioTracks);
	}

	void close();

	bool isOpen();
}

#endif // MAKAILIB_AUDIO_CORE_H
