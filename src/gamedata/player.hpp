#ifndef MAKAI_BASE_PLAYER_H
#define MAKAI_BASE_PLAYER_H

struct PlayerEntity2D: AreaCircle2D {
	DERIVED_CLASS(PlayerEntity2D, AreaCircle2D)
	DERIVED_CONSTRUCTOR(PlayerEntity2D, AreaCircle2D, {
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
		sprite = mesh.createReference<Reference::AnimatedPlane>();
		$ecl $_ROOT += this;
		$ecl groups.addEntity(this, $layer(PLAYER));
		moveTween.tweenStep = Tween::ease.out.cubic;
		moveTween.setTarget(&position);
		invincibility.paused = true;
		invincibility.onSignal = $signal{
			this->collision.enabled = true;
			sprite->setColor(Color::WHITE);
		};
		animator.repeat = true;
		animator.onSignal = $signal {
			if(++sprite->sprite.x >= sprite->size.x)
				sprite->sprite.x = 0;
		};
		invincibility.delay = 60;
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

	bool action(std::string what) {
		return input.getButtonDown(actionKeys[what]);
	}

	virtual void onShot()	{}
	virtual void onDeath()	{}
	virtual void onBomb()	{}
	virtual void onItem()	{}
	virtual void onExtra()	{}

	Event::Timer invincibility;
	Event::Timer animator;

	void pichun() {
		if (!collision.enabled) return;
		collision.enabled = false;
		$debug("Hit!");
		sprite->setColor(Color::GRAY);
		moveTween.reinterpolate(spawnPoint);
		invincibility.reset();
		invincibility.paused = false;
	}

	void spawnPlayer(Vector2 from) {
		moveTween.reinterpolate(from, spawnPoint);
	}

	bool flipX = true;
	bool flipY = true;

	virtual void onFrame(float delta) {
		input.update();
		AreaCircle2D::onFrame(delta);
		Vector2 direction(0);
		direction.y =(
			input.getButtonDown(actionKeys["up"])
			- input.getButtonDown(actionKeys["down"])
		);
		direction.x =(
			input.getButtonDown(actionKeys["right"])
			- input.getButtonDown(actionKeys["left"])
		);
		if (flipX) direction.x *= -1;
		if (flipY) direction.y *= -1;
		if (action("focus"))
			position -= direction * speed.focused * delta;
		else
			position -= direction * speed.unfocused * delta;
		Transform2D transform = globalTransform();
		sprite->local.position		= Vector3(transform.position, zIndex);
		sprite->local.rotation.z	= transform.rotation;
		sprite->local.scale			= Vector3(transform.scale, 0.00001);

		if(action("shot"))	onShot();
		if(action("bomb"))	onBomb();
		if(action("item"))	onItem();
		if(action("extra"))	onExtra();
	}

	virtual void onCollision(Entity* target) {
		if ($ecl groups.hasEntity(target, $layer(ENEMY_BULLET))) {
			onDeath();
			pichun();
		}
	}

private:
	Tween::Tween<Vector2> moveTween;
};

#endif // MAKAI_BASE_PLAYER_H
