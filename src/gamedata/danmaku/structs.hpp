namespace {
	using namespace CollisionData;
}

struct ObjectParam {
	Tween::EaseFunc easing = Tween::ease.out.linear;
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

struct MODULAR GenericObjectData {
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
	ObjectParam stretch;
	bool rotateHitbox	= true;
};

typedef std::vector<float> ObjectFlags;

struct DanmakuObject;

typedef TypedSignal<DanmakuObject*> ObjectSignal;

typedef TypedTasking::Task<DanmakuObject> ObjectTask;

#define DANMAKU_OBJ_TASK	T_TASK(DanmakuObject*)
#define DANMAKU_OBJ_SIGNAL	T_SIGNAL(DanmakuObject*)

struct DanmakuObject {
	DanmakuObject() {
		auto pass = DANMAKU_OBJ_SIGNAL {};
		onFree			= pass;
		onObjectFrame	= pass;
		onUnpause		= pass;
		onSpawnEnd		= pass;
	}

	RenderData::Reference::AnimatedPlane* sprite = nullptr;

	TypedTasking::MultiTasker<DanmakuObject> taskers;

	ObjectSignal onFree;
	ObjectSignal onUnpause;
	ObjectSignal onObjectFrame;
	ObjectSignal onSpawnEnd;

	ObjectFlags flags;

	float zIndex = 0;
	float zScale = 0;

	float spawnSpeed = .2f;

	Transform2D local;

	Pause pause;

	Vector4 color = Color::WHITE;

	virtual void onFrame(float delta) {
		if (free) return;
		if (sprite) sprite->setColor(color * tint);
		onObjectFrame(this);
		if (spawning) {
			if (tint.a < 1.0) tint.a += spawnSpeed;
			else {tint.a = 1.0; spawning = false;}
			onObjectSpawnEnd();
			onSpawnEnd(this);
		}
		if (pause.enabled) {
			if (pause.time < 0) return;
			pause.time = Math::clamp(pause.time - delta, 0.0f, 1.0f);
			if ((pause.time) > 0) return;
			pause.enabled = false;
			onUnpause(this);
		}
		taskers.yield(delta, this);
	}

	DanmakuObject* spawn() {
		spawning = true;
		tint.a = 0.0f;
		onObjectSpawnBegin();
		return this;
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
		local.position = 0;
		return this;
	}

	virtual DanmakuObject* clearSignals() {
		auto pass = T_SIGNAL(DanmakuObject*) {};
		onSpawnEnd = onObjectFrame = onFree = onUnpause = pass;
		return this;
	}

	virtual DanmakuObject* discard() {return this;}

	virtual void updateSprite() {}

	bool isFree() {
		return free;
	}

protected:
	virtual void onObjectSpawnBegin()	{}
	virtual void onObjectSpawnEnd()		{}

	Vector4 tint = Color::WHITE;

	bool spawning = false;

	bool free = true;

	float zOffset = 0;

	friend class DanmakuObjectManager;
};

struct DanmakuObjectManager: Entity {
	DERIVED_CLASS(DanmakuObjectManager, Entity)
	DERIVED_CONSTRUCTOR(DanmakuObjectManager, Entity, {addToGame(this, "DanmakuGame");})
};

typedef std::vector<DanmakuObject*> ObjectList;

struct {
	struct {
		float bullet	= 2.0;
		float laser		= 1.0;
		float bomb		= 5.0;
	} playerDamage;
} defaults;

ErrorType(OutOfObjects);
