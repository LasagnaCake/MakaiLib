namespace {
	Mix_Music*	current	= nullptr;
	AudioFunc*	queued	= nullptr;
}

void update() {
	if (!Mix_PlayingMusic()) {
		current = nullptr;
		if (queued) {
			(*queued)();
			queued = nullptr;
		}
	}
}

void stopMusic(size_t fadeOutTime = 0) {
	if (fadeOutTime)
		Mix_FadeOutMusic(fadeOutTime);
	else
		Mix_HaltMusic();
}

void pauseMusic() {
	Mix_PauseMusic();
}

void resumeMusic() {
	Mix_ResumeMusic();
}

void setVolume(signed char volume) {
	Mix_VolumeMusic(volume);
}

signed char getVolume() {
	return Mix_VolumeMusic(-1);
}

struct SongMetaData {
	string	title;
	string	artist;
	string	album;
	string	copyright;
};

class Music: public Base::Playable {
public:
	using Playable::Playable;

	virtual ~Music() final {destroy();}

	void play(int const& loops = 0, size_t const& fadeInTime = 0) {
		if (!exists()) return;
		this->loops = loops;
		current	= source;
		if (fadeInTime)
			Mix_FadeInMusic(source, loops, fadeInTime);
		else
			Mix_PlayMusic(source, loops);
	}

	String getTitle() {
		return String(Mix_GetMusicTitle(source));
	}

	SongMetaData getMetaData() {
		const char* data;
		return SongMetaData {
			(data = Mix_GetMusicTitleTag(source)) ? data : "",
			(data = Mix_GetMusicArtistTag(source)) ? data : "",
			(data = Mix_GetMusicAlbumTag(source)) ? data : "",
			(data = Mix_GetMusicCopyrightTag(source)) ? data : ""
		};
	}

	void switchInto(size_t const& fadeOutTime, size_t const& fadeInTime) {
		if (!exists()) return;
		this->fadeInTime = fadeInTime;
		queueMusic();
		stopMusic(fadeOutTime);
	}

	void switchInto(size_t const& fadeOutTime, size_t const& fadeInTime, int const& loops) {
		if (!exists()) return;
		this->fadeInTime	= fadeInTime;
		this->loops			= loops;
		queueMusic();
		stopMusic(fadeOutTime);
	}

	void queueMusic() {
		if (!exists()) return;
		queued = &onQueue;
	}

	inline Mix_Music* getSource() {
		return source;
	}

protected:
	void onCreate(String const& path) final override {
		if (isAudioSystemClosed) throw Error::FailedAction("Failed to load file: Audio system is closed!");
		data = FileLoader::getBinaryFile(path);
		source = Mix_LoadMUS_RW(SDL_RWFromMem(data.data(), data.size()), true);
		if (!source)
			throw Error::FailedAction(
				"Could not load file '" + path + "'!",
				"gl_music.hpp",
				"46",
				"Music::onCreate",
				String(Mix_GetError())
			);
	}

	void onDestroy() final override {
		DEBUGLN("Deleting music source...");
		if (!isAudioSystemClosed) {
			if (queued == &onQueue)
				queued = nullptr;
			if (Mix_PlayingMusic() && current == source)
				Mix_HaltMusic();
			Mix_FreeMusic(source);
		}
		data.clear();
		DEBUGLN("Music source deleted!");
	}

	void onUpdate() final override {
		if (current != source) return;
	}

private:
	size_t	fadeInTime	= 0;
	int		loops		= 0;

	AudioFunc onQueue = SIGNAL {
		play(loops, fadeInTime);
	};

	FileLoader::BinaryData data;

	Mix_Music* source = NULL;
};
