namespace {
	using namespace CollisionData;
}

struct ObjectParam {
	$twn EaseFunc easing = $twn ease.out.linear;
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

struct GenericObjectData {
	// Collision data
	CircleBounds2D	hitbox;
	// Flags
	bool discardable	= true;
	bool collidable		= true;
	// Destroy on Playfield Exit
	bool dope = true;
};

struct ObjectData: GenericObjectData {
	ObjectParam	vel;
	ObjectParam	rot;
};

typedef std::vector<float> ObjectFlags;

struct DanmakuObject {
	DanmakuObject() {
		auto pass = $tsignal(DanmakuObject*) {};
		onFree			= pass;
		onObjectFrame	= pass;
		onUnpause		= pass;
	}

	$ref AnimatedPlane* sprite = nullptr;

	$tts MultiTasker<DanmakuObject> taskers;

	$tev Signal<DanmakuObject*> onFree;
	$tev Signal<DanmakuObject*> onUnpause;
	$tev Signal<DanmakuObject*> onObjectFrame;

	ObjectFlags flags;

	float zIndex = 0;
	float zScale = 0;

	Transform2D local;

	Pause pause;

	virtual void onFrame(float delta) {
		if (free) return;
		onObjectFrame(this);
		if (pause.enabled) {
			if (pause.time < 0) return;
			pause.time = Math::clamp(pause.time - delta, 0.0f, 1.0f);
			if ((pause.time) > 0) return;
			pause.enabled = false;
			onUnpause(this);
		}
		taskers.yield(delta, this);
	}

	virtual DanmakuObject* reset() {return this;}

	virtual DanmakuObject* setZero() {return this;}

	virtual DanmakuObject* enable() {
		return setFree(false);
	}

	virtual DanmakuObject* setFree(bool state = true) {
		free = state;
		if (free) {
			onFree(this);
			flags.clear();
		}
		return this;
	}

	virtual DanmakuObject* clearSignals() {
		auto pass = $tsignal(DanmakuObject*) {};
		onObjectFrame = onFree = onUnpause = pass;
		return this;
	}

	virtual DanmakuObject* discard() {return this;}

	virtual void updateSprite() {}

	// INTERNAL USE ONLY DO NOT TOUCH
	void _setZOffset(float offset) {
		_zOffset = offset;
	}

	bool isFree() {
		return free;
	}

protected:
	bool free;

	float _zOffset = 0;
};

typedef std::vector<DanmakuObject*> ObjectList;

struct {
	float playerDamage = 1.0;
} defaults;
