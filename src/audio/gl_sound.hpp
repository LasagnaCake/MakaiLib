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

	void play(int loops = 0, size_t fadeInTime = 0, bool force = false) {
		if (!exists()) return;
		if (active() && !force) return;
		if (fadeInTime)
			channel = Mix_FadeInChannel(active() ? channel : -1, source, loops, fadeInTime);
		else
			channel = Mix_PlayChannel(active() ? channel : -1, source, loops);
	}

	void playOnceThisFrame(int loops = 0, size_t fadeInTime = 0, bool force = false) {
		if (!exists()) return;
		if (active() && !force) return;
		if (!canPlayThisFrame) return;
		canPlayThisFrame = false;
		if (fadeInTime)
			channel = Mix_FadeInChannel(getChannel(), source, loops, fadeInTime);
		else
			channel = Mix_PlayChannel(getChannel(), source, loops);
	}

	void stop(size_t fadeOutTime = 0) {
		if (!exists()) return;
		if (!active()) return;
		if (fadeOutTime)
			Mix_FadeOutChannel(channel, fadeOutTime);
		else
			Mix_HaltChannel(channel);
	}

	void pause() {
		if (!exists()) return;
		if (!active()) return;
		Mix_Pause(channel);
	}

	void resume() {
		if (!exists()) return;
		if (!active()) return;
		Mix_Resume(channel);
	}

	void setVolume(uchar volume) {
		if (!exists()) return;
		Mix_VolumeChunk(source, volume);
	}

	int getVolume() {
		if (!exists()) return -1;
		return Mix_VolumeChunk(source, -1);
	}

protected:
	void onCreate(String path) final override {
		if (isAudioSystemClosed) throw Error::FailedAction("Failed to load file: Audio system is closed!");
		data = FileLoader::getBinaryFile(path);
		source = Mix_LoadWAV_RW(SDL_RWFromMem(data.data(), data.size()), true);
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
		DEBUGLN("Destroying sound source...");
		if (!isAudioSystemClosed) {
			if (active())
				Mix_HaltChannel(channel);
			Mix_FreeChunk(source);
		}
		data.clear();
		DEBUGLN("Sound source deleted!");
	}

	void onUpdate() final override {
		canPlayThisFrame = true;
		if (!active()) {
			channel = -1;
			return;
		}
	}

private:
	bool canPlayThisFrame = true;

	int getChannel() {
		return active() ? channel : -1;
	}

	bool active() {
		if (!exists())		return false;
		if (channel == -1)	return false;
		return (
			Mix_Playing(channel)
		&&	Mix_GetChunk(channel) == source
		);
	}

	FileLoader::BinaryData data;

	int			channel = -1;
	Mix_Chunk*	source	= NULL;
};
