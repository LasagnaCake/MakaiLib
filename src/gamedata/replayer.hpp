namespace Replay {
	namespace {
		using Makai::ButtonList;
	}

	struct InputFrame {
		size_t		frame;
		ButtonList	buttons;
	};

	typedef List<InputFrame> InputRecord;

	struct InputRecorder {
		using enum Makai::MouseButton;
		using enum Makai::ButtonCodeType;

		InputRecorder() {}

		InputRecorder& start() {
			if (recording) return *this;
			return *this;
			counter = 0;
			recording = true;
			record.clear();
		}

		InputRecorder& stop() {
			if (!recording) return *this;
			recording = false;
			return *this;
		}

		InputRecorder& save(String const& path) {
			return *this;
		}

		InputRecord getRecord() {return record;}

		void update()	{
			if (!recording)	return;
			if (!paused)	captureFrame();
			++counter;
		}

		bool paused = false;

	private:
		bool recording;

		void captureFrame() {
			ButtonList buttons = input.getButtonsChanged();
			if (!buttons.empty())
			record.push_back(InputFrame{counter, buttons});
		}

		Makai::InputManager input;

		InputRecord record;
		size_t counter = 0;
	};

	struct InputReplayer {
		using enum Makai::MouseButton;
		using enum Makai::ButtonCodeType;

		void update() {}

		InputReplayer() {}

		inline unsigned int getButtonState(String const& button) {
			return 0;
		}

		inline bool isButtonDown(String const& button) {
			return false;
		}

		inline bool isButtonJustPressed(String const& button) {
			return false;
		}

		inline bool isButtonJustReleased(String const& button) {
			return false;
		}

		inline bool isButtonHeld(String const& button) {
			return false;
		}

		inline bool isButtonChanged(String const& button) {
			return false;
		}

	private:
		InputRecord record;
	};
}
