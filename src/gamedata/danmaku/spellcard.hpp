struct SpellCard: Entity {
	DERIVED_CLASS(SpellCard, Entity)

	DERIVED_CONSTRUCTOR(SpellCard, Entity, {
		background.setRenderLayer($layer(SPELL));
		effect.setRenderLayer($layer(ENEMY));
		boss = (BossEntity2D*)&(*getParent());
		time.setRenderLayer($layer(UI));
		spellName.setRenderLayer($layer(UI));
	})

	$rdt Renderable	background;
	$rdt Renderable	effect;

	$txt Label	time;
	$txt Label	spellName;

	Pointer<BossEntity2D> boss;

	virtual void onFrame(float delta) {

	}
};
