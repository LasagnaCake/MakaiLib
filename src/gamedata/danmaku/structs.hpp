namespace {
	using namespace CollisionData;
}

struct ObjectParam {
	float start		= 0;
	float end		= 0;
	float omega		= 0;
	float factor	= 0;
	float current	= 0;
};

struct Pause {
	long time		= -1;
	bool enabled	= false;
};

struct ObjectData {
	// Collision data
	CircleBounds2D	hitbox;
	// Parameters
	ObjectParam	vel;
	ObjectParam	rot;
	// Flags
	bool discardable	= true;
	bool collidable		= true;
	// Destroy on Playfield Exit
	bool dope = true;
};

struct DanmakuObject {
	DanmakuObject() {
		auto pass = $tsignal(Object*) {};
		onFree			= pass;
		onObjectFrame	= pass;
		onUnpause		= pass;
	}

	$ref AnimatedPlane* sprite = nullptr;

	$tts MultiTasker<DanmakuObject> taskers;

	$tev Signal<DanmakuObject*> onFree;
	$tev Signal<DanmakuObject*> onUnpause;
	$tev Signal<DanmakuObject*> onObjectFrame;

	float zIndex = 0;
	float zScale = 0;

	Transform2D local;

	Pause pause;

	virtual void onFrame(float delta) {
		if (free) return;
		onObjectFrame(this);
		if (pause.enabled) {
			if (pause.time < 0) return;
			if ((--pause.time) > 0) return;
			pause.enabled = false;
			onUnpause(this);
		}
		taskers.yield(this);
	}

	virtual DanmakuObject* reset() {return this;}

	virtual DanmakuObject* setZero() {return this;}

	virtual DanmakuObject* enable() {
		return setFree(false);
	}

	virtual DanmakuObject* setFree(bool state = true) {
		free = state;
		if (free) onFree(this);
		return this;
	}

	virtual DanmakuObject* discard() {return this;}

	virtual void updateSprite() {}

	bool isFree() {
		return free;
	}

protected:
	bool free;
};

typedef std::vector<DanmakuObject*> ObjectList;
