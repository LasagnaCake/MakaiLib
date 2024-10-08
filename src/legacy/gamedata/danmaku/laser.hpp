template<Tweening::Tweenable T>
using	LaserParam = ObjectParam<T>;

struct LaserData: ObjectData {
	ObjectParam<float>		length;
	ObjectParam<Vector2>	width;
	bool	active	= false;
};

struct LineLaserData: LaserData {
	bool staticSpriteShape	= false;
};

struct BentLaserData: LaserData {
};

template <size_t LASER_COUNT, size_t ACTOR_LAYER, size_t ENEMY_LAYER> class LineLaserManager;

class LineLaser: public DanmakuObject {
public:
	LineLaser(): DanmakuObject() {
	}

	LineLaserData params;

	void onFrame(float delta) override {
		DANMAKU_FRAME_BEGIN;
		UPDATE_PARAM(vel)
		UPDATE_PARAM(rot)
		UPDATE_PARAM(width)
		UPDATE_PARAM(length)
		if (params.vel.current)
			local.position += VecMath::angleV2(params.rot.current) * params.vel.current * delta;
		local.rotation = params.rot.current;
		setLaserShape();
		updateSprite();
	}

	LineLaser* reset() override {
		DanmakuObject::reset();
		setZero();
		params.vel.current = params.vel.start;
		local.rotation =
		params.rot.current = params.rot.start;
		params.width.current = params.width.start;
		params.length.current = params.length.start;
		setLaserShape();
		updateSprite();
		return this;
	}

	LineLaser* setZero() override {
		DanmakuObject::setZero();
		params.width.current =
		params.width.factor =
		local.rotation =
		params.vel.current =
		params.vel.factor =
		params.rot.current =
		params.rot.factor =
		params.length.current = 0;
		return this;
	}

	LineLaser* enable() override {
		return setFree(false);
	}

	LineLaser* setFree(bool free = true) override {
		DanmakuObject::setFree(free);
		if (sprite) sprite->visible = !free;
		return this;
	}

	LineLaser* discard() override {
		if (params.discardable)
			despawn();
		return this;
	}

	LineLaser* forceDiscard() override {
		despawn();
		return this;
	}

	void setSpriteSize(Vector2 width, float const& length) {
		width	*= 0.5;
		sprite->setOrigin(
			Vector3(-width.x, +width.y),
			Vector3(length + width.x, +width.y),
			Vector3(-width.x, -width.y),
			Vector3(length + width.x, -width.y)
		);
	}

	void setLaserShape() {
		if (!sprite || params.staticSpriteShape) return;
		Vector2 width = (params.width.current * 0.5);
		float length = params.length.current;
		sprite->setOrigin(
			Vector3(-width.x, +width.y),
			Vector3(length + width.x, +width.y),
			Vector3(-width.x, -width.y),
			Vector3(length + width.x, -width.y)
		);
	}

	void updateSprite() override {
		if (!sprite) return;
		// Set sprite transforms
		sprite->local.position		= Vector3(local.position, zIndex + zOffset);
		sprite->local.rotation.z	= local.rotation;
		sprite->local.scale			= Vector3(local.scale, zScale)/* * (1+(1-factor)*2)*/;
	}

	template <class T>
	bool colliding(T target) {
		if (!params.active) return false;
		CapsuleBounds2D self = CapsuleBounds2D {
			local.position,
			(params.width.current / SQRT2) + params.hitbox.radius,
			params.length.current,
			params.rot.current,
			params.rotateHitbox ? params.hitbox.rotation : (params.hitbox.rotation - params.rot.current)
		};
		return withinBounds(target, self);
	}

