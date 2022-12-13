struct Enemy2D: AreaCircle2D {
	DERIVED_CLASS(Enemy2D, AreaCircle2D)

	float health = 100.0;

	$rdt Renderable mesh;

	$ref AnimatedPlane* sprite;

	DERIVED_CONSTRUCTOR(Enemy2D, AreaCircle2D, {
		mesh.setRenderLayer($layer(ENEMY));
		$ecl groups.addEntity(this, $layer(ENEMY));
		sprite = mesh.createReference<AnimatedPlane>();
	})

	virtual void onDeath() {
		delete this;
	}

	void onFrame(float delta) override {
		updateSprite();
		if (health <= 0.0)
			onDeath();
	}

	void onCollision(Entity* e) override {
		auto obj = (PlayerEntity2D*)e;
		if ($ecl groups.hasEntity(e, $layer(PLAYER_BULLET)) && mainPlayer) {
			health -= getMainPlayer()->damage;
		}
		if (obj == mainPlayer) {
			obj->pichun();
		}
	}

	void updateSprite() {
		Transform2D self = globalTransform();
		sprite->local.position		= $vec3(self.position, zIndex);
		sprite->local.rotation.z	= self.rotation;
		sprite->local.scale = $vec3(self.scale, zScale);
	}
};
