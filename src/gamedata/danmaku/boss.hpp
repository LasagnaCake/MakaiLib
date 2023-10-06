struct BossPhaseData {
	float			health			= 100;
	size_t			delay			= 60;
	size_t			invincibility	= 60;
	Event::Signal	action			= Event::DEF_SIGNAL;
};

class BossEntity2D: public EnemyEntity2D {
public:
	DERIVED_CLASS(BossEntity2D, EnemyEntity2D)

	DERIVED_CONSTRUCTOR(BossEntity2D, EnemyEntity2D, {
		// Boss phase timer
		phaseTimer.stop();
		phaseTimer.onSignal = SIGNAL {
			beginNextPhase();
		};
	})

	std::vector<BossPhaseData> phases;

	virtual void onDeath() {
		if (!battling) return;
		if (!phases.size()) endBattle();
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

	virtual void onBattleBegin() {}

	virtual void onPhaseBegin(size_t phase)	{}
	virtual void onPhaseEnd(size_t phase)	{}

	virtual void onBattleEnd() {}

	void beginNextPhase() {
		if(!battling) return;
		if(!phases.size()) endBattle();
		auto phase = phases.end();
		phases.pop_back();
		health = phase->health;
		phase->action();
		onPhaseBegin(phases.size());
	}

	void startBattle() {
		if(battling) return;
		battling =
		collision.enabled = true;
		onBattleBegin();
	}

	void endBattle() {
		if(!battling) return;
		battling =
		collision.enabled = false;
		onBattleEnd();
	}

private:
	bool battling = false;

	Event::Timer phaseTimer;
};