	Function<Vector2(float)> getAsLineFunc(bool includeWidth = false) {
		Vector2 from = local.position;
		Vector2 to = VecMath::angleV2(params.rot.current);
		float trot = (
			params.rotateHitbox
		?	params.hitbox.rotation
		:	(params.hitbox.rotation - params.rot.current)
		);
		if (includeWidth){
			from -= VecMath::angleV2(params.rot.current - PI) * params.width.current;
			to *= Collision::getRadius(params.width.current, trot) + params.length.current;
		}
		else
			to *= params.length.current;
		to += local.position;
		return [=](float fac) -> Vector2 {
			return Math::lerp(from, to, Vector2(fac));
		};
	}

protected:
	void onObjectSpawnBegin()	override {params.collidable = false;}
	void onObjectSpawnEnd()		override {params.collidable = true;}
	void onObjectDespawnBegin()	override {params.collidable = false;}
	//void onObjectDespawnEnd()	override {setFree(true);}

private:
	template<size_t L, size_t A, size_t E> friend class LineLaserManager;
};

typedef std::vector<LineLaser*> LineLaserList;

template <
	size_t LASER_COUNT,
	size_t ACTOR_LAYER,
	size_t ENEMY_LAYER
>
class LineLaserManager: DanmakuObjectManager {
public:
	constexpr static size_t laserCount = LASER_COUNT;

	DERIVED_CLASS(LineLaserManager, DanmakuObjectManager)

	DERIVED_CONSTRUCTOR(LineLaserManager, DanmakuObjectManager, {
		mesh.setRenderLayer(ACTOR_LAYER);
		addToGroup(ACTOR_LAYER);
		mesh.material.shaded = false;
	})

	Renderable mesh;

	virtual ~LineLaserManager() {
		//delete &mesh;
		DEBUGLN("\nDeleting laser manager...");
		delete[] lasers;
	}

	void onFrame(float delta) override {
		GAME_PARALLEL_FOR
		for (size_t i = 0; i < LASER_COUNT; i++) {
			auto* l = &lasers[i];
			l->onFrame(delta);
			if (!l->isFree() && l->params.collidable) {
				for EACH(actor, Entities::groups.getGroup(ENEMY_LAYER)) {
					auto a = (AreaCircle2D*)actor;
					if (
						a->collision.enabled
						&& l->colliding(a->collision.shape)
					) {
						a->onCollision(this);
						l->discard();
					}
				}
			}
		}
	}

	void freeAll() {
		GAME_PARALLEL_FOR
		for SEACH(l, lasers, LASER_COUNT) l.setFree(); END_SEACH
	}

	void discardAll() {
		GAME_PARALLEL_FOR
		for SEACH(l, lasers, LASER_COUNT) l.discard(); END_SEACH
	}

	void forceDiscardAll() {
		GAME_PARALLEL_FOR
		for SEACH(l, lasers, LASER_COUNT) l.forceDiscard(); END_SEACH
	}

	size_t getFreeCount() {
		size_t count = 0;
		for SEACH(l, lasers, LASER_COUNT)
			if (l.isFree())
				count++;
		END_SEACH
		return count;
	}

	void create() {
		if (created) return;
		haltProcedure = false;
		for (size_t i = LASER_COUNT; i > 0; i--) {
			auto& l = lasers[i-1];
			if (!l.sprite) l.sprite = mesh.createReference<AnimatedPlane>();
			l.setFree(true);
			l.zOffset = (1 - ((((float)LASER_COUNT)-((float)i)) / ((float)LASER_COUNT)));
			if (haltProcedure) return;
		}
		created = true;
	}

	template <CollisionType T = CircleBounds2D>
	LineLaserList getInArea(T target) {
		LineLaserList res;
		GAME_PARALLEL_FOR
		for SEACH_IF(l, lasers, LASER_COUNT, !l.isFree() && l.params.collidable) {
			if (l.colliding(target))
			res.push_back(&l);
		} END_SEACH
		return res;
	}

	LineLaserList getActive() {
		LineLaserList res;
		GAME_PARALLEL_FOR
		for SEACH_IF(l, lasers, LASER_COUNT, !l.isFree()) res.push_back(&l); END_SEACH
		return res;
	}

	void forEach(Callback<LineLaser> func) {
		GAME_PARALLEL_FOR
		for SRANGE(i, 0, LASER_COUNT)
			func(lasers[i]);
	}

	void forEachActive(Callback<LineLaser> func) {
		GAME_PARALLEL_FOR
		for SRANGE(i, 0, LASER_COUNT)
			if (!lasers[i].isFree())
				func(lasers[i]);
	}

