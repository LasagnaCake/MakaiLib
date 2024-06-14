struct CollectibleData: GenericObjectData {
	size_t type		= ItemType::POWER_ITEM;
	float value		= 1;
	float gravity	= 1;
	float bounce	= 10;
	float spin		= 0;
	bool pocable	= true;
};

CollectibleData getItemData(size_t type, float value = 1, bool autoCollect = true) {
	CollectibleData item;
	item.type		= type;
	item.value		= value;
	item.pocable	= autoCollect;
	return item;
}

template <size_t ITEM_COUNT, size_t ACTOR_LAYER> class CollectibleManager;

struct Collectible: DanmakuObject {
	Collectible() : DanmakuObject() {
	}

	CollectibleData params;

	void onFrame(float delta) override {
		DANMAKU_FRAME_BEGIN;
		local.rotation			+= params.spin;
		params.hitbox.rotation	+= params.spin;
		gravity = Math::clamp(gravity + params.gravity * delta, -params.bounce * delta, params.gravity * delta * 10.0f);
		local.position.y -= gravity;
		if (pocing && target)
			local.position = Math::lerp(local.position, *target, Vector2(10 * delta));
		updateSprite();
	}

	Collectible* reset() override {
		DanmakuObject::reset();
		setZero();
		gravity = -params.bounce;
		return this;
	}

	Collectible* setZero() override {
		DanmakuObject::setZero();
		sprite->local.rotation.z =
		local.rotation =
		params.hitbox.rotation =
		gravity = 0;
		return this;
	}

	Collectible* setFree(bool free = true) override {
		DanmakuObject::setFree(free);
		if (sprite) sprite->visible = !free;
		pocing = false;
		target = nullptr;
		return this;
	}

	Collectible* discard() override {
		if (params.discardable)
			setFree();
		return this;
	}

	Collectible* forceDiscard() override {
		setFree();
		return this;
	}

	Collectible* enable() override {
		return (Collectible*)setFree(false);
	}

	void updateSprite() override {
		if (!sprite) return;
		// Set sprite transforms
		sprite->local.position = Vector3(local.position, zIndex + zOffset);
		sprite->local.rotation.z = local.rotation;
		sprite->local.scale = Vector3(local.scale, zScale);
		// Set hitbox transforms
		params.hitbox.position	= local.position;
		params.hitbox.rotation	= local.rotation;
	}

	Collectible* setAutoCollect(Vector2* target = nullptr) {
		if (!params.pocable) return this;
		pocing = true;
		this->target = target;
		return this;
	}

	Vector2* target = nullptr;
	bool pocing = false;

private:
	float gravity = 0;

	template<size_t C, size_t A> friend class CollectibleManager;
};

typedef std::vector<Collectible*> CollectibleList;

template <size_t ITEM_COUNT, size_t ACTOR_LAYER>
class CollectibleManager: public DanmakuObjectManager {
public:
	constexpr static size_t itemCount = ITEM_COUNT;

	DERIVED_CLASS(CollectibleManager, DanmakuObjectManager)

	DERIVED_CONSTRUCTOR(CollectibleManager, DanmakuObjectManager, {
		mesh.setRenderLayer(ITEM_LAYER);
		addToGroup(ITEM_LAYER);
		mesh.material.shaded = false;
	})

	Collision::BoxBounds2D playfield;

	float poc = 0;

	Renderable mesh;

	virtual ~CollectibleManager() {
		//delete &mesh;
		DEBUGLN("\nDeleting item manager...");
		delete[] items;
	}

	void onFrame(float delta) override {
		GAME_PARALLEL_FOR
		for SEACH(item, items, ITEM_COUNT)
			for EACH(actor, Entities::collisionLayers.getGroup(ACTOR_LAYER)) {
				item.onFrame(delta);
				if (!item.isFree() && item.params.collidable) {
					auto a = (PlayerEntity2D*)actor;
					if (a) {
						auto targetBounds = a->getGrazeBounds();
						auto targetItembox = a->getItemBounds();
						if (
							Collision::withinBounds(
								item.params.hitbox,
								targetBounds
							)
						) {
							a->onItemGet(item.params.type, item.params.value);
							item.discard();
						} else if (
							(
								(a->position.y > poc)
							&&	item.params.pocable
							)
						||	Collision::withinBounds(
								item.params.hitbox,
								targetItembox
							)
						) {
							item.setAutoCollect(&a->position);
						}
					}
				}
				if (item.params.dope)
					if (
						! Collision::withinBounds(
							item.params.hitbox,
							playfield
						)
					) {
						item.setFree(true);
					}
			}
		END_SEACH
	}

	void collectAll(bool forceCollect = false, bool forceDiscard = false) {
		auto mp = getMainPlayer();
		GAME_PARALLEL_FOR
		for SEACH_IF(item, items, ITEM_COUNT, !item.isFree()) {
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
		} END_SEACH
	}

	void freeAll() {
		GAME_PARALLEL_FOR
		for SEACH(item, items, ITEM_COUNT) item.setFree(); END_SEACH
	}

	void discardAll() {
		GAME_PARALLEL_FOR
		for SEACH(item, items, ITEM_COUNT) item.discard(); END_SEACH
	}

