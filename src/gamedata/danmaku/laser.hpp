typedef ObjectParam	LaserParam;

struct LaserData: ObjectData {
	ObjectParam length;
	ObjectParam width;
	bool active	= false;
};

struct LineLaserData: LaserData {
};

struct BentLaserData: LaserData {
};

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
		local.rotation =
		params.vel.current =
		params.vel.factor =
		params.rot.current =
		params.rot.factor =
		params.width.current =
		params.width.factor =
		params.length.current =
		params.length.factor = 0;
		return this;
	}

	LineLaser* enable() override {
		return setFree(false);
	}

	LineLaser* setFree(bool state = true) override {
		DanmakuObject::setFree(state);
		if (sprite)	sprite->visible	= !free;
		return this;
	}

	LineLaser* discard() override {
		if (params.discardable)
			setFree();
		return this;
	}

	void setLaserShape() {
		if (!sprite) return;
		float hWidth = (params.width.current / (2.0 + !params.active * 2.0));
		float curLen = params.length.current;
		sprite->setOrigin(
			$vec3(-hWidth, hWidth),
			$vec3(curLen + hWidth, hWidth),
			$vec3(-hWidth, -hWidth),
			$vec3(curLen + hWidth, -hWidth)
		);
	}

	void updateSprite() override {
		if (!sprite) return;
		// Copy position data to sprite
		sprite->local.position		= $vec3(local.position, zIndex + _zOffset);
		sprite->local.rotation.z	= local.rotation;
		sprite->local.scale			= Vector3(local.scale, zScale);
	}

	template <class T>
	bool colliding(T target) {
		if (!params.active) return false;
		RayBounds2D self = RayBounds2D {
			local.position,
			params.length.current,
			params.width.current / SQRT2,
			params.rot.current
		};
		return withinBounds(target, self);
	}

	std::function<Vector2(float)> getAsLineFunc( bool includeWidth = false) {
		Vector2 from = local.position;
		Vector2 to = $vmt angleV2(params.rot.current);
		if (includeWidth){
			from -= $vmt angleV2(params.rot.current - PI) * params.width.current;
			to *= params.length.current + params.width.current;
		}
		else
			to *= params.length.current;
		to += local.position;
		return [=](float fac) -> Vector2 {
			return $mth lerp(from, to, $vec2(fac));
		};
	}
};

typedef std::vector<LineLaser*> LineLaserList;

template <
	size_t LASER_COUNT,
	size_t ACTOR_LAYER = $layer(ENEMY_BULLET),
	size_t ENEMY_LAYER = $layer(PLAYER)
>
class LineLaserManager: Entity {
public:
	DERIVED_CLASS(LineLaserManager, Entity)

	DERIVED_CONSTRUCTOR(LineLaserManager, Entity, {
		addToGame(this, "DanmakuGame");
		mesh.setRenderLayer(ACTOR_LAYER);
		addToGroup(ACTOR_LAYER);
		mesh.material.shaded = false;
	})

	Renderable mesh;

	virtual ~LineLaserManager() {
		//delete &mesh;
		$debug("\nDeleting laser manager...");
		delete[] lasers;
	}

	void onFrame(float delta) override {
		GAME_PARALLEL_FOR
		for (auto i = 0; i < LASER_COUNT; i++) {
			auto* l = &lasers[i];
			l->onFrame(delta);
			if (!l->isFree() && l->params.collidable) {
				for $each(actor, $ecl groups.getGroup(ENEMY_LAYER)) {
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
		for $seach(l, lasers, LASER_COUNT) l.setFree(); $endseach
	}

	void discardAll() {
		GAME_PARALLEL_FOR
		for $seach(l, lasers, LASER_COUNT) l.discard(); $endseach
	}

	size_t getFreeCount() {
		size_t count = 0;
		for $seach(l, lasers, LASER_COUNT)
			if (l.isFree())
				count++;
		$endseach
		return count;
	}

	void create() {
		if (created) return;
		haltProcedure = false;
		for (size_t i = LASER_COUNT; i > 0; i--) {
			auto& l = lasers[i-1];
			if (!l.sprite) l.sprite = mesh.createReference<AnimatedPlane>();
			l.setFree(true);
			l._setZOffset(
				(1 - ((((float)LASER_COUNT)-((float)i)) / ((float)LASER_COUNT)))
			);
			if (haltProcedure) return;
		}
		created = true;
	}

	template <CollisionType T = CircleBounds2D>
	LineLaserList getInArea(T target) {
		LineLaserList res;
		GAME_PARALLEL_FOR
		for $seachif(l, lasers, LASER_COUNT, !l.isFree() && l.params.collidable) {
			if (l.colliding(target))
			res.push_back(&l);
		} $endseach
		return res;
	}

	LineLaserList getActive() {
		LineLaserList res;
		GAME_PARALLEL_FOR
		for $seachif(l, lasers, LASER_COUNT, !l.isFree()) res.push_back(&l); $endseach
		return res;
	}

	void forEach(Callback<LineLaser> func) {
		GAME_PARALLEL_FOR
		for $ssrange(i, 0, LASER_COUNT)
			func(lasers[i]);
	}

	void forEachActive(Callback<LineLaser> func) {
		GAME_PARALLEL_FOR
		for $ssrange(i, 0, LASER_COUNT)
			if (!lasers[i].isFree())
				func(lasers[i]);
	}

	template <CollisionType T>
	void forEachInArea(T area, Callback<LineLaser> func) {
		GAME_PARALLEL_FOR
		for $ssrange(i, 0, LASER_COUNT)
			if (
				!lasers[i].isFree()
			&&	lasers[i].params.collidable
			&&	$cdt withinBounds(lasers[i].params.hitbox, area)
			) func(lasers[i]);
	}

	LineLaser* getLastLineLaser() {
		return (LineLaser*)last;
	}

	LineLaser* createLineLaser() {
		//GAME_PARALLEL_FOR
		for $seachif(l, lasers, LASER_COUNT, l.isFree()) {
			last = l.enable()->setZero();
			last->pause = Pause();
			last->params = LineLaserData();
			last->taskers.clearTaskers();
			last->flags.clear();
			last->clearSignals();
			last->setLaserShape();
			last->updateSprite();
			#ifdef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
			pbobw = 0;
			#endif
			return last;
		} $endseach
		#ifndef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
		throw OutOfObjects(
			getName()
			+ ": Out of usable lasers ("
			+ std::to_string(LASER_COUNT)
			+ ")!"
		);
		#else
		last = lasers[pbobw++].enable()->setZero();
		last->params = LineLaserData();
		last->taskers.clearTaskers();
		last->flags.clear();
		last->clearSignals();
		if (pbobw > LASER_COUNT) pbobw = 0;
		last->setLaserShape();
		last->updateSprite();
		return last;
		#endif
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
	#ifdef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
	size_t pbobw = 0;
	#endif
	LineLaser* last = nullptr;
};

typedef LineLaserManager<PLAYER_LASER_COUNT, $layer(PLAYER_LASER), $layer(ENEMY)>	PlayerLineLaserManager;
typedef LineLaserManager<ENEMY_LASER_COUNT, $layer(ENEMY_LASER), $layer(PLAYER)>	EnemyLineLaserManager;

PlayerLineLaserManager*	playerLineLaserManager = nullptr;
EnemyLineLaserManager*	enemyLineLaserManager = nullptr;

#define DANMAKU_PLLM $dmk playerLineLaserManager
#define DANMAKU_ELLM $dmk enemyLineLaserManager

#define $linelaser(TYPE)	$getman( TYPE##LineLaser )
#define $setll(TYPE)		$setman( TYPE##LineLaser )
