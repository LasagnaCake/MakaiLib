struct Stage: Entity {
	DERIVED_CLASS(Stage, Entity)

	DERIVED_CONSTRUCTOR(Stage, Entity, {
		addToGame(this, "DanmakuGame");
	})

	virtual void onFrame(float delta) {}
};
