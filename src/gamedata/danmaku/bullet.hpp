#ifndef MAKAI_BASE_BULLET_H
#define MAKAI_BASE_BULLET_H

namespace {
	using namespace CollisionData;
}

struct BulletParam {
	float start		= 0;
	float end		= 0;
	float omega		= 0;
	bool constant	= false;
};

struct BulletData {
	// Collision data
	BoxBounds2D*	playfield;
	CircleBounds2D	hitbox;
	// Parameters
	BulletParam	speed;
	BulletParam	rotation;
	// Flags
	bool rotateSprite	= true;
	bool shuttle		= true;
	bool rebound		= true;
	bool destroyable	= true;
	bool collidable		= true;
	// Destroy on Playfield Exit
	bool dope = true;
};

class Bullet {
public:
	Bullet() {
	}

	BulletData settings;

	$ref AnimatedPlane* sprite = nullptr;

	$tsk MultiTasker taskers;

	$evt Signal onFree = Event::DEF_SIGNAL;

	void onFrame(float delta) {
		if (free) return;
		taskers.yield();
		speedFactor = Math::clamp(speedFactor + settings.speed.omega, 0.0f, 1.0f);
		rotationFactor = Math::clamp(rotationFactor + settings.rotation.omega, 0.0f, 1.0f);
		currentSpeed = Math::lerp(
			settings.speed.start,
			settings.speed.end,
			speedFactor
		);
		currentRotation = Math::angleLerp(
			settings.rotation.start,
			settings.rotation.end,
			rotationFactor
		);
		if (currentSpeed)
			local.position += VecMath::angleV2(currentRotation) * currentSpeed * delta;
		if (sprite) {
			// Set sprite position
			sprite->local.position = Vector3(local.position);
			settings.hitbox.position = local.position;
			// Set sprite rotation
			sprite->local.rotation.z = local.rotation;
			if (settings.rotateSprite)
				sprite->local.rotation.z += currentRotation;
			// Set sprite scale
			sprite->local.scale = Vector3(local.scale);
		}
	}

	Bullet* reset() {
		currentSpeed = settings.speed.start;
		local.rotation = currentRotation = settings.rotation.start;
		return this;
	}

	Bullet* enable() {
		return setFree(false);
	}

	bool isFree() {
		return free;
	}

	Bullet* setFree(bool state = true) {
		free = state;
		if (sprite) sprite->visible = !free;
		if (free) onFree();
		return this;
	}

	Transform2D local;

	float currentSpeed		= 0;
	float currentRotation	= 0;

private:
	float speedFactor		= 0;
	float rotationFactor	= 0;

	bool free = true;
};

template <size_t BULLET_COUNT>
struct BulletManager: Entity {
	DERIVED_CONSTRUCTOR(BulletManager, Entity, {
		$ecl $_ROOT += this;
		mesh.setRenderLayer($layer(ENEMY_BULLET));
		$ecl groups.addEntity(this, $layer(ENEMY_BULLET));
	})

	DERIVED_CLASS(BulletManager, Entity)

	Renderable mesh;

	void onDelete() override {
		$ecl groups.removeFromAll(this);
	}

	void onFrame(float delta) override {
		for $each(b, bullets) {
			b.onFrame(delta);
			if (!b.isFree() && b.settings.collidable)
			for $each(player, $ecl groups.getGroup($layer(PLAYER))) {
				auto p = ((AreaCircle2D*)player);
				if (
					p->collision.enabled
					&& CollisionData::withinBounds(
						b.settings.hitbox,
						p->getCircleBounds()
					)
				) p->onCollision(this);
			}
		}
	}

	void create() {
		if (created) return;
		haltProcedure = false;
		for (size_t i = 0; i < BULLET_COUNT; i++) {
			if (!bullets[i].sprite)
				bullets[i].sprite = mesh.createReference<AnimatedPlane>();
			bullets[i].setFree();
			onBulletCreated();
			if (haltProcedure) return;
		}
		created = true;
	}

	Bullet* getLastBullet() {
		return last;
	}

	Bullet* createBullet() {
		for $each(b, bullets)
			if (b.isFree()) {
				last = b.enable()->reset();
				last->taskers.clearTaskers();
				return last;
			}
		throw std::runtime_error(
			getName()
			+ ": Out of usable bullets ("
			+ std::to_string(BULLET_COUNT)
			+ ")!"
		);
	}

	Bullet* createBullet(BulletData bullet) {
		Bullet* b = createBullet();
		b->settings = bullet;
		return b->reset();
	}

	Bullet bullets[BULLET_COUNT];

	$evt Signal onBulletCreated = $evt DEF_SIGNAL;

	bool haltProcedure = false;

private:
	bool created = false;

	Bullet* last = nullptr;
};

#endif // MAKAI_BASE_BULLET_H
