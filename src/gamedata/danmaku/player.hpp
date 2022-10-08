#ifndef MAKAI_BASE_PLAYER_H
#define MAKAI_BASE_PLAYER_H

template <
	template <
		size_t = 64,
		size_t = $layer(PLAYER_BULLET),
		size_t = $layer(ENEMY),
		class = Bullet
	>
	class BM_TYPE = BulletManager
>
struct Familiar2D: Entity2D {
	DERIVED_CLASS(Familiar2D, Entity2D)
	DERIVED_CONSTRUCTOR(Familiar2D, Entity2D, {

	})

	$ref AnimatedPlane* sprite;

	virtual void onFrame(float delta) {

	}
};

// Can this be classified as lunacy?
template <
	template <
		size_t = 64,
		size_t = $layer(PLAYER_BULLET),
		size_t = $layer(ENEMY),
		class = Bullet
	>
	class BM_TYPE = BulletManager,
	class OPTION_TYPE = Familiar2D<BM_TYPE>,
	class = $isderivedof(OPTION_TYPE, Familiar2D<BM_TYPE>)
>
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
		actionKeys["item"]	= SDL_SCANCODE_C;
		actionKeys["extra"]	= SDL_SCANCODE_SPACE;
		actionKeys["skip"]	= SDL_SCANCODE_LCTRL;
		// Create sprite
		sprite = mesh.createReference<Reference::AnimatedPlane>();
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
		deathbomb.onSignal = $signal {
			$debug("Hit!");
			moveTween.reinterpolate(spawnPoint);
			setInvincible(90);
		};
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
	})
	KeyBinds actionKeys;

	Renderable mesh;
	$ref AnimatedPlane* sprite;
	Makai::InputManager input;

	Vector2 spawnPoint = Vector2(0, 0);

	struct {
		float focused = 7;
		float unfocused = 14;
	} speed;

	bool action(std::string what, bool justPressed = false) {
		if (justPressed)
			return input.ifButtonJustPressed(actionKeys[what]);
		else
			return input.getButtonDown(actionKeys[what]);
	}

	bool isFocused	= false;
	bool canBomb	= true;

	virtual void onFocus()		{}
	virtual void onShot()		{}
	virtual void onBomb()		{}
	virtual void onDeathBomb()	{}
	virtual void onItem()		{}
	virtual void onExtra()		{}

	virtual void onDeath()		{}

	$evt Timer invincibility;
	$evt Timer animator;
	$evt Timer deathbomb;
	$evt Timer bombCooldown;

	void pichun() {
		if (!collision.enabled) return;
		collision.enabled = false;
		sprite->setColor(Color::GRAY);
		deathbomb.start();
	}

	void spawnPlayer(Vector2 from) {
		moveTween.reinterpolate(from, spawnPoint);
	}

	bool flipX = true;
	bool flipY = true;

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
		// Flip if necessary
		if (flipX) direction.x *= -1;
		if (flipY) direction.y *= -1;
		// Move acoording to focus state
		if (isFocused)
			position -= direction * speed.focused * delta;
		else
			position -= direction * speed.unfocused * delta;
		// Update sprite
		updateSprite();
		// Do extra actions
		if(isFocused)		onFocus();
		if(action("shot"))	onShot();
		// If can bomb and bombing...
		if(action("bomb") && canBomb) {
			// Disable bombing
			canBomb = false;
			// Set off bomb timer
			bombCooldown.start();
			// Set off invincibility timer
			setInvincible(bombCooldown.delay);
			// Do normal bomb or deathbomb, acoordingly
			if (deathbomb.paused) {
				$debug("Normal Bomb!");
				onBomb();
			} else {
				$debug("Death Bomb!");
				deathbomb.paused = true;
				onDeathBomb();
			}
		};
		if(action("item"))	onItem();
		if(action("extra"))	onExtra();
	}

	virtual void onCollision(Entity* target) {
		if ($ecl groups.hasEntity(target, $layer(ENEMY_BULLET))) {
			onDeath();
			pichun();
		}
	}

	void setInvincible(size_t time) {
		collision.enabled = false;
		sprite->setColor(Color::GRAY);
		invincibility.start(time);
	}

private:
	void updateSprite() {
		Transform2D transform = globalTransform();
		sprite->local.position		= Vector3(transform.position, zIndex);
		sprite->local.rotation.z	= transform.rotation;
		sprite->local.scale			= Vector3(transform.scale, 0);
	}

	Tween::Tween<Vector2> moveTween;
};

#endif // MAKAI_BASE_PLAYER_H
