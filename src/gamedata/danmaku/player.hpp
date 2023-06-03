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

	$ref AnimatedPlane* sprite;

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
		mesh.setRenderLayer($layer(PLAYER));
		// Create hitbox sprite
		hitboxSprite = hitboxMesh.createReference<Reference::AnimatedPlane>();
		hitboxMesh.setRenderLayer($layer(PLAYER_HITBOX));
		// Add to game
		$debug("< FINGERS IN HIS ASS SUNDAY >");
		addToGame(this, "DanmakuGame");
		$ecl groups.addObject(this, $layer(PLAYER));
		removeFromCollisionLayer(0);
		addToCollisionLayer($layer(PLAYER));
		// Invincibility timer
		invincibility.stop().onSignal = $signal{
			this->collision.enabled = true;
			sprite->setColor(Color::WHITE);
		};
		invincibility.delay = 90;
		// Animation timer
		animator.repeat = true;
		animator.onSignal = $signal {
			if((++sprite->frame.x) >= sprite->size.x)
				sprite->frame.x = 0;
		};
		// Death bomb timer
		deathbomb.stop().onSignal = $signal {$debug("Hit!"); onDeath();};
		deathbomb.delay = 5;
		// Bomb cooldown timer
		bombCooldown.stop().onSignal = $signal {canBomb = true;};
		bombCooldown.delay = 300;
		// Shot cooldown timer
		shotCooldown.stop().onSignal = $signal {canShoot = true;};
		shotCooldown.delay = 300;
		// Movement tween
		moveTween.tweenStep = Tween::ease.out.cubic;
		moveTween.setTarget(&position).setStepCount(30).conclude();
		// Main shot
		mainShot.stop().repeat = true;
		mainShot.onSignal	= $signal {$debug("Main Shot!"); onShotRequest();};
		mainShot.delay = 1;
		// Option shot
		optionShot.stop().repeat = true;
		optionShot.onSignal	= $signal {$debug("Option Shot!"); onOptionShotRequest();};
		optionShot.delay = 20;
		// Hitbox size
		collision.shape.radius = 0.1;
		// Set self as main player
		setAsMainPlayer();
		// Set unshaded
		mesh.material.shaded = false;
	})

	KeyBinds actionKeys;

	Renderable mesh;
	$ref AnimatedPlane*	sprite;

	Renderable hitboxMesh;
	$ref AnimatedPlane* hitboxSprite;

	$mki InputManager	input;

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

	bool flipX	= true;
	bool flipY	= true;

	PlayerData data;

	CircleBounds2D grazebox;

	Vector4 invincibleColor = INVINCIBLE_COLOR;

	BoxBounds2D board;

	bool isFocused	= false;
	bool canBomb	= true;
	bool canShoot	= true;

	$evt Timer invincibility;
	$evt Timer animator;
	$evt Timer deathbomb;
	$evt Timer bombCooldown;
	$evt Timer shotCooldown;
	$evt Timer mainShot;
	$evt Timer optionShot;

	Vector2 movement = Vector2(0);

	virtual void onCreate() override {}

	virtual void onEnteringFocus()	{$debug("Focus Enter!");}
	virtual void onExitingFocus()	{$debug("Focus Exit!");}

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
		moveTween.reinterpolate(spawnPoint);
		setInvincible(90);
	}

	virtual void onGraze(size_t count, BulletList list) {
		data.graze += count;
	}
	virtual void onGraze(size_t count, LineLaserList list) {
		data.graze += count;
	}

	virtual void onItemGet(size_t type, size_t quantity) {
		switch (type) {
			case $item(POWER):		data.power		+= quantity; break;
			case $item(POINT):		data.point		+= quantity; break;
			case $item(EXTRA):		data.extra		+= quantity; break;
			case $item(LIFE):		data.life		+= quantity; break;
			case $item(LIFE_BIT):	data.lifeBit	+= quantity; break;
			case $item(BOMB):		data.bomb		+= quantity; break;
			case $item(BOMB_BIT):	data.bombBit	+= quantity; break;
		}
	}

	bool action(std::string what, bool justPressed = false) {
		if (justPressed)
			return input.isButtonJustPressed(actionKeys[what]);
		else
			return input.getButtonDown(actionKeys[what]);
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

	void spawnPlayer(Vector2 from) {
		moveTween.reinterpolate(from, spawnPoint);
	}

	void spawnPlayer(Vector2 from, Vector2 to) {
		moveTween.reinterpolate(from, to);
	}

	virtual void onFrame(float delta) {
		// Updat inputs
		input.update();
		// Get focus state
		isFocused = action("focus");
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
		if (flipX) direction.x *= -1;
		if (flipY) direction.y *= -1;
		// Save direction
		movement = direction;
		// Move acoording to focus state
		if (isFocused)
			position -= direction * speed.focused * delta;
		else
			position -= direction * speed.unfocused * delta;
		// Clamp position
		position = $cdt getBounded(position, board);
		// Set collision's position
		collision.shape.position = globalPosition();
		// Update sprite
		updateSprite();
		hitboxMesh.trans.position = Vec3(position, zIndex);
		// Do focus entering & exiting action, acoordingly
		if(action("focus", true))
			onEnteringFocus();
		else if(!isFocused && lbs.focus) {
			onExitingFocus();
		}
		lbs.focus = isFocused;
		// Set hitbox visibility
		hitboxMesh.trans.scale = Math::lerp(
			hitboxMesh.trans.scale,
			Vec3(isFocused * grazebox.radius),
			Vec3(0.25f)
		);
		hitboxSprite->local.rotation.z += 0.025;
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
		auto elist = $ecl groups.getGroup($layer(ENEMY));
		if (elist.size()) {
			for $eachif(
				enemy,
				elist,
				$cdt isColliding(
					((AreaCircle2D*)enemy)->collision,
					collision
				)
			)
				((AreaCircle2D*)enemy)->onCollision(this);
		}
		// Do shot actions
		optionShot.paused =
		mainShot.paused = !(action("shot") && canShoot);
		// If can bomb and bombing...
		if(action("bomb", true) && canBomb) {
			// Do normal bomb or deathbomb, acoordingly
			if (deathbomb.paused) {
				$debug("Normal bomb!");
				onBomb();
			} else {
				$debug("Deathbomb!");
				onDeathBomb();
			}
		}
		// Do extra actions
		if(action("sub"))	onSubAction();
		if(action("extra"))	onExtraAction();
	}

	virtual void onDelete() {
		$debug("Deleting player...");
	}

	virtual void onCollision(Entity* target) {
		if (
			target->isInGroup($layer(ENEMY)) ||
			target->isInGroup($layer(ENEMY_BULLET)) ||
			target->isInGroup($layer(ENEMY_LASER)) ||
			$ecl collisionLayers.isInGroup(target, $layer(ENEMY)) ||
			$ecl collisionLayers.isInGroup(target, $layer(ENEMY_BULLET)) ||
			$ecl collisionLayers.isInGroup(target, $layer(ENEMY_LASER))
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

	void captureAndPauseAllTimers() {
		if (!snapshot) {
			prevTimerState[0] = invincibility.paused;
			prevTimerState[1] = animator.paused;
			prevTimerState[2] = deathbomb.paused;
			prevTimerState[3] = bombCooldown.paused;
			prevTimerState[4] = shotCooldown.paused;
			prevTimerState[5] = mainShot.paused;
			prevTimerState[6] = optionShot.paused;
			prevTimerState[7] = moveTween.paused;
			snapshot = true;
		}
		invincibility.paused	= true;
		animator.paused			= true;
		deathbomb.paused		= true;
		bombCooldown.paused		= true;
		shotCooldown.paused		= true;
		mainShot.paused			= true;
		optionShot.paused		= true;
		moveTween.paused		= true;
	}

	void rollbackAllTimers() {
		if (!snapshot) return;
		snapshot = false;
		invincibility.paused	= prevTimerState[0];
		animator.paused			= prevTimerState[1];
		deathbomb.paused		= prevTimerState[2];
		bombCooldown.paused		= prevTimerState[3];
		shotCooldown.paused		= prevTimerState[4];
		mainShot.paused			= prevTimerState[5];
		optionShot.paused		= prevTimerState[6];
		moveTween.paused		= prevTimerState[7];
	}
private:
	bool snapshot = false;
	bool prevTimerState[8] = {
		false,
		false,
		false,
		false,
		false,
		false,
		false
	};
	// Last Button State
	struct {
		bool focus = false;
	} lbs;

	void updateSprite() {
		Transform2D self = globalTransform();
		sprite->local.position		= Vector3(self.position, zIndex);
		// For some reason the sprite is at a fucking angle sometimes
		//sprite->local.rotation.z	= self.rotation;
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
