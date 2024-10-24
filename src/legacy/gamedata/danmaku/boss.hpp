struct BossPhaseData {
	float			health			= 100;
	size_t			delay			= 60;
	size_t			invincibility	= 60;
	size_t			duration		= 3600;
	bool			spell			= false;
	Event::Signal	action			= Event::DEF_SIGNAL;
};

typedef List<BossPhaseData> BossPhases;

class BossEntity2D: public EnemyEntity2D {
public:
	DERIVED_CLASS(BossEntity2D, EnemyEntity2D)

	DERIVED_CONSTRUCTOR(BossEntity2D, EnemyEntity2D, {
		// Boss phase timer
		phaseTimer.setManual();
		phaseTimer.stop();
		phaseTimer.onSignal = SIGNAL {
			updatePhase();
		};
		// Boss phase duration timer
		durationTimer.setManual();
		durationTimer.stop();
		durationTimer.onSignal = SIGNAL {
			beginNextPhase();
		};
		// Boss mover tween
		bossMover.setManual();
		setUIElements();
		setUILayer(UI_LAYER);
		battling =
		collision.enabled = false;
		timerDisplay.text.rectAlign	=
		timerDisplay.text.textAlign	= .5;
	})

	FontFace
		timerFont,
		phaseFont
	;

	Label
		timerDisplay,
		phaseDisplay
	;

	LinearBar	remainingPhases;

	BossPhases	phases;

	virtual void onDeath() {
		if (!battling) return;
		if (!invincible) beginNextPhase();
	}

	virtual void onBattleBegin() {beginNextPhase();}

	virtual void onPhaseBegin(size_t phase, bool spell)	{}
	virtual void onPhaseEnd(size_t phase, bool spell)	{}

	virtual void onBattleEnd() {}

	virtual void onFrame(float delta) override {
		EnemyEntity2D::onFrame(delta);
		bossMover.yield();
		if (!battling) return;
		phaseTimer.yield();
		durationTimer.yield();
		float tsec = (phaseDuration - durationTimer.getCounter()) / getMainProgram()->maxCycleRate;
		size_t phaseCount = phases.size();
		timerDisplay.text.content = Helper::floatString(tsec, 2);
		remainingPhases.max			= phaseCount;
		remainingPhases.value		= phaseCount - currentPhase;
		phaseDisplay.text.content	= toString(
			Helper::formatNumber(currentPhase, 2),
			"/",
			Helper::formatNumber(phaseCount, 2)
		);
	}

	void beginNextPhase() {
		if (!battling) return;
		timerDisplay.text.content = "00.00";
		durationTimer.stop();
		auto& phase = phases[currentPhase];
		if (currentPhase != 0)
			onPhaseEnd(currentPhase-1, phases[currentPhase-1].spell);
		if (currentPhase >= phases.size()) {
			endBattle();
			return;
		}
		if (!phase.delay) {
			setInvincible(phase.invincibility);
			updatePhase();
		} else {
			maxHealth = ++health;
			setInvincible(phase.delay + phase.invincibility);
			phaseTimer.start(phase.delay);
		}
	}

	void startBattle() {
		if(battling || phases.empty()) return;
		battling =
		collision.enabled = true;
		currentPhase = 0;
		phaseTimer.stop();
		durationTimer.stop();
		onBattleBegin();
	}

	void endBattle() {
		if(!battling) return;
		battling =
		collision.enabled = false;
		currentPhase = Math::Max::SIZET_V;
		phaseTimer.stop();
		durationTimer.stop();
		onBattleEnd();
	}

	void setUIVisibility(bool visible = true) {
		healthBar.active		= visible;
		timerDisplay.active		= visible;
		phaseDisplay.active		= visible;
		remainingPhases.active	= visible;
	}

	/*virtual void setObjectLayer(size_t layer, size_t topLayer) override {
		EnemyEntity2D::setObjectLayer(layer, topLayer);
	}*/

	void moveTo(
		Vector2 const&	pos,
		size_t const&	time,
		Ease::EaseMode	mode		= Ease::linear,
		Event::Signal	onCompleted	= Event::DEF_SIGNAL
	) {
		bossMover.setInterpolation(position, pos, time, mode, &position);
		bossMover.onCompleted = onCompleted;
	}

	void setUILayer(size_t layer) {
		timerDisplay.setRenderLayer(layer);
		phaseDisplay.setRenderLayer(layer);
	}

	bool isInBattle() {return battling;}

	size_t currentPhase = 0;

	size_t getPhaseTimeLeft() {
		return phaseTimer.delay - phaseTimer.getCounter();
	}

	size_t getPhaseDuration() {
		return phaseTimer.delay;
	}

private:
	void setUIElements() {
		timerDisplay.font = timerFont;
		phaseDisplay.font = phaseFont;
		phaseDisplay.text =
		timerDisplay.text = TextData {
			"0",
			TextRect{7, 1},
			Vector2(0.5, 0.5),
			Vector2(0.0, 1.0)
		};
	}

	void updatePhase() {
		if(!battling) return;
		if(phases.empty() || currentPhase >= phases.size()) {
			endBattle();
			return;
		}
		auto& phase = phases[currentPhase];
		maxHealth =
		health = phase.health;
		phase.action();
		phaseDuration = phase.duration;
		durationTimer.start(phase.duration);
		onPhaseBegin(currentPhase, phase.spell);
		++currentPhase;
	}

	float phaseDuration = 0;

	bool battling = false;

	Event::Timer phaseTimer;
	Event::Timer durationTimer;

	Tweening::Tween<Vector2> bossMover;
};
