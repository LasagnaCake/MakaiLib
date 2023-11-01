#ifndef MAKAI_BASE_BULLET_H
#define MAKAI_BASE_BULLET_H

typedef ObjectParam	BulletParam;

struct BulletData: ObjectData {
	bool shuttle		= false;
	bool rebound		= false;
	bool rotateSprite	= true;
};

template <size_t BULLET_COUNT, size_t ACTOR_LAYER, size_t ENEMY_LAYER> class BulletManager;

class Bullet: public DanmakuObject {
public:
	Bullet(): DanmakuObject() {
		auto pass = DANMAKU_OBJ_SIGNAL {};
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
		UPDATE_PARAM(stretch)
		if (params.vel.current)
			local.position += VecMath::angleV2(params.rot.current) * params.vel.current * delta;
		local.rotation = params.rot.current;
		updateSprite();
	}

	Bullet* reset() override {
		DanmakuObject::reset();
		setZero();
		params.vel.current = params.vel.start;
		local.rotation =
		params.rot.current = params.rot.start;
		params.stretch.current = params.stretch.start;
		params.hitbox.position = local.position;
		updateSprite();
		if (sprite)
			sprite->local.rotation.z = local.rotation;
		return this;
	}

	Bullet* setZero() override {
		DanmakuObject::setZero();
		params.vel.current =
		local.rotation =
		params.rot.current =
		params.vel.factor =
		params.rot.factor =
		params.stretch.factor = 0;
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
			despawn();
		return this;
	}

	void updateSprite() override {
		if (!sprite) return;
		// Set sprite transforms
		sprite->local.position	= Vector3(local.position, zIndex + zOffset);
		if (params.rotateSprite)
			sprite->local.rotation.z = local.rotation;
		sprite->local.scale = Vector3(local.scale, zScale) * (1+(1-factor)*2);
		// Set hitbox transforms
		params.hitbox.position	= local.position;
		if (params.rotateHitbox)
			params.hitbox.rotation	= local.rotation;
	}

	Bullet* clearSignals() override {
		DanmakuObject::clearSignals();
		auto pass = T_SIGNAL (DanmakuObject*) {};
		onRebound = onShuttle = pass;
		return this;
	}

protected:
	void onObjectSpawnBegin() override {params.collidable = false;}
	void onObjectSpawnEnd() override {params.collidable = true;}
	void onObjectDespawnBegin() override {params.collidable = false;}
	//void onObjectDespawnEnd() override {setFree(true);}

private:
	template<size_t B, size_t A, size_t E> friend class BulletManager;
};

typedef std::vector<Bullet*> BulletList;

template <
	size_t BULLET_COUNT,
	size_t ACTOR_LAYER,
	size_t ENEMY_LAYER
>
class BulletManager: DanmakuObjectManager {
public:
	constexpr static size_t bulletCount = BULLET_COUNT;

	DERIVED_CLASS(BulletManager, DanmakuObjectManager)

	DERIVED_CONSTRUCTOR(BulletManager, DanmakuObjectManager, {
		mesh.setRenderLayer(ACTOR_LAYER);
		addToGroup(ACTOR_LAYER);
		mesh.material.shaded = false;
	})

	CollisionData::BoxBounds2D playfield;
	CollisionData::BoxBounds2D board;

	Renderable mesh;

	virtual ~BulletManager() {
		//delete &mesh;
		DEBUGLN("\nDeleting bullet manager...");
		delete[] bullets;
	}

