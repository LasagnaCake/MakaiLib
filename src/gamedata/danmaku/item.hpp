struct CollectibleData: GenericObjectData {
	size_t type		= $item(POWER);
	float value		= 1;
	float gravity	= 1;
	float bounce	= 10;
	bool spin		= false;
	bool pocable	= true;
};

CollectibleData getItemData(size_t type, float value = 1, bool autoCollect = true) {
	CollectibleData item;
	item.type		= type;
	item.value		= value;
	item.pocable	= autoCollect;
	return item;
}

struct Collectible: DanmakuObject {
	Collectible() : DanmakuObject() {
	}

	CollectibleData params;

	void onFrame(float delta) override {
		DANMAKU_FRAME_BEGIN;
		if (params.spin) local.rotation += delta;
		gravity = Math::clamp(gravity + params.gravity * delta, -params.bounce * delta, params.gravity * delta * 10.0f);
		local.position.y -= gravity;
		if (pocing && target.exists())
			local.position = Math::lerp(local.position, *target, Vector2(10 * delta));
		updateSprite();
	}

	WeakPointer<DanmakuObject> reset() override {
		setZero();
		gravity = -params.bounce;
		return this;
	}

	WeakPointer<DanmakuObject> setZero() override {
		sprite->local.rotation.z =
		gravity = 0;
		return this;
	}

	WeakPointer<DanmakuObject> setFree(bool state = true) override {
		DanmakuObject::setFree(state);
		if (sprite) sprite->visible = !free;
		pocing = false;
		target.unbind();
		return this;
	}

	WeakPointer<DanmakuObject> discard() override {
		if (params.discardable)
			setFree();
		return this;
	}

	WeakPointer<DanmakuObject> enable() override {
		return setFree(false);
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

	WeakPointer<Collectible> setAutoCollect(WeakPointer<Vector2> target = nullptr) {
		if (!params.pocable) return this;
		pocing = true;
		this->target = target;
		return this;
	}

	WeakPointer<Vector2> target;
	bool pocing = false;

private:
	float gravity = 0;
};

typedef std::vector<WeakPointer<Collectible>> CollectibleList;

template <size_t ITEM_COUNT>
class CollectibleManager: Entity {
public:
	DERIVED_CLASS(CollectibleManager, Entity)

	DERIVED_CONSTRUCTOR(CollectibleManager, Entity, {
		addToGame(this, "DanmakuGame");
		mesh.setRenderLayer($layer(ITEM));
		addToGroup($layer(ITEM));
		$ecl collisionLayers.addObject(this, $layer(ITEM));
		mesh.material.shaded = false;
	})

	$cdt BoxBounds2D playfield;

	float poc = 0;

	Renderable mesh;

	void onDelete() override {
		//delete &mesh;
		$debug("\nDeleting item manager...");
		delete[] items;
	}

