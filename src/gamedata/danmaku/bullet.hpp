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
		DANMAKU_FRAME_BEGIN;
		UPDATE_PARAM(vel)
		UPDATE_PARAM(rot)
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
		if (sprite)
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
		sprite->local.position = Vector3(local.position, zIndex + _zOffset);
		params.hitbox.position = local.position;
		// Set sprite rotation
		if (params.rotateSprite)
			sprite->local.rotation.z = local.rotation;
		// Set sprite scale
		sprite->local.scale = Vector3(local.scale, zScale);
	}

	Bullet* clearSignals() override {
		DanmakuObject::clearSignals();
		auto pass = $tsignal(DanmakuObject*) {};
		onRebound = onShuttle = pass;
		return this;
	}
};

#define $_b(VAR) ((Bullet*)(VAR))
#define $b(VAR) (($dmk Bullet*)(VAR))

typedef std::vector<Bullet*> BulletList;

template <
	size_t BULLET_COUNT,
	size_t ACTOR_LAYER = $layer(ENEMY_BULLET),
	size_t ENEMY_LAYER = $layer(PLAYER),
	size_t COLLI_LAYER = ACTOR_LAYER
>
class BulletManager: Entity {
public:
	DERIVED_CLASS(BulletManager, Entity)

	DERIVED_CONSTRUCTOR(BulletManager, Entity, {
		addToGame(this, "DanmakuGame");
		mesh.setRenderLayer(ACTOR_LAYER);
		$ecl groups.addEntity(this, ACTOR_LAYER);
		$ecl collisionLayers.addEntity(this, COLLI_LAYER);
	})

	$cdt BoxBounds2D playfield;
	$cdt BoxBounds2D board;

	Renderable mesh;

	void onDelete() override {
		//delete &mesh;
		$debug("\nDeleting bullet manager...");
		$ecl groups.removeFromAll(this);
		delete[] bullets;
	}

	void onFrame(float delta) override {
		/*auto half = [&]() {
			for (auto i = 0; i < BULLET_COUNT / 2; i++) {
				auto* b = &bullets[i];
				b->onFrame(delta);
				if (!b->isFree() && b->params.collidable)
				for $each(actor, $ecl groups.getGroup(ENEMY_LAYER)) {
					auto a = ((AreaCircle2D*)actor);
					auto targetBounds = a->getCircleBounds();
					if (
						a->collision.enabled
						&& $cdt withinBounds(
							b->params.hitbox,
							targetBounds
						)
					) {
						a->onCollision(this);
						b->discard();
					}
				}
				if (b->params.rebound || b->params.shuttle)
					if (
						! $cdt withinBounds(
							b->local.position,
							board
						)
					) {
						// Rebounding (reflecting) takes precedence over shuttling (wrapping)
						if (b->params.rebound) {
							#define $wreflect(AA) AA = Math::pi - AA
							// Check X
							if (
								b->local.position.x < board.x.min ||
								b->local.position.x > board.x.max
							) {
								$wreflect(b->params.rot.current);
								$wreflect(b->params.rot.start);
								$wreflect(b->params.rot.end);
							}
							#undef $wreflect
							#define $wreflect(AA) AA = - AA
							// Check Y
							if (
								b->local.position.y < board.y.min ||
								b->local.position.y > board.y.max
							) {
								$wreflect(b->params.rot.current);
								$wreflect(b->params.rot.start);
								$wreflect(b->params.rot.end);
							}
							b->onRebound(b);
							// Disable rebounding
							b->params.rebound = false;
							#undef $wreflect
						}
						else if (b->params.shuttle) {
							// Check X
							if (b->local.position.x < board.x.min)
								b->local.position.x = board.x.max;
							if (b->local.position.x > board.x.max)
								b->local.position.x = board.x.min;
							// Check Y
							if (b->local.position.y < board.y.min)
								b->local.position.y = board.y.max;
							if (b->local.position.y > board.y.max)
								b->local.position.y = board.y.min;
							b->onShuttle(b);
							// Disable shuttle
							b->params.shuttle = false;
						}
					}
				if (b->params.dope)
					if (
						! $cdt withinBounds(
							b->params.hitbox,
							playfield
						)
					) {
						b->setFree(true);
					}
			}
		};
		std::thread firstHalf(half);*/
		for (auto i = 0; i < BULLET_COUNT; i++) {
			auto* b = &bullets[i];
			b->onFrame(delta);
			if (!b->isFree() && b->params.collidable)
			if (mainPlayer) {
				auto& a = (mainPlayer);
				auto targetBounds = a->getCircleBounds();
				if (
					a->collision.enabled
					&& $cdt withinBounds(
						b->params.hitbox,
						targetBounds
					)
				) {
					a->onCollision(this);
					b->discard();
				}
			}
			if (b->params.rebound || b->params.shuttle)
				if (
					! $cdt withinBounds(
						b->local.position,
						board
					)
				) {
					// Rebounding (reflecting) takes precedence over shuttling (wrapping)
					if (b->params.rebound) {
						#define $wreflect(AA) AA = Math::pi - AA
						// Check X
						if (
							b->local.position.x < board.x.min ||
							b->local.position.x > board.x.max
						) {
							$wreflect(b->params.rot.current);
							$wreflect(b->params.rot.start);
							$wreflect(b->params.rot.end);
						}
						#undef $wreflect
						#define $wreflect(AA) AA = - AA
						// Check Y
						if (
							b->local.position.y < board.y.min ||
							b->local.position.y > board.y.max
						) {
							$wreflect(b->params.rot.current);
							$wreflect(b->params.rot.start);
							$wreflect(b->params.rot.end);
						}
						b->onRebound(b);
						// Disable rebounding
						b->params.rebound = false;
						#undef $wreflect
					}
					else if (b->params.shuttle) {
						// Check X
						if (b->local.position.x < board.x.min)
							b->local.position.x = board.x.max;
						if (b->local.position.x > board.x.max)
							b->local.position.x = board.x.min;
						// Check Y
						if (b->local.position.y < board.y.min)
							b->local.position.y = board.y.max;
						if (b->local.position.y > board.y.max)
							b->local.position.y = board.y.min;
						b->onShuttle(b);
						// Disable shuttle
						b->params.shuttle = false;
					}
				}
			if (b->params.dope)
				if (
					! $cdt withinBounds(
						b->params.hitbox,
						playfield
					)
				) {
					b->setFree(true);
				}
		}
		//firstHalf.join();
	}

