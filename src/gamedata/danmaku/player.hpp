#ifndef MAKAI_BASE_PLAYER_H
#define MAKAI_BASE_PLAYER_H

struct PlayerData {
	size_t	score	= 0;
	size_t	power	= 0;
	size_t	point	= 0;
	size_t	graze	= 0;
	size_t	life	= 0;
	size_t	bomb	= 0;
	size_t	extra	= 0;
	size_t	lifeBit	= 0;
	size_t	bombBit	= 0;
};

struct Familiar2D: Entity2D {
	DERIVED_CLASS(Familiar2D, Entity2D)
	DERIVED_CONSTRUCTOR(Familiar2D, Entity2D, {

	})

	RenderData::Reference::AnimatedPlane* sprite;

	virtual void onFrame(float delta) {
		updateSprite();
	}

	virtual Bullet* onShotRequest(bool focused) {
		return nullptr;
	}

	Bullet* fireShot(bool focused) {
		return onShotRequest(focused);
	}

private:
	void updateSprite() {
		Transform2D self = globalTransform();
		sprite->local.position		= Vector3(self.position, zIndex);
		sprite->local.rotation.z	= self.rotation;
		sprite->local.scale			= Vector3(self.scale, zScale);
	}
};

struct PlayerEntity2D: AreaCircle2D {
	DERIVED_CLASS(PlayerEntity2D, AreaCircle2D)
	DERIVED_CONSTRUCTOR(PlayerEntity2D, AreaCircle2D, {
		// Set acton keys
		actionKeys["up"]	= SDL_SCANCODE_UP;
		actionKeys["down"]	= SDL_SCANCODE_DOWN;
		actionKeys["left"]	= SDL_SCANCODE_LEFT;
		actionKeys["right"]	= SDL_SCANCODE_RIGHT;
		actionKeys["focus"]	= SDL_SCANCODE_LSHIFT;
		actionKeys["shot"]	= SDL_SCANCODE_Z;
		actionKeys["bomb"]	= SDL_SCANCODE_X;
		actionKeys["sub"]	= SDL_SCANCODE_C;
		actionKeys["extra"]	= SDL_SCANCODE_SPACE;
		actionKeys["skip"]	= SDL_SCANCODE_LCTRL;
		// Create sprite
		sprite = mesh.createReference<Reference::AnimatedPlane>();
		setAppropriateStartingLayer();
		removeFromCollisionLayer(0);
		// Create hitbox sprite
		hitboxSprite = hitboxMesh.createReference<Reference::AnimatedPlane>();
		hitboxMesh.setRenderLayer(PLAYER_HITBOX_LAYER);
		// Create grazebox sprite
		grazeboxSprite = grazeboxMesh.createReference<Reference::AnimatedPlane>();
		grazeboxMesh.setRenderLayer(PLAYER_HITBOX_LAYER);
		// Add to game
		DEBUGLN("< FINGERS IN HIS ASS SUNDAY >");
		addToGame(this, "DanmakuGame");
		// Invincibility timer
		invincibility.pause().onSignal = SIGNAL {
			this->collision.enabled = true;
			sprite->setColor(Color::WHITE);
		};
		invincibility.delay = 90;
		// Animation timer
		animator.repeat = true;
		animator.onSignal = SIGNAL {
			if((++sprite->frame.x) >= sprite->size.x)
				sprite->frame.x = 0;
		};
		// Death bomb timer
		deathbomb.stop().onSignal = SIGNAL {DEBUGLN("Hit!"); onDeath();};
		deathbomb.delay = 5;
		// Bomb cooldown timer
		bombCooldown.stop().onSignal = SIGNAL {canBomb = true;};
		bombCooldown.delay = 300;
		// Shot cooldown timer
		shotCooldown.stop().onSignal = SIGNAL {canShoot = true;};
		shotCooldown.delay = 300;
		// Movement tween
		moveTween.tweenStep = Tween::ease.out.cubic;
		moveTween.setTarget(&position).setStepCount(30).conclude();
		// Main shot
		mainShot.pause().repeat = true;
		mainShot.onSignal	= SIGNAL {DEBUGLN("Main Shot!"); onShotRequest();};
		mainShot.delay = 1;
		// Option shot
		optionShot.pause().repeat = true;
		optionShot.onSignal	= SIGNAL {DEBUGLN("Option Shot!"); onOptionShotRequest();};
		optionShot.delay = 20;
		// Hitbox size
		collision.shape.radius = 0.1;
		// Set self as main player
		setAsMainPlayer();
		// Set unshaded
		mesh.material.shaded = false;
		// Set timers & tweens manual
		invincibility.setManual();
		animator.setManual();
		deathbomb.setManual();
		bombCooldown.setManual();
		shotCooldown.setManual();
		mainShot.setManual();
		optionShot.setManual();
		moveTween.setManual();
	})

	virtual ~PlayerEntity2D() {
		DEBUGLN("Deleting player...");
	}

	KeyBinds actionKeys;

	Renderable mesh;
	RenderData::Reference::AnimatedPlane*	sprite;

	Renderable hitboxMesh;
	RenderData::Reference::AnimatedPlane* hitboxSprite;

