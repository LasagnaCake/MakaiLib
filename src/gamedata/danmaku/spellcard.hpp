struct SpellCard: Entity {
	DERIVED_CLASS(SpellCard, Entity)

	DERIVED_CONSTRUCTOR(SpellCard, Entity, {
		background.setRenderLayer(SPELL_LAYER);
		effect.setRenderLayer(ENEMY_LAYER - 1);
		boss = (BossEntity2D*)getParent();
		time.setRenderLayer(UI_LAYER);
		spellName.setRenderLayer(UI_LAYER);
	})

	RenderData::Renderable	background;
	RenderData::Renderable	effect;

	RenderData::Text::Label	time;
	RenderData::Text::Label	spellName;

	BossEntity2D* boss;

	virtual void onFrame(float delta) {

	}
};
