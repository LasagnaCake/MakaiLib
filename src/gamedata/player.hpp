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
		sprite = mesh.createReference<Reference::AnimatedPlane>();
	})
	KeyBinds actionKeys;

	Renderable mesh;
	Reference::AnimatedPlane* sprite;
	Makai::InputManager input;

	float zIndex = 1;

	struct {
		float focused = 3;
		float unfocused = 6;
	} speed;

	virtual void onFrame(float delta) {
		input.update();
		AreaCircle2D::onFrame(delta);
		Vector2 direction(0);
		direction.y =(
			input.getButtonDown(actionKeys["down"])
			- input.getButtonDown(actionKeys["up"])
		);
		direction.x =(
			input.getButtonDown(actionKeys["right"])
			- input.getButtonDown(actionKeys["left"])
		);
		if (input.getButtonDown(actionKeys["focus"]))
			position -= direction * speed.focused * delta;
		else
			position -= direction * speed.unfocused * delta;
		Transform2D transform = globalTransform();
		sprite->local.position		= Vector3(transform.position, zIndex);
		sprite->local.rotation.z	= transform.rotation;
		sprite->local.scale			= Vector3(transform.scale, 0.00001);
		if(++sprite->sprite.x >= sprite->size.x)
			sprite->sprite.x = 0;
	}
};

#endif // MAKAI_BASE_PLAYER_H
