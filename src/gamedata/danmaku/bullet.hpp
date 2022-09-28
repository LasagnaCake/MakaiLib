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

	Reference::AnimatedPlane* sprite;

	Tasking::MultiTasker taskers;

	void onFrame(float delta) {
		taskers.yield();
		if (sprite) {
			// Set sprite position
			sprite->local.position = local.position;
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
		currentRotation = settings.rotation.start;
		local.rotation = currentRotation;
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
		if (sprite) sprite->visible = !state;
		return this;
	}

	Transform2D local;

	float currentSpeed		= 0;
	float currentRotation	= 0;
private:

	bool free = false;
};

template <size_t BULLET_COUNT>
class BulletManager: Entity {
public:
	DERIVED_CONSTRUCTOR(BulletManager, Entity, {
		EntityClass::$_ROOT += this;
		mesh.setRenderLayer($layer(ENEMY_BULLET));
	})

	DERIVED_CLASS(BulletManager, Entity)

	Renderable mesh;

	void onFrame(float delta) override {
		for $each(b, bullets) {
			b.onFrame(delta);
		}
	}

	void create() {
		if (created) return;
		for (size_t i = 0; i < BULLET_COUNT; i++) {
			bullets[i].sprite = mesh.createReference<AnimatedPlane>();
			bullets[i].setFree();
			onBulletCreated();
		}
		created = true;
	}

	Bullet* createBullet() {
		for $each(b, bullets)
			if (b.isFree())
				return b.enable()->reset();
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

	Event::Signal onBulletCreated;

private:
	bool created = false;
};

#endif // MAKAI_BASE_BULLET_H
