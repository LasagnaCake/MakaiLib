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
		EntityClass::$_ROOT += this;
		EntityClass::groups.addEntity(this, $layer(PLAYER));
		moveTween.x.tweenStep = moveTween.y.tweenStep = Tween::ease.out.cubic;
		moveTween.x.setTarget(&position.x);
		moveTween.y.setTarget(&position.y);
		invincibility.paused = true;
		invincibility.onSignal = $signal{
			this->collision.enabled = true;
			sprite->setColor(Color::WHITE);
		};
		invincibility.delay = 60;
		moveTween.x.setStep(60);
		moveTween.y.setStep(60);
	})
	KeyBinds actionKeys;

	Renderable mesh;
	Reference::AnimatedPlane* sprite;
	Makai::InputManager input;

	Vector2 spawnPoint = Vector2(0, 0);

	struct {
		float focused = 4;
		float unfocused = 8;
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

	void pichun() {
		if (!collision.enabled) return;
		collision.enabled = false;
		$debug("Hit!");
		sprite->setColor(Color::GRAY);
		moveTween.x.reinterpolate(spawnPoint.x - position.x, 60);
		moveTween.y.reinterpolate(spawnPoint.y - position.y, 60);
		invincibility.reset();
		invincibility.paused = false;
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
		if(++sprite->sprite.x >= sprite->size.x)
			sprite->sprite.x = 0;

		if(action("shot"))	onShot();
		if(action("bomb"))	onBomb();
		if(action("item"))	onItem();
		if(action("extra"))	onExtra();
	}

	virtual void onCollision(Entity* target) {
		if (EntityClass::groups.hasEntity(target, BULLET_MANAGER_GROUP)) {
			onDeath();
			pichun();
		}
	}

private:
	struct {
		Tween::Tween x;
		Tween::Tween y;
	} moveTween;
};

#endif // MAKAI_BASE_PLAYER_H
