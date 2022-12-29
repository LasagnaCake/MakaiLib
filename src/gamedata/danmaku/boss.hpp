struct BossPhaseData {
	float		health			= 100;
	size_t		delay			= 60;
	size_t		invincibility	= 60;
	$evt Signal	action			= $evt DEF_SIGNAL;
};

struct BossEntity2D: EnemyEntity2D {
	DERIVED_CLASS(BossEntity2D, EnemyEntity2D)
	DERIVED_CONSTRUCTOR(BossEntity2D, EnemyEntity2D, {
		// Boss phase timer
		phaseTimer.stop();
		phaseTimer.onSignal = $signal{
			beginNextPhase();
		};
	})

	std::vector<BossPhaseData> phases;

	virtual void onDeath() {
		if (!phases.size()) queueDestroy();
		else {
			onPhaseEnd(phases.size());
			auto phase = phases.end();
			if (!phase->delay) {
				setInvincible(phase->invincibility);
				beginNextPhase();
			} else {
				setInvincible(phase->delay + phase->invincibility);
				phaseTimer.start(phase->delay);
			}
		}
	}

	virtual void onPhaseBegin(size_t phase)	{}
	virtual void onPhaseEnd(size_t phase)	{}

	void beginNextPhase() {
		if(!phases.size()) return;
		auto phase = phases.end();
		phases.pop_back();
		health = phase->health;
		phase->action();
		onPhaseBegin(phases.size());
	}

private:

	$evt Timer phaseTimer;
};
