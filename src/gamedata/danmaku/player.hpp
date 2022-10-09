#ifndef MAKAI_BASE_PLAYER_H
#define MAKAI_BASE_PLAYER_H

struct Familiar2D: Entity2D {
	DERIVED_CLASS(Familiar2D, Entity2D)
	DERIVED_CONSTRUCTOR(Familiar2D, Entity2D, {

	})

	$ref AnimatedPlane* sprite;

	virtual void onFrame(float delta) {
		updateSprite();
	}

private:
	void updateSprite() {
		Transform2D self = globalTransform();
		sprite->local.position		= Vector3(self.position, zIndex);
		sprite->local.rotation.z	= self.rotation;
		sprite->local.scale			= Vector3(self.scale, 0);
	}
};

// Can this be classified as lunacy?
/*
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
*/
/*
	TODO: Figure out how to deal with bullet damage
*/
template <
	size_t	BULLET_COUNT,
	class	BULLET_TYPE	= Bullet,
	class = $isderivedof(BULLET_TYPE, Bullet)
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
			onDeath();
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
		// Main shot
		mainShot.stop();
		mainShot.repeat = true;
		mainShot.onSignal	= $signal {$debug("Main Shot!"); onShot();};
		mainShot.delay = 5;
		// Option shot
		optionShot.stop();
		optionShot.repeat = true;
		optionShot.onSignal	= $signal {$debug("Option Shot!"); onOptionShot();};
		optionShot.delay = 20;
	})

	KeyBinds actionKeys;

	Renderable mesh;
	$ref AnimatedPlane* sprite;
	$mki InputManager input;
	PlayerBulletManager<BULLET_COUNT, BULLET_TYPE> pbm;

	struct {
		float main;
		float option;
	} damage;

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

	virtual void onFocus()	{}

	virtual void onShot()		{}
	virtual void onOptionShot()	{}

	virtual void onBomb()	{}
	virtual void onItem()	{}
	virtual void onExtra()	{}

	virtual void onDeathBomb()	{}
	virtual void onDeath()		{}

	$evt Timer invincibility;
	$evt Timer animator;
	$evt Timer deathbomb;
	$evt Timer bombCooldown;
	$evt Timer mainShot;
	$evt Timer optionShot;

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
		// Update sprite
		updateSprite();
		// Do extra actions
		if(isFocused)		onFocus();
		optionShot.paused =
		mainShot.paused = !action("shot");
		// If can bomb and bombing...
		if(action("bomb", true) && canBomb) {
			// Disable bombing
			canBomb = false;
			// Set off bomb timer
			bombCooldown.start();
			// Do normal bomb or deathbomb, acoordingly
			if (deathbomb.paused) {
				$debug("Normal bomb!");
				onBomb();
			} else {
				$debug("Deathbomb!");
				onDeathBomb();
			}
		};
		if(action("item"))	onItem();
		if(action("extra"))	onExtra();
	}

	virtual void onCollision(Entity* target) {
		if ($ecl groups.hasEntity(target, $layer(ENEMY_BULLET))) {
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
		Transform2D self = globalTransform();
		sprite->local.position		= Vector3(self.position, zIndex);
		sprite->local.rotation.z	= self.rotation;
		sprite->local.scale			= Vector3(self.scale, 0);
	}

	Tween::Tween<Vector2> moveTween;
};

#endif // MAKAI_BASE_PLAYER_H
