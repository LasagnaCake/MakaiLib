struct Stage: Entity {
	DERIVED_CLASS(Stage, Entity)

	DERIVED_CONSTRUCTOR(Stage, Entity, {
		addToGame(this, "DanmakuGame");
	})

	virtual ~Stage() {}

	bool practiceMode = false;

	virtual void onFrame(float delta) {}

	virtual void onPracticeEnd()	= 0;
	virtual void onStageEnd()		= 0;

	void endStage() {
		if (practiceMode)	onPracticeEnd();
		else				onStageEnd();
	}
};
