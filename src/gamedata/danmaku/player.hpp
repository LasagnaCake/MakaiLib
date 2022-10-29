#ifndef MAKAI_BASE_PLAYER_H
#define MAKAI_BASE_PLAYER_H

struct PlayerData {
	size_t score	= 0;
	size_t power	= 0;
	size_t point	= 0;
	size_t graze	= 0;
	size_t extra	= 0;
	float life		= 0;
	float bomb		= 0;
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
		// Add to game
		addToGame(this, "DanmakuGame");
		$ecl groups.addEntity(this, $layer(PLAYER));
		// Invincibility timer
		invincibility.stop();
		invincibility.onSignal = $signal{
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
		deathbomb.stop();
		deathbomb.onSignal = $signal {$debug("Hit!"); onDeath();};
		deathbomb.delay = 5;
		// Bomb cooldown timer
		bombCooldown.stop();
		bombCooldown.onSignal = $signal {canBomb = true;};
		bombCooldown.delay = 300;
		// Movement tween
		moveTween.tweenStep = Tween::ease.out.cubic;
		moveTween.setTarget(&position);
		moveTween.setStepCount(30);
		moveTween.conclude();
		// Main shot
		mainShot.stop();
		mainShot.repeat = true;
		mainShot.onSignal	= $signal {$debug("Main Shot!"); onShotRequest();};
		mainShot.delay = 5;
		// Option shot
		optionShot.stop();
		optionShot.repeat = true;
		optionShot.onSignal	= $signal {$debug("Option Shot!"); onOptionShotRequest();};
		optionShot.delay = 20;
		// Hitbox size
		collision.size = 0.1;
		// Set self as main player
		setAsMainPlayer();
	})

	KeyBinds actionKeys;

	Renderable mesh;
	$ref AnimatedPlane*	sprite;
	$mki InputManager	input;

	float damage = 3;

	Vector2 spawnPoint	= Vector2(0, 0);

	struct {
		float focused	= 8;
		float unfocused	= 16;
	} speed;

	bool flipX	= true;
	bool flipY	= true;

	PlayerData data;

	CircleBounds2D grazebox;

	BoxBounds2D board;

	bool isFocused	= false;
	bool canBomb	= true;

	$evt Timer invincibility;
	$evt Timer animator;
	$evt Timer deathbomb;
	$evt Timer bombCooldown;
	$evt Timer mainShot;
	$evt Timer optionShot;

	virtual void onEnteringFocus()	{$debug("Focus Enter!");}
	virtual void onExitingFocus()	{$debug("Focus Exit!");}

	virtual void onShotRequest()		{}
	virtual void onOptionShotRequest()	{}

	virtual void onBomb()	{
		setInvincible(bombCooldown.delay);
		disableBomb();
	}
	virtual void onDeathBomb()	{
		deathbomb.stop();
		onBomb();
	}

	virtual void onSubAction()		{}
	virtual void onExtraAction()	{}

	virtual void onDeath()	{
		moveTween.reinterpolate(spawnPoint);
		setInvincible(90);
	}

	virtual void onGraze(size_t count, BulletList list) {
		data.graze += count;
	}
	virtual void onGraze(size_t count, LineLaserList list) {
		data.graze += count;
	}
	virtual void onItemGet(size_t type, float quantity) {
		switch (type) {
			case $item(POWER): data.power	+= quantity; break;
			case $item(POINT): data.point	+= quantity; break;
			case $item(EXTRA): data.extra	+= quantity; break;
			case $item(BOMB): data.bomb		+= quantity; break;
			case $item(LIFE): data.life		+= quantity; break;
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
		sprite->setColor(INVINCIBLE_COLOR);
		deathbomb.start();
	}

	void spawnPlayer(Vector2 from) {
		moveTween.reinterpolate(from, spawnPoint);
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
			input.getButtonDown(actionKeys["up"])
			- input.getButtonDown(actionKeys["down"])
		);
		direction.x = (
			input.getButtonDown(actionKeys["right"])
			- input.getButtonDown(actionKeys["left"])
		);
		// Normalize direction
		direction = direction.normalized();
		// Flip if necessary
		if (flipX) direction.x *= -1;
		if (flipY) direction.y *= -1;
		// Move acoording to focus state
		if (isFocused)
			position -= direction * speed.focused * delta;
		else
			position -= direction * speed.unfocused * delta;
		// Clamp position
		position = $cdt getBounded(position, board);
		// Update sprite
		updateSprite();
		// Do focus entering & exiting action, acoordingly
		if(action("focus", true))
			onEnteringFocus();
		else if(!isFocused && lbs.focus) {
			onExitingFocus();
		}
		lbs.focus = isFocused;
		// Do graze action
		CircleBounds2D grazeShape = getGrazeBounds();
		if(enemyBulletManager) {
			BulletList blist = enemyBulletManager->getInArea(grazeShape);
			if (blist.size()) {
				size_t grazeCount = 0;
				for $eachif(b, blist, !b->grazed) {
					b->grazed = true;
					grazeCount++;
				}
				if (grazeCount)
					onGraze(grazeCount, blist);
			}
		}
		if(enemyLineLaserManager) {
			LineLaserList lllist = enemyLineLaserManager->getInArea(grazeShape);
			if (lllist.size()) {
				onGraze(lllist.size(), lllist);
			}
		}
		// Do shot actions
		optionShot.paused =
		mainShot.paused = !action("shot");
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

	virtual void onCollision(Entity* target) {
		if ($ecl groups.hasEntity(target, $layer(ENEMY_BULLET))) {
			pichun();
		}
	}

	void setInvincible() {
		collision.enabled = false;
		sprite->setColor(INVINCIBLE_COLOR);
		invincibility.start();
	}

	void setInvincible(size_t time) {
		collision.enabled = false;
		sprite->setColor(INVINCIBLE_COLOR);
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
	// Last Button State
	struct {
		bool focus = false;
	} lbs;

	void updateSprite() {
		Transform2D self = globalTransform();
		sprite->local.position		= Vector3(self.position, zIndex);
		// For some reason the sprite is at a fucking angle sometimes
		sprite->local.rotation.z	= self.rotation + 1;
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
