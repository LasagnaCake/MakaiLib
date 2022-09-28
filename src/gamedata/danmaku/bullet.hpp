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

class Bullet: public Entity2D {
public:
	DERIVED_CONSTRUCTOR(Bullet, Entity2D, {
		EntityClass::groups.addEntity(this, $layer(ENEMY_BULLET));
	})
	DERIVED_CLASS(Bullet, Entity2D)

	BulletData settings;

	Reference::AnimatedPlane* sprite;

	void onFrame(float delta) override {
		if (sprite) {
			// Set sprite position
			sprite->local.position = Vector3(
				globalPosition(),
				zIndex
			);
			// Set sprite rotation
			sprite->local.rotation.z = globalRotation();
			if (settings.rotateSprite)
				sprite->local.rotation.z += currentRotation;
			// Set sprite scale
			sprite->local.scale = Vector3(globalScale());
		}
	}

	Bullet* reset() {
		currentSpeed = settings.speed.start;
		currentRotation = settings.rotation.start;
		rotation = currentRotation;
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
		process = !state;
		return this;
	}

	float currentSpeed		= 0;
	float currentRotation	= 0;
private:

	bool free = false;
};

template <size_t BULLET_COUNT>
class BulletManager: Entity2D {
public:
	DERIVED_CONSTRUCTOR(BulletManager, Entity2D, {
		EntityClass::$_ROOT += this;
		mesh.setRenderLayer($layer(ENEMY_BULLET));
	})

	virtual ~BulletManager() {
		for (auto bullet: bullets)
			delete bullet;
	}

	Renderable mesh;

	void create() {
		if (created) return;
		for (size_t i = 0; i < BULLET_COUNT; i++) {
			auto b = new Bullet("Danmaku");
			bullets[i] = b;
			addChild(b);
			b->sprite = mesh.createReference<AnimatedPlane>();
			b->setFree();
			onBulletCreated();
		}
		created = true;
	}

	DERIVED_CLASS(BulletManager, Entity2D)

	Bullet* createBullet() {
		for (auto b: bullets)
			if (b->isFree())
				return b->enable()->reset();
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

	std::array<Bullet*, BULLET_COUNT> bullets;

	Event::Signal onBulletCreated;

private:
	bool created = false;
};

#endif // MAKAI_BASE_BULLET_H
