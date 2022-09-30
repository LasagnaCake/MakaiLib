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

	Event::Signal onFree = Event::DEF_SIGNAL;

	void onFrame(float delta) {
		if (free) return;
		taskers.yield();
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
		if (sprite) sprite->visible = !free;
		if (free) onFree();
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
		EntityClass::groups.addEntity(this, BULLET_MANAGER_GROUP);
	})

	DERIVED_CLASS(BulletManager, Entity)

	Renderable mesh;

	void onFrame(float delta) override {
		for $each(b, bullets) {
			b.onFrame(delta);
			if (!b.isFree() && b.settings.collidable)
			for $each(player, EntityClass::groups.getGroup($layer(PLAYER))) {
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
		for (size_t i = 0; i < BULLET_COUNT; i++) {
			bullets[i].sprite = mesh.createReference<AnimatedPlane>();
			bullets[i].setFree();
			onBulletCreated();
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

	Event::Signal onBulletCreated = Event::DEF_SIGNAL;

private:
	bool created = false;

	Bullet* last = nullptr;
};

#endif // MAKAI_BASE_BULLET_H
