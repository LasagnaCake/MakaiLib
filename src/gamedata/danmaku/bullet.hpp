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
		auto pass = $objsignal {};
		onRebound		= pass;
		onShuttle		= pass;
	}

	ObjectSignal onRebound;
	ObjectSignal onShuttle;

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
		if (state) grazed = false;
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
	size_t ENEMY_LAYER = $layer(PLAYER)
>
class BulletManager: Entity {
public:
	DERIVED_CLASS(BulletManager, Entity)

	DERIVED_CONSTRUCTOR(BulletManager, Entity, {
		addToGame(this, "DanmakuGame");
		mesh.setRenderLayer(ACTOR_LAYER);
		addToGroup(ACTOR_LAYER);
		mesh.material.shaded = false;
	})

	$cdt BoxBounds2D playfield;
	$cdt BoxBounds2D board;

	Renderable mesh;

	virtual ~BulletManager() {
		//delete &mesh;
		$debug("\nDeleting bullet manager...");
		delete[] bullets;
	}

	void onFrame(float delta) override {
		GAME_PARALLEL_FOR
		for (auto i = 0; i < BULLET_COUNT; i++) {
			auto& b = bullets[i];
			b.onFrame(delta);
			if (!b.isFree() && b.params.collidable) {
				for $each(actor, $ecl groups.getGroup(ENEMY_LAYER)) {
					auto a = (AreaCircle2D*)actor;
					auto targetBounds = a->collision.shape;
					if (
						a->collision.enabled
						&& $cdt withinBounds(
							b.params.hitbox,
							targetBounds
						)
					) {
						a->onCollision(this);
						b.discard();
					}
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
							b.local.position.x < board.min.x||
							b.local.position.x > board.max.x
						) {
							$wreflect(b.params.rot.current);
							$wreflect(b.params.rot.start);
							$wreflect(b.params.rot.end);
						}
						#undef $wreflect
						#define $wreflect(AA) AA = - AA
						// Check Y
						if (
							b.local.position.y < board.min.y ||
							b.local.position.y > board.max.y
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
						if (b.local.position.x < board.min.x)
							b.local.position.x = board.max.x;
						if (b.local.position.x > board.max.x)
							b.local.position.x = board.min.x;
						// Check Y
						if (b.local.position.y < board.min.y)
							b.local.position.y = board.max.y;
						if (b.local.position.y > board.max.y)
							b.local.position.y = board.min.y;
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
		GAME_PARALLEL_FOR
		for $seach(b, bullets, BULLET_COUNT) b.setFree(); $endseach
	}

	void discardAll() {
		GAME_PARALLEL_FOR
		for $seach(b, bullets, BULLET_COUNT) b.discard(); $endseach
	}

	size_t getFreeCount() {
		size_t count = 0;
		GAME_PARALLEL_FOR
		for $seach(b, bullets, BULLET_COUNT)
			if (b.isFree())
				count++;
		$endseach
		return count;
	}

	void create() {
		if (created) return;
		haltProcedure = false;
		for (size_t i = BULLET_COUNT; i > 0; i--) {
			auto& b = bullets[i-1];
			if (!b.sprite) b.sprite = mesh.createReference<AnimatedPlane>();
			b.setFree(true);
			b._setZOffset(
				(1.0 - ((((float)BULLET_COUNT)-((float)i)) / ((float)BULLET_COUNT)))
			);
			b.sprite->size = Vector2(16, 16);
			if (haltProcedure) return;
		}
		created = true;
	}

	template <CollisionType T = CircleBounds2D>
	BulletList getInArea(T target) {
		BulletList res;
		GAME_PARALLEL_FOR
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

	BulletList getActive() {
		BulletList res;
		GAME_PARALLEL_FOR
		for $seachif(b, bullets, BULLET_COUNT, !b.isFree()) res.push_back(&b); $endseach
		return res;
	}

	void forEach(Callback<Bullet> func) {
		GAME_PARALLEL_FOR
		for $ssrange(i, 0, BULLET_COUNT)
			func(bullets[i]);
	}

	void forEachActive(Callback<Bullet> func) {
		GAME_PARALLEL_FOR
		for $ssrange(i, 0, BULLET_COUNT)
			if (!bullets[i].isFree())
				func(bullets[i]);
	}

	template <CollisionType T>
	void forEachInArea(T area, Callback<Bullet> func) {
		GAME_PARALLEL_FOR
		for $ssrange(i, 0, BULLET_COUNT)
			if (
				!bullets[i].isFree()
			&&	bullets[i].params.collidable
			&&	$cdt withinBounds(bullets[i].params.hitbox, area)
			) func(bullets[i]);
	}

	Bullet* getLastBullet() {
		return (Bullet*)last;
	}

	Bullet* createBullet() {
		//GAME_PARALLEL_FOR
		for $seachif(b, bullets, BULLET_COUNT, b.isFree()) {
			last = b.enable()->setZero();
			last->pause = Pause();
			last->params = BulletData();
			last->grazed = false;
			last->taskers.clearTaskers();
			last->flags.clear();
			last->clearSignals();
			last->updateSprite();
			last->sprite->setColor();
			#ifdef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
			pbobw = 0;
			#endif
			return last;
		} $endseach
		#ifndef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
		throw OutOfObjects(
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
		last->updateSprite();
		last->sprite->setColor();
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

typedef BulletManager<PLAYER_BULLET_COUNT, $layer(PLAYER_BULLET), $layer(ENEMY)>	PlayerBulletManager;
typedef BulletManager<ENEMY_BULLET_COUNT, $layer(ENEMY_BULLET), $layer(PLAYER)>		EnemyBulletManager;

PlayerBulletManager*	playerBulletManager	= nullptr;
EnemyBulletManager*		enemyBulletManager	= nullptr;

#define DANMAKU_PBM $dmk playerBulletManager
#define DANMAKU_EBM $dmk enemyBulletManager

#define $bullet(TYPE)	$getman( TYPE##Bullet )
#define $setb(TYPE)		$setman( TYPE##Bullet )

#endif // MAKAI_BASE_BULLET_H
