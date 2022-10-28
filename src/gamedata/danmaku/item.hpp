struct CollectibleData: GenericObjectData {
	size_t type = $item(POWER);
	size_t value = 1;
	float gravity = 0.1;
	bool spin = false;
};

struct Collectible: DanmakuObject {
	Collectible() : DanmakuObject() {
		auto pass = $tsignal(Object*) {};
	}

	CollectibleData params;

	void onFrame(float delta) override {
		DANMAKU_FRAME_BEGIN;
		if (params.spin) local.rotation += 1.0/60.0;
		gravity = Math::clamp(gravity + params.gravity, -params.gravity * 10.0, params.gravity);
		local.position.y -= gravity;
	}

	Collectible* reset() override {
		setZero();
		gravity = -params.gravity * 10.0;
	}

	Collectible* setZero() override {
		sprite->local.rotation.z =
		gravity = 0;
	}

	Collectible* setFree(bool state = true) override {
		DanmakuObject::setFree(state);
		if (sprite) sprite->visible = !free;
		return this;
	}

	Collectible* discard() override {
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
		sprite->local.rotation.z = local.rotation;
		// Set sprite scale
		sprite->local.scale = Vector3(local.scale, zScale);
	}

private:
	float gravity = 0;
};

typedef std::vector<Collectible*> CollectibleList;

template <size_t ITEM_COUNT>
struct CollectibleManager: Entity {
	DERIVED_CLASS(CollectibleManager, Entity)

	DERIVED_CONSTRUCTOR(CollectibleManager, Entity, {
		addToGame(this, "DanmakuGame");
		mesh.setRenderLayer($layer(ITEM));
		$ecl groups.addEntity(this, $layer(ITEM));
	})

	$cdt BoxBounds2D playfield;

	Renderable mesh;

	void onDelete() override {
		$ecl groups.removeFromAll(this);
	}

	void onFrame(float delta) override {
		for $each(item, items) {
			item.onFrame(delta);
			if (!item.isFree() && item.params.collidable) {
				auto a = getMainPlayer();
				if (a)
					if (
						a->collision.enabled
						&& $cdt withinBounds(
							item.params.hitbox,
							a->getCircleBounds()
						)
					) {
						a->on;
						item.discard();
					}
			}
			if (item.params.dope)
				if (
					! $cdt withinBounds(
						item.params.hitbox,
						playfield
					)
				) {
					item.setFree(true);
				}
		}
	}

	void freeAll() {
		for $each(item, items) item.setFree();
	}

	void discardAll() {
		for $each(item, items) item.discard();
	}

	size_t getFreeCount() {
		size_t count = 0;
		for $each(item, items)
			if (item.isFree())
				count++;
		return count;
	}

	void create() {
		if (created) return;
		haltProcedure = false;
		for (size_t i = 0; i < ITEM_COUNT; i++) {
			if (!items[i].sprite)
				items[i].sprite =
					mesh.createReference<AnimatedPlane>();
			items[i].setFree(true);
			items[i]._setZOffset(Math::epsilonF * ((float)i));
			if (haltProcedure) return;
		}
		created = true;
	}

	CollectibleList getInArea($cdt CircleBounds2D target) {
		CollectibleList res;
		for $eachif(item, items, !item.isFree() && item.params.collidable) {
			if (
				$cdt withinBounds(
					item.params.hitbox,
					target
				)
			) {
				res.push_back(&item);
			}
		}
		return res;
	}

	CollectibleList getInArea($cdt BoxBounds2D target) {
		CollectibleList res;
		for $eachif(item, items, !item.isFree() && item.params.collidable) {
			if (
				$cdt withinBounds(
					item.params.hitbox,
					target
				)
			) res.push_back(&item);
		}
		return res;
	}

	CollectibleList getActive() {
		CollectibleList res;
		for $eachif(item, items, !item.isFree()) res.push_back(&item);
		return res;
	}

	Collectible* getLastCollectible() {
		return last;
	}

	Collectible* createCollectible() {
		for $eachif(item, items, item.isFree()) {
			last = item.enable()->setZero();
			last->pause = Pause();
			last->params = CollectibleData();
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
			+ ": Out of usable items ("
			+ std::to_string(ITEM_COUNT)
			+ ")!"
		);
		#else
		last = items[pbobw++].enable()->setZero();
		last->params = CollectibleData();
		last->taskers.clearTaskers();
		if (pbobw > ITEM_COUNT) pbobw = 0;
		last->updateSprite();
		return last;
		#endif
	}

	Collectible* createCollectible(CollectibleData item) {
		Collectible* c = createCollectible();
		c->params = item;
		return c->reset();
	}

	Collectible items[ITEM_COUNT];

	bool haltProcedure = false;

private:
	bool created = false;
	#ifdef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
	size_t pbobw = 0;
	#endif
	Collectible* last = nullptr;
};

typedef CollectibleManager<COLLECTIBLE_ITEM_COUNT> ItemManager;

ItemManager* itemManager = nullptr;
