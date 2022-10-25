#define $laser(TYPE)	$getman( TYPE##Laser )
#define $setl(TYPE)		$setman( TYPE##Laser )

typedef ObjectParam	LaserParam;

struct LaserData: ObjectData {
	float length	= 1.0;
	float width		= 1.0;
	bool enabled	= false;
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

	$ref AnimatedPlane* head = nullptr;
	$ref AnimatedPlane* tail = nullptr;
	LineLaserData params;

	$twn EaseFunc interpolate = $twn ease.out.linear;

	void onFrame(float delta) override {
		DanmakuObject::onFrame(delta);
		params.vel.factor = Math::clamp(params.vel.factor + params.vel.omega, 0.0f, 1.0f);
		params.rot.factor = Math::clamp(params.rot.factor + params.rot.omega, 0.0f, 1.0f);
		params.vel.current = params.vel.easing(
			params.vel.factor,
			params.vel.start,
			params.vel.end,
			1.0f
		);
		params.rot.current = params.rot.easing(
			params.rot.factor,
			params.rot.start,
			params.rot.end,
			1.0f
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
		setLaserShape();
		updateSprite();
		return this;
	}

	LineLaser* setZero() override {
		params.vel.current =
		local.rotation =
		params.rot.current =
		params.vel.factor =
		params.width =
		params.length =
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

	void setLaserShape() {
		if (!sprite) return;
		float hWidth = params.width / 2;
		$vec2
		p1 = $vec2(0, hWidth),
		p2 = $vec2(params.length, -hWidth);
		sprite->setOrigin(
			$vec3(p1),
			$vec3(p2.x, p1.y),
			$vec3(p1.x, p2.y),
			$vec3(p2)
		);
		if (head) {
			head->setOrigin(
				$vec3(-hWidth, hWidth),
				$vec3(hWidth, hWidth),
				$vec3(hWidth, -hWidth),
				$vec3(-hWidth, -hWidth)
			);
		}
		if (tail) {
			tail->setOrigin(
				$vec3(-hWidth + params.length, hWidth),
				$vec3(hWidth + params.length, hWidth),
				$vec3(hWidth + params.length, -hWidth),
				$vec3(-hWidth + params.length, -hWidth)
			);
		}
	}

	void updateSprite() override {
		if (!sprite) return;
		// Copy position data to sprite
		sprite->local.position		= $vec3(local.position, zIndex);
		sprite->local.rotation.z	= local.rotation;
		sprite->local.scale			= Vector3(local.scale, zScale);
		// Ditto for head
		if (head) {
			head->local.position	= $vec3(local.position, zIndex);
			head->local.rotation.z	= local.rotation;
			head->local.scale		= Vector3(local.scale, zScale);
		}
		// Ditto for tail
		if (tail) {
			tail->local.position	= $vec3(local.position, zIndex);
			tail->local.rotation.z	= local.rotation;
			tail->local.scale		= Vector3(local.scale, zScale);
		}
	}
};

typedef std::vector<LineLaser*> LineLaserList;