	Renderable grazeboxMesh;
	RenderData::Reference::AnimatedPlane* grazeboxSprite;

	Makai::InputManager	input;

	struct {
		float bullet	= 3.0;
		float laser		= 1.0;
		float bomb		= 10.0;
	} damage;

	Vector2 spawnPoint	= Vector2(0, 0);

	struct {
		float focused	= 8;
		float unfocused	= 16;
	} speed;

	bool flipX	= false;
	bool flipY	= false;

	PlayerData data;

	CircleBounds2D grazebox;

	Vector4 invincibleColor = INVINCIBLE_COLOR;

	BoxBounds2D board;

	bool isFocused	= false;
	bool inDialog	= false;

	bool canBomb		= true;
	bool canShoot		= true;
	bool focusShooting	= false;

	Event::Timer invincibility;
	Event::Timer animator;
	Event::Timer deathbomb;
	Event::Timer bombCooldown;
	Event::Timer shotCooldown;
	Event::Timer mainShot;
	Event::Timer optionShot;

	Vector2 movement = Vector2(0);

	virtual void onCreate() override {}
	virtual void onDelete() override {}

	virtual void onEnteringFocus()	{DEBUGLN("Focus Enter!");}
	virtual void onExitingFocus()	{DEBUGLN("Focus Exit!");}

	virtual void onShotRequest()		{
		if (playerBulletManager) {
			auto b = playerBulletManager->createBullet();
			b->local.position = globalPosition() + Vector2(0, 1);
			b->params.hitbox.radius = 1;
			b->params.vel.start = 128;
			b->params.rot.start = -HPI;
			b->reset();
		}
	}
	virtual void onOptionShotRequest()	{}

	virtual void onBomb()	{
		setInvincible(bombCooldown.delay);
		disableBomb();
		disableShot();
	}
	virtual void onDeathBomb()	{
		deathbomb.stop();
		onBomb();
	}

	virtual void onSubAction()		{}
	virtual void onExtraAction()	{}

	virtual void onPreDeath()	{}
	virtual void onDeath()		{
		moveTween.reinterpolateTo(spawnPoint);
		setInvincible(90);
	}

	virtual void onGraze(size_t count, BulletList list) {
		DEBUGLN("Bullet Graze!");
		data.graze += count;
	}
	virtual void onGraze(size_t count, LineLaserList list) {
		DEBUGLN("Laser Graze!");
		data.graze += count;
	}

	virtual void onItemGet(size_t type, size_t quantity) {
		DEBUG("{ Item Type: ");
		DEBUG(type);
		DEBUG(" (");
		DEBUG(quantity);
		DEBUGLN(") }");
		switch (type) {
			case ItemType::POWER_ITEM:		data.power		+= quantity; break;
			case ItemType::POINT_ITEM:		data.point		+= quantity; break;
			case ItemType::EXTRA_ITEM:		data.extra		+= quantity; break;
			case ItemType::LIFE_ITEM:		data.life		+= quantity; break;
			case ItemType::LIFE_BIT_ITEM:	data.lifeBit	+= quantity; break;
			case ItemType::BOMB_ITEM:		data.bomb		+= quantity; break;
			case ItemType::BOMB_BIT_ITEM:	data.bombBit	+= quantity; break;
		}
	}

	bool action(std::string what, bool justPressed = false) {
		if (justPressed)
			return input.isButtonJustPressed(actionKeys[what]);
		else
			return input.isButtonDown(actionKeys[what]);
	}

	size_t actionState(std::string what) {
		return input.getButtonState(actionKeys[what]);
	}

	void pichun() {
		if (!collision.enabled) return;
		collision.enabled = false;
		sprite->setColor(invincibleColor);
		deathbomb.start();
		onPreDeath();
	}

	void setObjectLayer(size_t layer) {
		addToGroup(layer);
		addToCollisionLayer(layer);
		mesh.setRenderLayer(layer);
	}

	void spawnPlayer(Vector2 from) {
		moveTween.reinterpolate(from, spawnPoint);
	}

	void spawnPlayer(Vector2 from, Vector2 to) {
		moveTween.reinterpolate(from, to);
	}

