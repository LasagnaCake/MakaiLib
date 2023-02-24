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

class Sound: public Base::Playable {
public:
	using Playable::Playable;

	virtual ~Sound() final {destroy();}

	void onCreate(String path) final override {
		source = Mix_LoadWAV(path.c_str());
		if (!source)
			throw std::runtime_error("Could not load file '" + path + "'!\n\n REASON: " + String(Mix_GetError()));
	}

	void onDestroy() final override {
		if (channel != -1 && Mix_Playing(channel))
			Mix_HaltChannel(channel);
		Mix_FreeChunk(source);
		$debug("Source deleted!");
	}

	void onUpdate() override {
		if (!active()) {
			channel = -1;
			return;
		}
		Mix_Volume(channel, volume);
	}

	void play(int loops = 0, size_t fadeInTime = 0, bool force = false) {
		if (active() && !force) return;
		if (fadeInTime)
			channel = Mix_FadeInChannel(-1, source, loops, fadeInTime);
		else
			channel = Mix_PlayChannel(-1, source, loops);
	}

	void stop(size_t fadeOutTime = 0) {
		if (!active()) return;
		if (fadeOutTime)
			Mix_FadeOutChannel(channel, fadeOutTime);
		else
			Mix_HaltChannel(channel);
	}

	void pause() {
		if (!active()) return;
		Mix_Pause(channel);
	}

	void resume() {
		if (!active()) return;
		Mix_Resume(channel);
	}

	uint volume = 128;

private:
	bool active() {
		if (channel == -1) return false;
		return (
			Mix_GetChunk(channel) == source
		&&	Mix_Playing(channel)
		);
	}

	int			channel = -1;
	Mix_Chunk*	source;
};