	void onFrame(float delta) override {
		GAME_PARALLEL_FOR
		for (auto i = 0; i < BULLET_COUNT; i++) {
			auto& b = bullets[i];
			b.onFrame(delta);
			if (!b.isFree() && b.params.collidable) {
				for EACH(actor, EntityClass::groups.getGroup(ENEMY_LAYER)) {
					auto a = (AreaCircle2D*)actor;
					auto targetBounds = a->collision.shape;
					if (
						a->collision.enabled
						&& CollisionData::withinBounds(
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
					! CollisionData::withinBounds(
						b.local.position,
						board
					)
				) {
					// Rebounding (reflecting) takes precedence over shuttling (wrapping)
					if (b.params.rebound) {
						#define W_REFLECT(AA) AA = Math::pi - AA
						// Check X
						if (
							b.local.position.x < board.min.x||
							b.local.position.x > board.max.x
						) {
							W_REFLECT(b.params.rot.current);
							W_REFLECT(b.params.rot.start);
							W_REFLECT(b.params.rot.end);
						}
						#undef W_REFLECT
						#define W_REFLECT(AA) AA = - AA
						// Check Y
						if (
							b.local.position.y < board.min.y ||
							b.local.position.y > board.max.y
						) {
							W_REFLECT(b.params.rot.current);
							W_REFLECT(b.params.rot.start);
							W_REFLECT(b.params.rot.end);
						}
						b.onRebound(&b);
						// Disable rebounding
						b.params.rebound = false;
						#undef W_REFLECT
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
					! CollisionData::withinBounds(
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
		for SEACH(b, bullets, BULLET_COUNT) b.setFree(); END_SEACH
	}

	void discardAll() {
		GAME_PARALLEL_FOR
		for SEACH(b, bullets, BULLET_COUNT) b.discard(); END_SEACH
	}

	size_t getFreeCount() {
		size_t count = 0;
		GAME_PARALLEL_FOR
		for SEACH(b, bullets, BULLET_COUNT)
			if (b.isFree())
				count++;
		END_SEACH
		return count;
	}

	void create() {
		if (created) return;
		haltProcedure = false;
		for (size_t i = BULLET_COUNT; i > 0; i--) {
			auto& b = bullets[i-1];
			if (!b.sprite) b.sprite = mesh.createReference<AnimatedPlane>();
			b.setFree(true);
			b.zOffset = (1.0 - ((((float)BULLET_COUNT)-((float)i)) / ((float)BULLET_COUNT)));
			b.sprite->size = Vector2(16, 16);
			if (haltProcedure) return;
		}
		created = true;
	}

	template <CollisionType T = CircleBounds2D>
	BulletList getInArea(T target) {
		BulletList res;
		GAME_PARALLEL_FOR
		for SEACH_IF(b, bullets, BULLET_COUNT, !b.isFree() && b.params.collidable) {
			if (
				CollisionData::withinBounds(
					b.params.hitbox,
					target
				)
			) {
				res.push_back(&b);
			}
		} END_SEACH
		return res;
	}

	BulletList getActive() {
		BulletList res;
		GAME_PARALLEL_FOR
		for SEACH_IF(b, bullets, BULLET_COUNT, !b.isFree()) res.push_back(&b); END_SEACH
		return res;
	}

	void forEach(Callback<Bullet> func) {
		GAME_PARALLEL_FOR
		for SSRANGE(i, 0, BULLET_COUNT)
			func(bullets[i]);
	}

	void forEachActive(Callback<Bullet> func) {
		GAME_PARALLEL_FOR
		for SSRANGE(i, 0, BULLET_COUNT)
			if (!bullets[i].isFree())
				func(bullets[i]);
	}

	template <CollisionType T>
	void forEachInArea(T area, Callback<Bullet> func) {
		GAME_PARALLEL_FOR
		for SSRANGE(i, 0, BULLET_COUNT)
			if (
				!bullets[i].isFree()
			&&	bullets[i].params.collidable
			&&	CollisionData::withinBounds(bullets[i].params.hitbox, area)
			) func(bullets[i]);
	}

	Bullet* getLastBullet() {
		return (Bullet*)last;
	}

	Bullet* createBullet() {
		Bullet* current = nullptr;
		//GAME_PARALLEL_FOR
		for SEACH_IF(b, bullets, BULLET_COUNT, b.isFree()) {
			current = b.enable()->setZero();
			#ifdef MAKAILIB_DANMAKU_PREVENT_INSTANCE_OVERFLOW_BY_WRAP
			pbobw = 0;
			#endif
			break;
		} END_SEACH
		if (!current)
		#ifndef MAKAILIB_DANMAKU_PREVENT_INSTANCE_OVERFLOW_BY_WRAP
			throw OutOfObjects(
				getName()
				+ ": Out of usable bullets ("
				+ std::to_string(BULLET_COUNT)
				+ ")!"
			);
		#else
			last = bullets[pbobw++].enable()->setZero();
		#endif
		else last = current;
		last->pause = Pause();
		last->params = BulletData();
		last->grazed = false;
		last->taskers.clearTaskers();
		last->flags.clear();
		last->clearSignals();
		last->updateSprite();
		last->sprite->setColor();
		return last;
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
	#ifdef MAKAILIB_DANMAKU_PREVENT_INSTANCE_OVERFLOW_BY_WRAP
	size_t pbobw = 0;
	#endif
	Bullet* last = nullptr;
};

#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
typedef BulletManager<PLAYER_BULLET_COUNT, PLAYER_BULLET_LAYER, ENEMY_LAYER>	PlayerBulletManager;
typedef BulletManager<ENEMY_BULLET_COUNT, ENEMY_BULLET_LAYER, PLAYER_LAYER>		EnemyBulletManager;

PlayerBulletManager*	playerBulletManager	= nullptr;
EnemyBulletManager*		enemyBulletManager	= nullptr;

#define DANMAKU_PBM GameData::Danmaku::playerBulletManager
#define DANMAKU_EBM GameData::Danmaku::enemyBulletManager

#else
typedef BulletManager<PLAYER_BULLET_COUNT/2, PLAYER1_BULLET_LAYER, ENEMY1_LAYER>	Player1BulletManager;
typedef BulletManager<PLAYER_BULLET_COUNT/2, PLAYER2_BULLET_LAYER, ENEMY2_LAYER>	Player2BulletManager;
typedef BulletManager<ENEMY_BULLET_COUNT/2, ENEMY1_BULLET_LAYER, PLAYER1_LAYER>		Enemy1BulletManager;
typedef BulletManager<ENEMY_BULLET_COUNT/2, ENEMY2_BULLET_LAYER, PLAYER2_LAYER>		Enemy2BulletManager;

Player1BulletManager*	player1BulletManager	= nullptr;
Player2BulletManager*	player2BulletManager	= nullptr;
Enemy1BulletManager*	enemy1BulletManager		= nullptr;
Enemy2BulletManager*	enemy2BulletManager		= nullptr;

#define DANMAKU_PBM1 GameData::Danmaku::player1BulletManager
#define DANMAKU_EBM1 GameData::Danmaku::enemy1BulletManager
#define DANMAKU_PBM2 GameData::Danmaku::player2BulletManager
#define DANMAKU_EBM2 GameData::Danmaku::enemy2BulletManager
#endif

#endif // MAKAI_BASE_BULLET_H
