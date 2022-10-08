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

struct Pause {
	bool enabled	= false;
	long time		= -1;
};

struct BulletData {
	// Collision data
	CircleBounds2D	hitbox;
	// Parameters
	BulletParam	speed;
	BulletParam	rotation;
	Pause pause;
	// Flags
	bool shuttle		= false;
	bool rebound		= false;
	bool discardable	= true;
	bool collidable		= true;
	bool rotateSprite	= true;
	// Destroy on Playfield Exit
	bool dope = true;
};

class Bullet {
public:
	Bullet() {
		onFree			= $tsignal(Bullet*) {};
		onBulletFrame	= onFree;
		onRebound		= onFree;
		onShuttle		= onFree;
	}

	BulletData settings;

	$ref AnimatedPlane* sprite = nullptr;

	$tsk MultiTasker taskers;

	$tev Signal<Bullet*> onFree;
	$tev Signal<Bullet*> onBulletFrame;
	$tev Signal<Bullet*> onRebound;
	$tev Signal<Bullet*> onShuttle;

	void onFrame(float delta) {
		if (free) return;
		onBulletFrame(this);
		if (settings.pause.enabled) {
			if (settings.pause.time < 0) return;
			if ((--settings.pause.time) > 0) return;
			settings.pause.enabled = false;
		}
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
		local.rotation = currentRotation;
		updateSprite();
	}

	Bullet* reset() {
		setZero();
		currentSpeed = settings.speed.start;
		local.rotation =
		currentRotation = settings.rotation.start;
		settings.hitbox.position = local.position;
		updateSprite();
		sprite->local.rotation.z = local.rotation;
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
		if (free) onFree(this);
		return this;
	}

	Bullet* discard() {
		if (settings.discardable)
			setFree();
		return this;
	}

	Transform2D local;

	float currentSpeed		= 0;
	float currentRotation	= 0;

private:
	void updateSprite() {
		if (!sprite) return;
		// Set sprite position
		sprite->local.position = Vector3(local.position);
		settings.hitbox.position = local.position;
		// Set sprite rotation
		if (settings.rotateSprite)
			sprite->local.rotation.z = local.rotation;
		// Set sprite scale
		sprite->local.scale = Vector3(local.scale);
	}

	float speedFactor		= 0;
	float rotationFactor	= 0;

	bool free = true;
};

template <
	size_t BULLET_COUNT,
	class BULLET_TYPE = Bullet,
	size_t ACTOR_LAYER = $layer(PLAYER_BULLET),
	size_t ENEMY_LAYER = $layer(ENEMY),
	class = $isderivedof(BULLET_TYPE, Bullet)
>
struct BulletManager: Entity {
	DERIVED_CONSTRUCTOR(BulletManager, Entity, {
		addToGame(this, "DanmakuGame");
		mesh.setRenderLayer(ACTOR_LAYER);
		$ecl groups.addEntity(this, ACTOR_LAYER);
	})

	$cdt BoxBounds2D playfield;
	$cdt BoxBounds2D board;

	DERIVED_CLASS(BulletManager, Entity)

	Renderable mesh;

	void onDelete() override {
		$ecl groups.removeFromAll(this);
	}

	void onFrame(float delta) override {
		for $each(b, bullets) {
			b.onFrame(delta);
			if (!b.isFree() && b.settings.collidable)
			for $each(actor, $ecl groups.getGroup(ENEMY_LAYER)) {
				auto p = ((AreaCircle2D*)actor);
				if (
					p->collision.enabled
					&& $cdt withinBounds(
						b.settings.hitbox,
						p->getCircleBounds()
					)
				) {
					p->onCollision(this);
					b.discard();
				}
			}
			if (b.settings.rebound || b.settings.shuttle)
				if (
					! $cdt withinBounds(
						b.local.position,
						board
					)
				) {
					// Rebounding (reflecting) takes precedence over shuttling (wrapping)
					if (b.settings.rebound) {
						#define $wreflect(AA) AA = Math::pi - AA
						// Check X
						if (
							b.local.position.x < board.x.min ||
							b.local.position.x > board.x.max
						) {
							$wreflect(b.currentRotation);
							$wreflect(b.settings.rotation.start);
							$wreflect(b.settings.rotation.end);
						}
						#undef $wreflect
						#define $wreflect(AA) AA = - AA
						// Check Y
						if (
							b.local.position.y < board.y.min ||
							b.local.position.y > board.y.max
						) {
							$wreflect(b.currentRotation);
							$wreflect(b.settings.rotation.start);
							$wreflect(b.settings.rotation.end);
						}
						b.onRebound(&b);
						// Disable rebounding
						b.settings.rebound = false;
						#undef $wreflect
					}
					else if (b.settings.shuttle) {
						// Check X
						if (b.local.position.x < board.x.min)
							b.local.position.x = board.x.max;
						if (b.local.position.x > board.x.max)
							b.local.position.x = board.x.min;
						// Check Y
						if (b.local.position.y < board.y.min)
							b.local.position.y = board.y.max;
						if (b.local.position.y > board.y.max)
							b.local.position.y = board.y.min;
						b.onShuttle(&b);
						// Disable shuttle
						b.settings.shuttle = false;
					}
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

	void freeAll() {
		for $each(b, bullets) b.setFree();
	}

	void discardAll() {
		for $each(b, bullets) b.discard();
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

	#define BULLET_LIST std::vector<BULLET_TYPE*>
	BULLET_LIST getInArea($cdt CircleBounds2D target) {
		BULLET_LIST res;
		for $eachif(b, bullets, !b.isFree() && b.settings.collidable) {
			if (
				$cdt withinBounds(
					b.settings.hitbox,
					target
				)
			) res.push_back(&b);
		}
		return res;
	}

	BULLET_LIST getInArea($cdt BoxBounds2D target) {
		BULLET_LIST res;
		for $eachif(b, bullets, !b.isFree() && b.settings.collidable) {
			if (
				$cdt withinBounds(
					b.settings.hitbox,
					target
				)
			) res.push_back(&b);
		}
		return res;
	}

	BULLET_LIST getActive() {
		BULLET_LIST res;
		for $eachif(b, bullets, !b.isFree()) res.push_back(&b);
		return res;
	}
	#undef BULLET_LIST

	BULLET_TYPE* getLastBullet() {
		return last;
	}

	BULLET_TYPE* createBullet() {
		for $eachif(b, bullets, b.isFree()) {
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
