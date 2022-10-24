#ifndef MAKAI_BASE_BULLET_H
#define MAKAI_BASE_BULLET_H

typedef ObjectParam	BulletParam;

struct BulletData: ObjectData {
	bool shuttle		= false;
	bool rebound		= false;
	bool rotateSprite	= true;
};

class Bullet: public DanmakuObject {
public:
	Bullet(): DanmakuObject() {
		auto pass = $tsignal(Bullet*) {};
		onRebound		= pass;
		onShuttle		= pass;
	}

	$tev Signal<Bullet*> onRebound;
	$tev Signal<Bullet*> onShuttle;

	BulletData params;

	bool grazed = false;

	void onFrame(float delta) override {
		if (free) return;
		DanmakuObject::onFrame(delta);
		if (pause.enabled) return;
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

	Bullet* reset() override {
		setZero();
		params.vel.current = params.vel.start;
		local.rotation =
		params.rot.current = params.rot.start;
		params.hitbox.position = local.position;
		updateSprite();
		sprite->local.rotation.z = local.rotation;
		return this;
	}

	Bullet* setZero() override {
		params.vel.current =
		local.rotation =
		params.rot.current =
		params.vel.factor =
		params.rot.factor = 0;
		return this;
	}

	Bullet* enable() override {
		return (Bullet*)setFree(false);
	}

	Bullet* setFree(bool state = true) override {
		DanmakuObject::setFree(state);
		if (sprite) sprite->visible = !free;
		return this;
	}

	Bullet* discard() override {
		if (params.discardable)
			setFree();
		return this;
	}

	void updateSprite() override {
		if (!sprite) return;
		// Set sprite position
		sprite->local.position = Vector3(local.position, zIndex);
		params.hitbox.position = local.position;
		// Set sprite rotation
		if (params.rotateSprite)
			sprite->local.rotation.z = local.rotation;
		// Set sprite scale
		sprite->local.scale = Vector3(local.scale, zScale);
	}
};

#define $_b(VAR) ((Bullet*)(VAR))
#define $b(VAR) (($dmk Bullet*)(VAR))

typedef std::vector<Bullet*> BulletList;

template <
	size_t BULLET_COUNT,
	size_t ACTOR_LAYER = $layer(ENEMY_BULLET),
	size_t ENEMY_LAYER = $layer(PLAYER)
>
struct BulletManager: Entity {
	DERIVED_CLASS(BulletManager, Entity)

	DERIVED_CONSTRUCTOR(BulletManager, Entity, {
		addToGame(this, "DanmakuGame");
		mesh.setRenderLayer(ACTOR_LAYER);
		$ecl groups.addEntity(this, ACTOR_LAYER);
	})

	$cdt BoxBounds2D playfield;
	$cdt BoxBounds2D board;

	Renderable mesh;

	void onDelete() override {
		$ecl groups.removeFromAll(this);
	}

	void onFrame(float delta) override {
		for $each(b, bullets) {
			b.onFrame(delta);
			if (!b.isFree() && b.params.collidable)
			for $each(actor, $ecl groups.getGroup(ENEMY_LAYER)) {
				auto a = ((AreaCircle2D*)actor);
				if (
					a->collision.enabled
					&& $cdt withinBounds(
						b.params.hitbox,
						a->getCircleBounds()
					)
				) {
					a->onCollision(this);
					b.discard();
				}
			}
			if (b.params.rebound || b.params.shuttle)
				if (
					! $cdt withinBounds(
						b.local.position,
						board
					)
				) {
					// Rebounding (reflecting) takes precedence over shuttling (wrapping)
					if (b.params.rebound) {
						#define $wreflect(AA) AA = Math::pi - AA
						// Check X
						if (
							b.local.position.x < board.x.min ||
							b.local.position.x > board.x.max
						) {
							$wreflect(b.params.rot.current);
							$wreflect(b.params.rot.start);
							$wreflect(b.params.rot.end);
						}
						#undef $wreflect
						#define $wreflect(AA) AA = - AA
						// Check Y
						if (
							b.local.position.y < board.y.min ||
							b.local.position.y > board.y.max
						) {
							$wreflect(b.params.rot.current);
							$wreflect(b.params.rot.start);
							$wreflect(b.params.rot.end);
						}
						b.onRebound(&b);
						// Disable rebounding
						b.params.rebound = false;
						#undef $wreflect
					}
					else if (b.params.shuttle) {
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
						b.params.shuttle = false;
					}
				}
			if (b.params.dope)
				if (
					! $cdt withinBounds(
						b.params.hitbox,
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

	BulletList getInArea($cdt CircleBounds2D target) {
		BulletList res;
		for $eachif(b, bullets, !b.isFree() && b.params.collidable) {
			if (
				$cdt withinBounds(
					b.params.hitbox,
					target
				)
			) {
				res.push_back(&b);
			}
		}
		return res;
	}

	BulletList getInArea($cdt BoxBounds2D target) {
		BulletList res;
		for $eachif(b, bullets, !b.isFree() && b.params.collidable) {
			if (
				$cdt withinBounds(
					b.params.hitbox,
					target
				)
			) res.push_back(&b);
		}
		return res;
	}

	BulletList getActive() {
		BulletList res;
		for $eachif(b, bullets, !b.isFree()) res.push_back(&b);
		return res;
	}

	Bullet* getLastBullet() {
		return last;
	}

	Bullet* createBullet() {
		for $eachif(b, bullets, b.isFree()) {
			last = b.enable()->setZero();
			last->pause = Pause();
			last->params = BulletData();
			last->grazed = false;
			last->taskers.clearTaskers();
			last->updateSprite();
			#ifdef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
			pbobw = 0;
			#endif
			return last;
		}
		#ifndef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
		throw std::runtime_error(
			getName()
			+ ": Out of usable bullets ("
			+ std::to_string(BULLET_COUNT)
			+ ")!"
		);
		#else
		last = bullets[pbobw++].enable()->setZero();
		last->params = BulletData();
		last->grazed = false;
		last->taskers.clearTaskers();
		if (pbobw > BULLET_COUNT) pbobw = 0;
		last->updateSprite();
		return last;
		#endif
	}

	Bullet* createBullet(BulletData bullet) {
		Bullet* b = createBullet();
		b->params = bullet;
		return b->reset();
	}

	Bullet bullets[BULLET_COUNT];

	bool haltProcedure = false;

private:
	bool created = false;
	#ifdef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
	size_t pbobw = 0;
	#endif
	Bullet* last = nullptr;
};

typedef BulletManager<PLAYER_BULLET_COUNT, $layer(PLAYER_BULLET), $layer(ENEMY)>	PlayerBulletManager;
typedef BulletManager<ENEMY_BULLET_COUNT, $layer(ENEMY_BULLET), $layer(PLAYER)>		EnemyBulletManager;

PlayerBulletManager*	playerBulletManager = nullptr;
EnemyBulletManager*		enemyBulletManager = nullptr;

#define $bullet(TYPE)	$getman( TYPE##Bullet )
#define $setb(TYPE)		$setman( TYPE##Bullet )

#endif // MAKAI_BASE_BULLET_H