	virtual void onFrame(float delta) {
		// Process timers & tweens
		invincibility.yield();
		animator.yield();
		deathbomb.yield();
		bombCooldown.yield();
		shotCooldown.yield();
		mainShot.yield();
		optionShot.yield();
		moveTween.yield();
		// Updat inputs
		input.update();
		// Get focus state
		isFocused = action("focus") || (focusShooting && action("shot"));
		// Do parent class frame
		AreaCircle2D::onFrame(delta);
		// Get direction
		Vector2 direction(0);
		direction.y = (
			action("up")
			- action("down")
		);
		direction.x = (
			action("right")
			- action("left")
		);
		// Normalize direction
		direction = direction.normalized();
		// Flip if necessary
		if (!flipX) direction.x *= -1;
		if (!flipY) direction.y *= -1;
		// Save direction
		movement = direction;
		// Move acoording to focus state
		if (isFocused)
			position -= direction * speed.focused * delta;
		else
			position -= direction * speed.unfocused * delta;
		// Clamp position
		position = CollisionData::getBounded(position, board);
		// Set collision's position
		collision.shape.position = globalPosition();
		// Update sprite
		updateSprite();
		grazeboxMesh.trans.position	=
		hitboxMesh.trans.position	= Vec3(position, zIndex);
		// Do focus entering & exiting action, acoordingly
		if(action("focus", true))
			onEnteringFocus();
		else if(!isFocused && lbs.focus) {
			onExitingFocus();
		}
		lbs.focus = isFocused;
		// Set grazebox visibility
		grazeboxMesh.trans.scale = Math::lerp(
			grazeboxMesh.trans.scale,
			Vec3(isFocused * grazebox.radius),
			Vec3(0.25f)
		);
		grazeboxSprite->local.rotation.z += 0.025;
		// Set hitbox visibility
		hitboxMesh.trans.scale = Math::max(0.4f, float(collision.shape.radius * SQRT2 * 2.0));
		hitboxMesh.active = isFocused;
		// Do graze action
		CircleBounds2D grazeShape = getGrazeBounds();
		if(enemyBulletManager) {
			BulletList blist;
			enemyBulletManager->forEachInArea(
				grazeShape,
				[&] (Bullet& b) {
					if(!b.grazed) {
						b.grazed = true;
						blist.push_back(&b);
					}
				}
			);
			if (!blist.empty())
				onGraze(blist.size(), blist);
		}
		if(enemyLineLaserManager) {
			LineLaserList lllist = enemyLineLaserManager->getInArea(grazeShape);
			if (!lllist.empty())
				onGraze(lllist.size(), lllist);
		}
		auto elist = EntityClass::groups.getGroup(ENEMY_LAYER);
		if (elist.size()) {
			for EACH_IF(
				enemy,
				elist,
				CollisionData::isColliding(
					((AreaCircle2D*)enemy)->collision,
					collision
				)
			)
				((AreaCircle2D*)enemy)->onCollision(this);
		}
		// Do shot actions
		optionShot.paused	=
		mainShot.paused		= (inDialog) || !(canShoot && action("shot"));
		// If can bomb and bombing...
		if((!inDialog) && canBomb && action("bomb", true)) {
			// Do normal bomb or deathbomb, acoordingly
			if (deathbomb.paused) {
				DEBUGLN("Normal bomb!");
				onBomb();
			} else {
				DEBUGLN("Deathbomb!");
				onDeathBomb();
			}
		}
		// Do extra actions
		if(action("sub"))	onSubAction();
		if(action("extra"))	onExtraAction();
	}

	virtual void onCollision(Entity* target) {
		if (
			target->isInGroup(ENEMY_LAYER) ||
			target->isInGroup(ENEMY_BULLET_LAYER) ||
			target->isInGroup(ENEMY_LASER_LAYER) ||
			EntityClass::collisionLayers.isInGroup(target, ENEMY_LAYER) ||
			EntityClass::collisionLayers.isInGroup(target, ENEMY_BULLET_LAYER) ||
			EntityClass::collisionLayers.isInGroup(target, ENEMY_LASER_LAYER)
		) {
			pichun();
		}
	}

	void setInvincible() {
		collision.enabled = false;
		sprite->setColor(invincibleColor);
		invincibility.start();
	}

	void setInvincible(size_t time) {
		collision.enabled = false;
		sprite->setColor(invincibleColor);
		invincibility.start(time);
	}

	void disableBomb() {
		canBomb = false;
		bombCooldown.start();
	}

	void disableBomb(size_t time) {
		canBomb = false;
		bombCooldown.start(time);
	}

	void disableShot() {
		canShoot = false;
		shotCooldown.start();
	}

	void disableShot(size_t time) {
		canShoot = false;
		shotCooldown.start(time);
	}

	void enableBomb() {
		canBomb = true;
		bombCooldown.stop().reset();
	}

	void enableShot() {
		canShoot = true;
		shotCooldown.stop().reset();
	}

	inline CircleBounds2D getGrazeBounds() {
		return CircleBounds2D {
			grazebox.position + globalPosition(),
			grazebox.radius
		};
	}

	inline void setAsMainPlayer() {
		mainPlayer = this;
	}

	inline Tween::Tween<Vector2>& getPlayerTween() {
		return moveTween;
	}

private:
	void setAppropriateStartingLayer() {
		#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
		setObjectLayer(PLAYER_LAYER);
		#else
		setObjectLayer(PLAYER1_LAYER);
		#endif
	}

	// Last Button State
	struct {
		bool focus = false;
	} lbs;

	void updateSprite() {
		Transform2D self = globalTransform();
		sprite->local.position		= Vector3(self.position, zIndex);
		// For some reason the sprite is at a fucking angle sometimes
		sprite->local.rotation.z	= self.rotation;
		sprite->local.scale			= Vector3(self.scale, 0);
	}

	Tween::Tween<Vector2> moveTween;
};

PlayerEntity2D* getMainPlayer() {
	if (mainPlayer)
		return (PlayerEntity2D*)mainPlayer;
	else
		return nullptr;
}

#endif // MAKAI_BASE_PLAYER_H