	void freeAll() {
		for $seach(b, bullets, BULLET_COUNT) b.setFree(); $endseach
	}

	void discardAll() {
		for $seach(b, bullets, BULLET_COUNT) b.discard(); $endseach
	}

	size_t getFreeCount() {
		size_t count = 0;
		for $seach(b, bullets, BULLET_COUNT)
			if (b.isFree())
				count++;
		$endseach
		return count;
	}

	void create() {
		if (created) return;
		haltProcedure = false;
		for (size_t i = 0; i < BULLET_COUNT; i++) {
			if (!bullets[i].sprite)
				bullets[i].sprite =
					mesh.createReference<AnimatedPlane>();
			bullets[i].setFree(true);
			bullets[i]._setZOffset(Math::epsilonF * ((float)i));
			bullets[i].sprite->size = Vector2(16, 16);
			if (haltProcedure) return;
		}
		created = true;
	}

	BulletList getInArea($cdt CircleBounds2D target) {
		BulletList res;
		for $seachif(b, bullets, BULLET_COUNT, !b.isFree() && b.params.collidable) {
			if (
				$cdt withinBounds(
					b.params.hitbox,
					target
				)
			) {
				res.push_back(&b);
			}
		} $endseach
		return res;
	}

	BulletList getInArea($cdt BoxBounds2D target) {
		BulletList res;
		for $seachif(b, bullets, BULLET_COUNT, !b.isFree() && b.params.collidable) {
			if (
				$cdt withinBounds(
					b.params.hitbox,
					target
				)
			) res.push_back(&b);
		} $endseach
		return res;
	}

	BulletList getActive() {
		BulletList res;
		for $seachif(b, bullets, BULLET_COUNT, !b.isFree()) res.push_back(&b); $endseach
		return res;
	}

	Bullet* getLastBullet() {
		return (Bullet*)last;
	}

	Bullet* createBullet() {
		for $seachif(b, bullets, BULLET_COUNT, b.isFree()) {
			last = b.enable()->setZero();
			last->pause = Pause();
			last->params = BulletData();
			last->grazed = false;
			last->taskers.clearTaskers();
			last->flags.clear();
			last->clearSignals();
			last->updateSprite();
			#ifdef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
			pbobw = 0;
			#endif
			return last;
		} $endseach
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
		last->flags.clear();
		last->clearSignals();
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

	Bullet* bullets = new Bullet[BULLET_COUNT];

	bool haltProcedure = false;

private:
	bool created = false;
	#ifdef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
	size_t pbobw = 0;
	#endif
	Bullet* last = nullptr;
};

typedef BulletManager<PLAYER_BULLET_COUNT, $layer(PLAYER_BULLET), $layer(ENEMY), $layer(PLAYER)>	PlayerBulletManager;
typedef BulletManager<ENEMY_BULLET_COUNT, $layer(ENEMY_BULLET), $layer(PLAYER), $layer(ENEMY)>		EnemyBulletManager;

PlayerBulletManager*	playerBulletManager = nullptr;
EnemyBulletManager*		enemyBulletManager = nullptr;

#define DANMAKU_PBM $dmk playerBulletMananger
#define DANMAKU_EBM $dmk enemyBulletManager

#define $bullet(TYPE)	$getman( TYPE##Bullet )
#define $setb(TYPE)		$setman( TYPE##Bullet )

#endif // MAKAI_BASE_BULLET_H
