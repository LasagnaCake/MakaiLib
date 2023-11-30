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

#define DANMAKU_OBJ_TASK	T_TASK(DanmakuObject* self)
#define DANMAKU_OBJ_SIGNAL	T_SIGNAL(DanmakuObject* self)

struct DanmakuObject {
	DanmakuObject() {
		auto pass = DANMAKU_OBJ_SIGNAL {};
		onFree			= pass;
		onObjectFrame	= pass;
		onUnpause		= pass;
		onSpawnBegin	= pass;
		onSpawnEnd		= pass;
		onDespawnBegin	= pass;
		onDespawnEnd	= pass;
	}

	RenderData::Reference::AnimatedPlane* sprite = nullptr;

	TypedTasking::MultiTasker<DanmakuObject> taskers;

	ObjectSignal onFree;
	ObjectSignal onUnpause;
	ObjectSignal onObjectFrame;
	ObjectSignal onSpawnBegin;
	ObjectSignal onSpawnEnd;
	ObjectSignal onDespawnBegin;
	ObjectSignal onDespawnEnd;

	ObjectFlags flags;

	float zIndex = 0;
	float zScale = 0;

	float spawnSpeed	= .2f;
	float despawnSpeed	= .1f;

	Transform2D local;

	Pause pause;

	Vector4 color = Color::WHITE;

	virtual void onFrame(float delta) {
		if (free) return;
		if (sprite) sprite->setColor(color * tint);
		onObjectFrame(this);
		if (spawning) {
			if (factor < 1.0) factor += spawnSpeed;
			else {
				factor = 1.0; spawning = false;
				onObjectSpawnEnd();
				onSpawnEnd(this);
			}
			spawnAnimation(factor);
		}
		if (despawning) {
			if (factor > 0.0f) factor -= despawnSpeed;
			else {
				factor = 0.0; despawning = false;
				onObjectDespawnEnd();
				onDespawnEnd(this);
				setFree(true);
			}
			despawnAnimation(factor);
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
		if (despawning || free) return this;
		spawning = true;
		factor = 0.0f;
		onObjectSpawnBegin();
		onSpawnBegin(this);
		return this;
	}

	DanmakuObject* despawn() {
		if (free) return this;
		despawning = true;
		spawning = false;
		factor = 1.0f;
		onObjectDespawnBegin();
		onDespawnBegin(this);
		return this;
	}

	DanmakuObject* clearAnimation() {
		spawning = despawning = false;
		factor = 1.0;
		clearAnimationData();
		return this;
	}

	virtual DanmakuObject* reset() {
		return this;
	}

	virtual DanmakuObject* setZero() {
		clearAnimation();
		zIndex = 0;
		zScale = 0;
		tint = Color::WHITE;
		spawnSpeed		= .2f;
		despawnSpeed	= .1f;
		return this;
	}

	virtual DanmakuObject* enable() {
		return setFree(false);
	}

	virtual DanmakuObject* setFree(bool state = true) {
		free = state;
		if (free) {
			onFree(this);
			flags.clear();
			spawning = despawning = false;
		}
		local.position = Vector2(-64, 64);
		return this;
	}

	virtual DanmakuObject* clearSignals() {
		auto pass = DANMAKU_OBJ_SIGNAL {};
		onObjectFrame	= pass;
		onFree			= pass;
		onUnpause		= pass;
		onSpawnBegin	= pass;
		onSpawnEnd		= pass;
		onDespawnBegin	= pass;
		onDespawnEnd	= pass;
		return this;
	}

	virtual DanmakuObject* discard()		{return this;}
	virtual DanmakuObject* forceDiscard()	{return this;}

	virtual void updateSprite() = 0;

	bool isFree() {
		return free;
	}

protected:
	virtual void onObjectSpawnBegin()	{}
	virtual void onObjectSpawnEnd()		{}
	virtual void onObjectDespawnBegin()	{}
	virtual void onObjectDespawnEnd()	{}

	virtual void spawnAnimation(float t)	{tint.a = (factor * 0.5) + 0.5;	}
	virtual void despawnAnimation(float t)	{tint.a = factor;				}
	virtual void clearAnimationData()		{tint.a = 1;					}

	Vector4 tint = Color::WHITE;

	float factor = 1.0f;

	bool despawning = false;

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
