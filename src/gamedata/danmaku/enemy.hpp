struct EnemyEntity2D: public AreaCircle2D {
	DERIVED_CLASS(EnemyEntity2D, AreaCircle2D)

	DERIVED_CONSTRUCTOR(EnemyEntity2D, AreaCircle2D, {
		health = 100.0;
		mesh.setRenderLayer($layer(ENEMY));
		$ecl groups.addEntity(this, $layer(ENEMY));
		sprite = mesh.createReference<AnimatedPlane>();
		collision.shape.radius = 1;
		addToGame(this, "DanmakuGame");
		sprite->setColor(Color::WHITE);
		// Invincibility timer
		invincibility.stop();
		invincibility.onSignal = $signal{
			invincible = false;
		};
		invincibility.delay = 60;
		healthBar.setRenderLayer($layer(OVERLAY));
		healthBar.material.color.w = 0.5;
		healthBar.trans.rotation.z = HPI;
		healthBar.size = 2;
	});

	float& health		= healthBar.value;
	float& maxHealth	= healthBar.max;

	bool invincible = false;

	bool collideWithPlayer = false;

	$rdt Renderable mesh;

	$ref AnimatedPlane* sprite;

	$bar RadialBar healthBar;

	virtual void onDelete() {
		$ecl groups.removeFromAll(this);
	}

	virtual void onDeath() {
		queueDestroy();
	}

	void onFrame(float delta) override {
		updateSprite();
		collision.shape.position = globalPosition();
		health = Math::min(health, maxHealth);
		if (health <= 0.0)
			onDeath();
	}

	void onCollision(Entity* e) override {
		auto player = getMainPlayer();
		if (!invincible) {
			if (
					$ecl groups.hasEntity(e, $layer(PLAYER_BULLET))
				||	$ecl groups.hasEntity(e, $layer(PLAYER_LASER))
			) {
				if (player) health -= player->damage.main;
				else health -= defaults.playerDamage.main;
			}
			if ($ecl groups.hasEntity(e, $layer(PLAYER_BOMB))) {
				if (player) health -= player->damage.bomb;
				else health -= defaults.playerDamage.bomb;
			}
		}
		auto obj = (PlayerEntity2D*)e;
		if (collideWithPlayer && obj == player) {
			obj->pichun();
		}
	}

	void setInvincible() {
		invincible = true;
		invincibility.start();
	}

	void setInvincible(size_t time) {
		invincible = true;
		invincibility.start(time);
	}

	void updateSprite() {
		Transform2D self = globalTransform();
		sprite->local.position		= $vec3(self.position, zIndex);
		sprite->local.scale			= $vec3(self.scale, zScale);
		healthBar.trans.position	= $vec3(self.position, zIndex);
		healthBar.trans.scale		= $vec3(self.scale, zScale);
	}

private:
	$evt Timer invincibility;
};
