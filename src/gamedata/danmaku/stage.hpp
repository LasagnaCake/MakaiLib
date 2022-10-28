struct Stage: Entity {
	DERIVED_CLASS(Stage, Entity)

	DERIVED_CONSTRUCTOR(Stage, Entity, {
		addToGame(this, "DanmakuGame");
	})

	$rdt Renderable mesh;
};