	template <CollisionType T>
	void forEachInArea(T area, Callback<LineLaser> func) {
		GAME_PARALLEL_FOR
		for SRANGE(i, 0, LASER_COUNT)
			if (
				!lasers[i].isFree()
			&&	lasers[i].params.collidable
			&&	Collision::withinBounds(lasers[i].params.hitbox, area)
			) func(lasers[i]);
	}

	LineLaser* getLastLineLaser() {
		return (LineLaser*)last;
	}

	LineLaser* createLineLaser() {
		LineLaser* current = nullptr;
		//GAME_PARALLEL_FOR
		for SEACH_IF(l, lasers, LASER_COUNT, l.isFree()) {
			current = l.enable()->setZero();
			#ifdef MAKAILIB_DANMAKU_PREVENT_INSTANCE_OVERFLOW_BY_WRAP
			pbobw = 0;
			#endif
			break;
		} END_SEACH
		if (!current)
		#ifndef MAKAILIB_DANMAKU_PREVENT_INSTANCE_OVERFLOW_BY_WRAP
			throw OutOfObjects(
				getName()
				+ ": Out of usable line lasers ("
				+ std::to_string(LASER_COUNT)
				+ ")!"
			);
		#else
			last = lasers[pbobw++].enable()->setZero();
		#endif
		else last = current;
		last->pause = Pause();
		last->params = LineLaserData();
		last->taskers.clearTaskers();
		last->flags.clear();
		last->clearSignals();
		last->setLaserShape();
		last->updateSprite();
		return last;
	}

	LineLaser* createLineLaser(LineLaserData laser) {
		LineLaser* l = createLineLaser();
		l->params = laser;
		return l->reset();
	}

	LineLaser* lasers = new LineLaser[LASER_COUNT];

	bool haltProcedure = false;

private:
	bool created = false;
	#ifdef MAKAILIB_DANMAKU_PREVENT_INSTANCE_OVERFLOW_BY_WRAP
	size_t pbobw = 0;
	#endif
	LineLaser* last = nullptr;
};

#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
typedef LineLaserManager<PLAYER_LASER_COUNT, PLAYER_LASER_LAYER, ENEMY_LAYER>	PlayerLineLaserManager;
typedef LineLaserManager<ENEMY_LASER_COUNT, ENEMY_LASER_LAYER, PLAYER_LAYER>	EnemyLineLaserManager;
PlayerLineLaserManager*	playerLineLaserManager = nullptr;
EnemyLineLaserManager*	enemyLineLaserManager = nullptr;

#define DANMAKU_PLLM GameData::Danmaku::playerLineLaserManager
#define DANMAKU_ELLM GameData::Danmaku::enemyLineLaserManager
#else
typedef LineLaserManager<PLAYER_LASER_COUNT/2, PLAYER1_LASER_LAYER, ENEMY1_LAYER>	Player1LineLaserManager;
typedef LineLaserManager<PLAYER_LASER_COUNT/2, PLAYER2_LASER_LAYER, ENEMY2_LAYER>	Player2LineLaserManager;
typedef LineLaserManager<ENEMY_LASER_COUNT/2, ENEMY1_LASER_LAYER, PLAYER1_LAYER>	Enemy1LineLaserManager;
typedef LineLaserManager<ENEMY_LASER_COUNT/2, ENEMY2_LASER_LAYER, PLAYER2_LAYER>	Enemy2LineLaserManager;
Player1LineLaserManager*	player1LineLaserManager	= nullptr;
Enemy1LineLaserManager*		enemy1LineLaserManager	= nullptr;
Player2LineLaserManager*	player2LineLaserManager	= nullptr;
Enemy2LineLaserManager*		enemy2LineLaserManager	= nullptr;

#define DANMAKU_PLLM1 GameData::Danmaku::player1LineLaserManager
#define DANMAKU_ELLM1 GameData::Danmaku::enemy1LineLaserManager
#define DANMAKU_PLLM2 GameData::Danmaku::player2LineLaserManager
#define DANMAKU_ELLM2 GameData::Danmaku::enemy2LineLaserManager
#endif
