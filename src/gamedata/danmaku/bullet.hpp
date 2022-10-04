#ifndef MAKAI_BASE_BULLET_H
#define MAKAI_BASE_BULLET_H

namespace {
	using namespace CollisionData;
}

struct BulletParam {
	float start		= 0;
	float end		= 0;
	float omega		= 0;
};

struct BulletData {
	// Collision data
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
		currentRotation = Math::lerp(
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
		setZero();
		currentSpeed = settings.speed.start;
		local.rotation =
		currentRotation = settings.rotation.start;
		settings.hitbox.position = local.position;
		return this;
	}

	Bullet* setZero() {
		currentSpeed =
		local.rotation =
		currentRotation =
		speedFactor =
		rotationFactor = 0;
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

template <
	size_t BULLET_COUNT,
	class BULLET_TYPE,
	class = std::enable_if<std::is_base_of<Bullet, BULLET_TYPE>::value>::type
>
struct BulletManager: Entity {
	DERIVED_CONSTRUCTOR(BulletManager, Entity, {
		$ecl $_ROOT += this;
		mesh.setRenderLayer($layer(ENEMY_BULLET));
		$ecl groups.addEntity(this, $layer(ENEMY_BULLET));
	})

	$cdt BoxBounds2D playfield;

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
					&& $cdt withinBounds(
						b.settings.hitbox,
						p->getCircleBounds()
					)
				) p->onCollision(this);
			}
			if (b.settings.dope)
				if (
					! $cdt withinBounds(
						b.settings.hitbox,
						playfield
					)
				) {
					b.setFree(true);
				}
		}
	}

	size_t getFreeCount() {
		size_t count = 0;
		for $each(b, bullets)
			if (b.isFree())
				count++;
		return count;
	}

	void create($evt Signal onBulletCreated = $evt DEF_SIGNAL) {
		if (created) return;
		haltProcedure = false;
		for (size_t i = 0; i < BULLET_COUNT; i++) {
			if (!bullets[i].sprite)
				bullets[i].sprite =
					mesh.createReference<AnimatedPlane>();
			bullets[i].setFree(true);
			onBulletCreated();
			if (haltProcedure) return;
		}
		created = true;
	}

	BULLET_TYPE* getLastBullet() {
		return last;
	}

	BULLET_TYPE* createBullet() {
		for $each(b, bullets)
			if (b.isFree()) {
				last = b.enable()->setZero();
				last->settings = BulletData();
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

	BULLET_TYPE* createBullet(BulletData bullet) {
		BULLET_TYPE* b = createBullet();
		b->settings = bullet;
		return b->reset();
	}

	BULLET_TYPE bullets[BULLET_COUNT];

	bool haltProcedure = false;

private:
	bool created = false;

	BULLET_TYPE* last = nullptr;
};

#endif // MAKAI_BASE_BULLET_H
