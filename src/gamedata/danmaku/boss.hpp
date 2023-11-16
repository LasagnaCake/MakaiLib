struct BossPhaseData {
	float			health			= 100;
	size_t			delay			= 60;
	size_t			invincibility	= 60;
	size_t			duration		= 3600;
	Event::Signal	action			= Event::DEF_SIGNAL;
};

class BossEntity2D: public EnemyEntity2D {
public:
	DERIVED_CLASS(BossEntity2D, EnemyEntity2D)

	DERIVED_CONSTRUCTOR(BossEntity2D, EnemyEntity2D, {
		// Boss phase timer
		phaseTimer.stop();
		phaseTimer.onSignal = SIGNAL {
			updatePhase();
		};
		// Boss phase duration timer
		durationTimer.stop();
		durationTimer.onSignal = SIGNAL {
			beginNextPhase();
		};
	})

	Label		timerDisplay;
	LinearBar	remainingPhases;

	std::vector<BossPhaseData> phases;

	virtual void onDeath() {
		if (!battling) return;
		beginNextPhase();
	}

	virtual void onBattleBegin() {}

	virtual void onPhaseBegin(size_t phase)	{}
	virtual void onPhaseEnd(size_t phase)	{}

	virtual void onBattleEnd() {}

	virtual void onFrame(float delta) override {
		EnemyEntity2D::onFrame(delta);
		float tsec = (phaseDuration - durationTimer.getCounter()) / getMainProgram()->maxCycleRate;
		timerDisplay.text.content = Helper::floatString(tsec, 2);
	}

	void beginNextPhase() {
		timerDisplay.text.content = "00.00";
		auto& phase = phases[currentPhase];
		onPhaseEnd(currentPhase-1);
		if (!phase.delay) {
			setInvincible(phase.invincibility);
			updatePhase();
		} else {
			setInvincible(phase.delay + phase.invincibility);
			phaseTimer.start(phase.delay);
		}
	}

	void startBattle() {
		if(battling || phases.empty()) return;
		battling =
		collision.enabled = true;
		currentPhase = 0;
		onBattleBegin();
	}

	void endBattle() {
		if(!battling) return;
		battling =
		collision.enabled = false;
		currentPhase = Math::Max::SIZET_V;
		onBattleEnd();
	}

	size_t currentPhase = 0;

private:
	void updatePhase() {
		if(!battling) return;
		if(phases.empty() || !(currentPhase < phases.size()))
			endBattle();
		auto& phase = phases[currentPhase];
		health = phase.health;
		phase.action();
		phaseDuration = phase.duration;
		durationTimer.start(phase.duration);
		onPhaseBegin(currentPhase);
		++currentPhase;
	}

	float phaseDuration = 0;

	bool battling = false;

	Event::Timer phaseTimer;
	Event::Timer durationTimer;
};
