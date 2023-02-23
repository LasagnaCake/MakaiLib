class Sound: public Base::Playable {
public:
	using Playable::Playable;

	virtual ~Sound() final {destroy();}

	void onCreate(String path) final override {
	}

	void onDestroy() final override {
	}

	void onUpdate() override {
	}

	void play(int loops = 0, size_t fadeInTime = 0) {
	}

private:
	Mix_Chunk* source;
};
