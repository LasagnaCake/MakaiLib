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
		auto pass = $tsignal(LineLaser*) {};
	}

	$ref AnimatedPlane* head = nullptr;
	$ref AnimatedPlane* tail = nullptr;
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
		if (head)	head->visible	= !free;
		if (tail)	tail->visible	= !free;
		return this;
	}

	LineLaser* discard() override {
		if (params.discardable)
			setFree();
		return this;
	}

	LineLaser* setLaserColor(Vector4 color) {
		sprite->setColor(color);
		head->setColor(color);
		tail->setColor(color);
		return this;
	}

	LineLaser* setLaserUVSize(Vector2 size) {
		sprite->size = size;
		head->size = size;
		tail->size = size;
		return this;
	}

	LineLaser* setLaserUVFrame(Vector2 frame = Vector2(0)) {
		sprite->frame = frame;
		head->frame = frame;
		tail->frame = frame;
		return this;
	}

	void setLaserShape() {
		if (!sprite) return;
		float hWidth = (params.width.current / (2.0 + !params.active * 2.0));
		float curLen = params.length.current;
		sprite->setOrigin(
			$vec3(hWidth, hWidth),
			$vec3(curLen - hWidth, hWidth),
			$vec3(hWidth, -hWidth),
			$vec3(curLen - hWidth, -hWidth)
		);
		if (head) {
			head->setOrigin(
				$vec3(-hWidth, hWidth),
				$vec3(hWidth, hWidth),
				$vec3(-hWidth, -hWidth),
				$vec3(hWidth, -hWidth)
			);
		}
		if (tail) {
			tail->setOrigin(
				$vec3(-hWidth + curLen, hWidth),
				$vec3(hWidth + curLen, hWidth),
				$vec3(-hWidth + curLen, -hWidth),
				$vec3(hWidth + curLen, -hWidth)
			);
		}
	}

	void updateSprite() override {
		if (!sprite) return;
		// Copy position data to sprite
		sprite->local.position		= $vec3(local.position, zIndex + _zOffset);
		sprite->local.rotation.z	= local.rotation;
		sprite->local.scale			= Vector3(local.scale, zScale);
		// Ditto for head
		if (head) {
			head->local.position	= $vec3(local.position, zIndex + _zOffset);
			head->local.rotation.z	= local.rotation;
			head->local.scale		= Vector3(local.scale, zScale);
		}
		// Ditto for tail
		if (tail) {
			tail->local.position	= $vec3(local.position, zIndex + _zOffset);
			tail->local.rotation.z	= local.rotation;
			tail->local.scale		= Vector3(local.scale, zScale);
		}
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
	size_t ENEMY_LAYER = $layer(PLAYER),
	size_t COLLI_LAYER = ACTOR_LAYER
>
class LineLaserManager: Entity {
public:
	DERIVED_CLASS(LineLaserManager, Entity)

	DERIVED_CONSTRUCTOR(LineLaserManager, Entity, {
		addToGame(this, "DanmakuGame");
		mesh.setRenderLayer(ACTOR_LAYER);
		$ecl groups.addEntity(this, ACTOR_LAYER);
		$ecl collisionLayers.addEntity(this, COLLI_LAYER);
		mesh.material.shaded = false;
	})

	Renderable mesh;

	void onDelete() override {
		//delete &mesh;
		$debug("\nDeleting laser manager...");
		$ecl groups.removeFromAll(this);
		delete[] lasers;
	}

	void onFrame(float delta) override {
		#ifdef $_PARALLEL_MANAGERS
		$speach(l, lasers, LASER_COUNT) {
				l.onFrame(delta);
				if (!l.isFree() && l.params.collidable) {
					for $each(actor, $ecl groups.getGroup(ENEMY_LAYER)) {
						auto a = (AreaCircle2D*)actor;
						if (
							a->collision.enabled
							&& l.colliding(a->collision.shape)
						) {
							a->onCollision(this);
							l.discard();
						}
					}
				}
			}
		$endspeach
		#else
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
		#endif
	}

	void freeAll() {
		#ifdef $_PARALLEL_MANAGERS
		$speach(l, lasers, LASER_COUNT) {l.setFree();} $endspeach
		#else
		for $seach(l, lasers, LASER_COUNT) l.setFree(); $endseach
		#endif
	}

	void discardAll() {
		#ifdef $_PARALLEL_MANAGERS
		$speach(l, lasers, LASER_COUNT) {l.discard();} $endspeach
		#else
		for $seach(l, lasers, LASER_COUNT) l.discard(); $endseach
		#endif
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
		for (size_t i = 0; i < LASER_COUNT; i++) {
			if (!lasers[i].sprite)
				lasers[i].sprite =
					mesh.createReference<AnimatedPlane>();
			if (!lasers[i].head)
				lasers[i].head =
					mesh.createReference<AnimatedPlane>();
			if (!lasers[i].tail)
				lasers[i].tail =
					mesh.createReference<AnimatedPlane>();
			lasers[i].setFree(true);
			lasers[i]._setZOffset(Math::epsilonF * ((float)i));
			if (haltProcedure) return;
		}
		created = true;
	}

	LineLaserList getInArea($cdt CircleBounds2D target) {
		LineLaserList res;
		for $seachif(l, lasers, LASER_COUNT, !l.isFree() && l.params.collidable) {
			if (l.colliding(target))
				res.push_back(&l);
		} $endseach
		return res;
	}

	LineLaserList getInArea($cdt BoxBounds2D target) {
		LineLaserList res;
		for $seachif(l, lasers, LASER_COUNT, !l.isFree() && l.params.collidable) {
			if (l.colliding(target))
			res.push_back(&l);
		} $endseach
		return res;
	}

	LineLaserList getActive() {
		LineLaserList res;
		for $seachif(l, lasers, LASER_COUNT, !l.isFree()) res.push_back(&l); $endseach
		return res;
	}

	LineLaser* getLastLineLaser() {
		return (LineLaser*)last;
	}

	LineLaser* createLineLaser() {
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
		throw std::runtime_error(
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

typedef LineLaserManager<PLAYER_LASER_COUNT, $layer(PLAYER_LASER), $layer(ENEMY), $layer(PLAYER)>	PlayerLineLaserManager;
typedef LineLaserManager<ENEMY_LASER_COUNT, $layer(ENEMY_LASER), $layer(PLAYER), $layer(ENEMY)>		EnemyLineLaserManager;

PlayerLineLaserManager*	playerLineLaserManager = nullptr;
EnemyLineLaserManager*	enemyLineLaserManager = nullptr;

#define DANMAKU_PLLM $dmk playerLineLaserMananger
#define DANMAKU_ELLM $dmk enemyLineLaserManager

#define $linelaser(TYPE)	$getman( TYPE##LineLaser )
#define $setll(TYPE)		$setman( TYPE##LineLaser )
