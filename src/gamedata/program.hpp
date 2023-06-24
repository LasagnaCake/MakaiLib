class GameApp;

namespace {
	GameApp* mainProgram = nullptr;
}

class GameApp: public Program {
public:
	GameApp(
		unsigned int width,
		unsigned int height,
		std::string windowTitle,
		bool fullscreen = false,
		bool useMIDI = false,
		std::string bufferShaderPath = "shaders/framebuffer/compose.slf",
		std::string mainShaderPath = "shaders/base/base.slf"
	) : Program (
			width,
			height,
			windowTitle,
			fullscreen,
			useMIDI,
			bufferShaderPath,
			mainShaderPath
	) {
		if (mainProgram)
			throw Error::DuplicateValue("Only one program can exist at a time!");
		mainProgram = this;
	};

	BufferMaterial&	frame = getFrameBuffer().material;

	std::map<size_t, BufferMaterial> layers;

	WorldMaterial world2D;
	WorldMaterial world3D;

	$cam Camera3D cam2D;
	$cam Camera3D cam3D{$vec3(0, 5, -10), $vec3(0, 0, 0)};

	size_t gameSeed = 0;

	inline void setCamera2D() {
		$scn camera = cam2D;
	}

	inline void setCamera3D() {
		$scn camera = cam3D;
	}

	inline void setWorldMaterial2D() {
		setWorldMaterial(world2D);
	}

	inline void setWorldMaterial3D() {
		setWorldMaterial(world3D);
	}

	void setLayerMaterial(size_t layerID) {
		getLayerBuffer().material = layers[layerID];
	}

	virtual ~GameApp() {
		mainProgram = nullptr;
	}

	void onOpen() override {
		// Set RNG Seed
		gameSeed = $rng getNewSeed();
		$debug(gameSeed);
		$rng setSeed(gameSeed);
		// Create 2D Camera
		Vector2 screenSpace = getWindowScale();
		cam2D = $cam getCamera2D(64, screenSpace);
		// Set 2D Camera
		setCamera2D();
	}

	void onLayerDrawBegin(size_t layerID) override {
		setLayerMaterial(layerID);
	}

	void onClose() override {}
};

struct ProgramSetting {
	Vector2	resolution;
	float	framerate;
	bool	fullscreen;
};

#define USER_QUIT throw Error::FailedAction("User quit the setup process!")

ProgramSetting queryProgramSettingsFromUser(bool use16by9 = false, bool letUserChooseFramerate = true, bool extendedFramerate = false) {
	StringList resList;
	std::vector<Vector2> resValue;
	for $each(res, (use16by9 ? $res set16x9 : $res set4x3)) {
		resList.push_back(res.first);
		resValue.push_back(res.second);
	}
	resList.push_back("Detect");
	Vector2 devSize = Makai::getDeviceSize();
	$debug(devSize.y * (4.0 / 3.0));
	$debug(devSize.y);
	$debug(devSize.y * (16.0 / 9.0));
	$debug(devSize.y);
	if (Popup::dialogBox(
		"Warning!",
		"Pressing escape on any of the next set of dialog boxes,\n"
		"including this one, will close the application.\n"
		"Do you understand?",
		Popup::Option::YES,
		SDL_MESSAGEBOX_WARNING
	) < 0) USER_QUIT;
	int winSize = Popup::dialogBox(
		std::string("App Configuration (1/") + (letUserChooseFramerate ? "3)" : "2)"),
		"Please select a window size.\n"
		"Selecting \"Detect\" will set to your screen's size.\n\n"
		"WARNING: Selecting \"Detect\" will set the application to fullscreen!\n",
		resList
	);
	if (winSize < 0) USER_QUIT;
	int fullscreen = 0;
	if (winSize != resList.size() - 1) {
		fullscreen = Popup::dialogBox(
			std::string("App Configuration (2/") + (letUserChooseFramerate ? "3)" : "2)"),
			"Would you like to run this application in fullscreen or windowed mode?",
			StringList{"Fullscreen", "Windowed"}
		);
	}
	if (fullscreen < 0) USER_QUIT;
	int framerate = 1;
	if (letUserChooseFramerate) {
		framerate = Popup::dialogBox(
			"App Configuration (3/3)",
			"Please select the maximum framerate (frames per second).\n"
			"Higher framerates require more powerful computers.\n"
			"If the program is running slow, try changing the framerate.",
			extendedFramerate
			? StringList{"10", "20", "30", "40", "50", "60"}
			: StringList{"30", "60"}
		);
		if (framerate < 0) USER_QUIT;
	}
	Vector2 window;
	if (winSize == resList.size() - 1) {
		window = devSize;
		window.x = Math::round(window.y * (use16by9 ? 16.0/9.0 : 4.0/3.0));
	} else
		window = resValue[winSize];
	float frate = 0;
	if (extendedFramerate)
		frate =  10 + (framerate * 10);
	else frate = 30 + (framerate * 30);
	return ProgramSetting{
		window,
		frate,
		!fullscreen
	};
}


GameApp* getMainProgram() {
	if (!mainProgram) return nullptr;
	return mainProgram;
}

#undef USER_QUIT