	void forceDiscardAll() {
		GAME_PARALLEL_FOR
		for SEACH(item, items, ITEM_COUNT) item.forceDiscard(); END_SEACH
	}

	size_t getFreeCount() {
		size_t count = 0;
		GAME_PARALLEL_FOR
		for SEACH(item, items, ITEM_COUNT)
			if (item.isFree())
				count++;
		END_SEACH
		return count;
	}

	void create() {
		if (created) return;
		haltProcedure = false;
		for (size_t i = ITEM_COUNT; i > 0; i--) {
			auto& c = items[i-1];
			if (!c.sprite) c.sprite = mesh.createReference<AnimatedPlane>();
			c.setFree(true);
			c.zOffset = (1 - ((((float)ITEM_COUNT)-((float)i)) / ((float)ITEM_COUNT)));
			if (haltProcedure) return;
		}
		created = true;
	}

	template <CollisionType T = CircleBounds2D>
	CollectibleList getInArea(T target) {
		CollectibleList res;
		GAME_PARALLEL_FOR
		for SEACH_IF(item, items, ITEM_COUNT, !item.isFree() && item.params.collidable) {
			if (
				Collision::withinBounds(
					item.params.hitbox,
					target
				)
			) res.push_back(&item);
		} END_SEACH
		return res;
	}

	CollectibleList getActive() {
		CollectibleList res;
		GAME_PARALLEL_FOR
		for SEACH_IF(item, items, ITEM_COUNT, !item.isFree()) res.push_back(&item); END_SEACH
		return res;
	}

	void forEach(Callback<Collectible> func) {
		GAME_PARALLEL_FOR
		for SSRANGE(i, 0, ITEM_COUNT)
			func(items[i]);
	}

	void forEachActive(Callback<Collectible> func) {
		GAME_PARALLEL_FOR
		for SSRANGE(i, 0, ITEM_COUNT)
			if (!items[i].isFree())
				func(items[i]);
	}

	template <CollisionType T>
	void forEachInArea(T area, Callback<Collectible> func) {
		GAME_PARALLEL_FOR
		for SSRANGE(i, 0, ITEM_COUNT)
			if (
				!items[i].isFree()
			&&	items[i].params.collidable
			&&	Collision::withinBounds(items[i].params.hitbox, area)
			) func(items[i]);
	}

	Collectible* getLastCollectible() {
		return (Collectible*)last;
	}

	Collectible* createCollectible() {
		Collectible* current = nullptr;
		//GAME_PARALLEL_FOR
		for SEACH_IF(item, items, ITEM_COUNT, item.isFree()) {
			current = item.enable()->setZero();
			#ifdef MAKAILIB_DANMAKU_PREVENT_INSTANCE_OVERFLOW_BY_WRAP
			pbobw = 0;
			#endif
			break;
		} END_SEACH
		if (!current)
		#ifndef MAKAILIB_DANMAKU_PREVENT_INSTANCE_OVERFLOW_BY_WRAP
			throw OutOfObjects(
				getName()
				+ ": Out of usable items! ("
				+ std::to_string(ITEM_COUNT)
				+ ")!"
			);
		#else
			last = items[pbobw++].enable()->setZero();
		#endif
		else last = current;
		last->pause = Pause();
		last->params = CollectibleData();
		last->taskers.clearTaskers();
		last->flags.clear();
		last->clearSignals();
		last->updateSprite();
		return last;
	}

	Collectible* createCollectible(CollectibleData item) {
		Collectible* c = createCollectible();
		c->params = item;
		return c->reset();
	}

	CollectibleList createCollectible(
		CollectibleData item,
		size_t count,
		Vector2 at,
		float radius = 1,
		Vector2 scale = Vector2(1),
		float angleOffset = 0,
		float spread = TAU,
		Vector2 frame = Vector2(0)
	) {
		CollectibleList res;
		GAME_PARALLEL_FOR
		for SSRANGE(i, 0, count) {
			res.push_back(createCollectible(item));
			last->local.position = at + (VecMath::angleV2(spread * ((float)i / (float)count) + angleOffset) * radius);
			last->local.scale = scale;
			last->sprite->frame = frame;
			last->reset();
			last->updateSprite();
		}
		return res;
	}

	Collectible* items = new Collectible[ITEM_COUNT];

	bool haltProcedure = false;

private:
	bool created = false;
	#ifdef MAKAILIB_DANMAKU_PREVENT_INSTANCE_OVERFLOW_BY_WRAP
	size_t pbobw = 0;
	#endif
	Collectible* last = nullptr;
};

#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
typedef CollectibleManager<COLLECTIBLE_ITEM_COUNT, PLAYER_LAYER> ItemManager;

ItemManager* itemManager = nullptr;

#define DANMAKU_IM GameData::Danmaku::itemManager
#else
typedef CollectibleManager<COLLECTIBLE_ITEM_COUNT, PLAYER1_LAYER> Item1Manager;
typedef CollectibleManager<COLLECTIBLE_ITEM_COUNT, PLAYER2_LAYER> Item2Manager;

Item1Manager* item1Manager = nullptr;
Item2Manager* item2Manager = nullptr;

#define DANMAKU_IM1 GameData::Danmaku::item1Manager
#define DANMAKU_IM2 GameData::Danmaku::item2Manager
#endif // MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
