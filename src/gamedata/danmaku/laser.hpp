#define $laser(TYPE)	$getman( TYPE##Laser )
#define $setl(TYPE)		$setman( TYPE##Laser )

typedef ObjectParam	LaserParam;

struct LaserData: ObjectData {
	bool enabled = false;
};

struct LineLaserData: LaserData {
};

struct BentLaserData: LaserData {
};

class LineLaser: public DanmakuObject {
public:
	LineLaser(): DanmakuObject() {
		auto pass = $tsignal(LineLaser*) {};
	}

	LineLaserData params;

	void onFrame(float delta) override {
		DanmakuObject::onFrame(delta);
		params.vel.factor = Math::clamp(params.vel.factor + params.vel.omega, 0.0f, 1.0f);
		params.rot.factor = Math::clamp(params.rot.factor + params.rot.omega, 0.0f, 1.0f);
		params.vel.current = Math::lerp(
			params.vel.start,
			params.vel.end,
			params.vel.factor
		);
		params.rot.current = Math::lerp(
			params.rot.start,
			params.rot.end,
			params.rot.factor
		);
		if (params.vel.current)
			local.position += VecMath::angleV2(params.rot.current) * params.vel.current * delta;
		local.rotation = params.rot.current;
		updateSprite();
	}

	LineLaser* reset() override {
		setZero();
		params.vel.current = params.vel.start;
		local.rotation =
		params.rot.current = params.rot.start;
		params.hitbox.position = local.position;
		updateSprite();
		sprite->local.rotation.z = local.rotation;
		return this;
	}

	LineLaser* setZero() override {
		params.vel.current =
		local.rotation =
		params.rot.current =
		params.vel.factor =
		params.rot.factor = 0;
		return this;
	}

	LineLaser* enable() override {
		return setFree(false);
	}

	LineLaser* setFree(bool state = true) override {
		DanmakuObject::setFree(state);
		if (sprite) sprite->visible = !free;
		return this;
	}

	LineLaser* discard() override {
		if (params.discardable)
			setFree();
		return this;
	}

	void updateSprite() override {
	}
};

typedef std::vector<LineLaser*> LineLaserList;
