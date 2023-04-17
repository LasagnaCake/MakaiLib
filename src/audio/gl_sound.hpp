void stopAllChannels(size_t fadeOutTime = 0) {
	if (fadeOutTime)
		Mix_FadeOutChannel(-1, fadeOutTime);
	else
		Mix_HaltChannel(-1);
}

void pauseAllChannels() {
	Mix_Pause(-1);
}

void resumeAllChannels() {
	Mix_Resume(-1);
}

void setMasterVolume(uchar volume) {
	Mix_Volume(-1, volume);
}

schar getMasterVolume() {
	return Mix_Volume(-1, -1);
}

class Sound: public Base::Playable {
public:
	using Playable::Playable;

	virtual ~Sound() final {destroy();}

	void onCreate(String path) final override {
		source = Mix_LoadWAV(path.c_str());
		if (!source)
			throw Error::FailedAction(
				"Could not load file '" + path + "'!",
				"gl_sound.hpp",
				"31",
				"Sound::onCreate",
				String(Mix_GetError())
			);
	}

	void onDestroy() final override {
		$debug("Destroying sound source...");
		if (!isAudioSystemClosing) {
			if (active())
				Mix_HaltChannel(channel);
		}
		Mix_FreeChunk(source);
		$debug("Sound source deleted!");
	}

	void onUpdate() override {
		if (!active()) {
			channel = -1;
			return;
		}
	}

	void play(int loops = 0, size_t fadeInTime = 0, bool force = false) {
		if (!created) return;
		if (active() && !force) return;
		if (fadeInTime)
			channel = Mix_FadeInChannel(-1, source, loops, fadeInTime);
		else
			channel = Mix_PlayChannel(-1, source, loops);
	}

	void stop(size_t fadeOutTime = 0) {
		if (!created) return;
		if (!active()) return;
		if (fadeOutTime)
			Mix_FadeOutChannel(channel, fadeOutTime);
		else
			Mix_HaltChannel(channel);
	}

	void pause() {
		if (!created) return;
		if (!active()) return;
		Mix_Pause(channel);
	}

	void resume() {
		if (!created) return;
		if (!active()) return;
		Mix_Resume(channel);
	}

	void setVolume(uchar volume) {
		if (!created) return;
		Mix_VolumeChunk(source, volume);
	}

	int getVolume() {
		if (!created) return -1;
		return Mix_VolumeChunk(source, -1);
	}

private:
	bool active() {
		if (!created)		return false;
		if (channel == -1)	return false;
		return (
			Mix_Playing(channel)
		&&	Mix_GetChunk(channel) == source
		);
	}

	int			channel = -1;
	Mix_Chunk*	source;
};