	void onFrame(float delta) override {
		#ifdef $_PARALLEL_MANAGERS
		$speach(item, items, ITEM_COUNT) {
				item.onFrame(delta);
				if (!item.isFree() && item.params.collidable) {
					auto a = (PlayerEntity2D*)mainPlayer;
					if (a) {
						auto targetBounds = a->getGrazeBounds();
						if (
							$cdt withinBounds(
								item.params.hitbox,
								targetBounds
							)
						) {
							a->onItemGet(item.params.type, item.params.value);
							item.discard();
						} else if (
							(a->position.y > poc)
							&& item.params.pocable
						) {
							item.setAutoCollect(&a->position);
						}
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
		$endspeach
		#else
		GAME_PARALLEL_FOR
		for $seach(item, items, ITEM_COUNT)
			item.onFrame(delta);
			if (!item.isFree() && item.params.collidable) {
				auto a = (PlayerEntity2D*)&(*mainPlayer);
				if (a) {
					auto targetBounds = a->getGrazeBounds();
					if (
						$cdt withinBounds(
							item.params.hitbox,
							targetBounds
						)
					) {
						a->onItemGet(item.params.type, item.params.value);
						item.discard();
					} else if (
						(a->position.y > poc)
						&& item.params.pocable
					) {
						item.setAutoCollect(&a->position);
					}
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
		$endseach
		#endif
	}

	void collectAll(bool forceCollect = false, bool forceDiscard = false) {
		auto mp = getMainPlayer();
		GAME_PARALLEL_FOR
		for $seachif(item, items, ITEM_COUNT, !item.isFree()) {
			if (forceCollect) {
				mp->onItemGet(
					item.params.type,
					item.params.value
				);
				if (forceDiscard)
					item.setFree();
				else
					item.discard();
			} else {
				item.setAutoCollect(&mp->position);
			}
		} $endseach
	}

	void freeAll() {
		#ifdef $_PARALLEL_MANAGERS
		$speach(item, items, ITEM_COUNT) {item.setFree();} $endspeach
		#else
		GAME_PARALLEL_FOR
		for $seach(item, items, ITEM_COUNT) item.setFree(); $endseach
		#endif
	}

	void discardAll() {
		#ifdef $_PARALLEL_MANAGERS
		$speach(item, items, ITEM_COUNT) {item.discard();} $endspeach
		#else
		GAME_PARALLEL_FOR
		for $seach(item, items, ITEM_COUNT) item.discard(); $endseach
		#endif
	}

	size_t getFreeCount() {
		size_t count = 0;
		GAME_PARALLEL_FOR
		for $seach(item, items, ITEM_COUNT)
			if (item.isFree())
				count++;
		$endseach
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

	template <CollisionType T = CircleBounds2D>
	CollectibleList getInArea(T target) {
		CollectibleList res;
		GAME_PARALLEL_FOR
		for $seachif(item, items, ITEM_COUNT, !item.isFree() && item.params.collidable) {
			if (
				$cdt withinBounds(
					item.params.hitbox,
					target
				)
			) res.push_back(&item);
		} $endseach
		return res;
	}

	CollectibleList getActive() {
		CollectibleList res;
		GAME_PARALLEL_FOR
		for $seachif(item, items, ITEM_COUNT, !item.isFree()) res.push_back(&item); $endseach
		return res;
	}

	void forEach(Callback<Collectible> func) {
		GAME_PARALLEL_FOR
		for $ssrange(i, 0, ITEM_COUNT)
			func(items[i]);
	}

	void forEachFree(Callback<Collectible> func) {
		GAME_PARALLEL_FOR
		for $ssrange(i, 0, ITEM_COUNT)
			if (items[i].isFree())
				func(items[i]);
	}

	template <CollisionType T>
	void forEachInArea(T area, Callback<Collectible> func) {
		GAME_PARALLEL_FOR
		for $ssrange(i, 0, ITEM_COUNT)
			if (
				items[i].isFree()
			&&	items[i].params.collidable
			&&	$cdt withinBounds(items[i].params.hitbox, area)
			) func(items[i]);
	}

	WeakPointer<Collectible> getLastCollectible() {
		return last;
	}

	WeakPointer<Collectible> createCollectible() {
		//GAME_PARALLEL_FOR
		for $seachif(item, items, ITEM_COUNT, item.isFree()) {
			last = item.enable()->setZero().castedTo<Collectible>();
			last->pause = Pause();
			last->params = CollectibleData();
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
			+ ": Out of usable items ("
			+ std::to_string(ITEM_COUNT)
			+ ")!"
		);
		#else
		last = items[pbobw++].enable()->setZero().castedTo<Collectible>();
		last->params = CollectibleData();
		last->taskers.clearTaskers();
		last->flags.clear();
		last->clearSignals();
		if (pbobw > ITEM_COUNT) pbobw = 0;
		last->updateSprite();
		return last;
		#endif
	}

	WeakPointer<Collectible> createCollectible(CollectibleData item) {
		WeakPointer<Collectible> c = createCollectible();
		c->params = item;
		return c->reset().castedTo<Collectible>();
	}

	CollectibleList createCollectible(CollectibleData item, size_t count, Vector2 at, float radius = 1, Vector2 scale = Vector2(1), float angleOffset = 0, float spread = TAU) {
		CollectibleList res;
		GAME_PARALLEL_FOR
		for $ssrange(i, 0, count) {
			res.push_back(createCollectible(item));
			last->local.position = at + ($vmt angleV2(spread * ((float)i / (float)count) + angleOffset) * radius);
			last->local.scale = scale;
			last->reset();
			last->updateSprite();
		}
		return res;
	}

	Collectible* items = new Collectible[ITEM_COUNT];

	bool haltProcedure = false;

private:
	bool created = false;
	#ifdef $_PREVENT_BULLET_OVERFLOW_BY_WRAP
	size_t pbobw = 0;
	#endif
	WeakPointer<Collectible> last;
};

typedef CollectibleManager<COLLECTIBLE_ITEM_COUNT> ItemManager;

Pointer<ItemManager> itemManager;

#define DANMAKU_IM $dmk itemManager
