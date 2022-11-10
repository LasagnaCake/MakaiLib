namespace Dialog {
	struct ActorPosition {
		Vector2	talk;
		Vector2	rest;
		Vector2	out;
	};

	struct Actor {
		$ref AnimatedPlane*	sprite;
		ActorPosition		position;
	};

	struct Message {
		StringList		actors;
		String			title;
		String			text;
		$twn EaseFunc	easing		= $twn ease.out.cubic;
		size_t			duration	= 600;
		bool			autoplay	= false;
		bool			leaving		= false;
	};

	typedef std::vector<Message>	MessageList;
	typedef std::map<String, Actor>	ActorList;

	struct Dialog: public Entity {
		DERIVED_CLASS(Dialog, Entity)
		DERIVED_CONSTRUCTOR(Dialog, Entity, {
			addToGame(this, "Dialog");
			autotimer.onSignal = $signal {
				nextMessage();
			};
			isFinished = true;
		})

		MessageList	messages;
		ActorList	actors;

		void setCurrentMessage(size_t index) {
			current = --index;
			nextMessage();
		}

		void begin() {
			for (auto& a: actors) {
				a.second.sprite->local.position = a.second.position.out;
				animator[a.first].value = &a.second.sprite->local.position;
			}
			isFinished = false;
		}

		void end() {
			for (auto& a: actors) {
				auto& anim = animator[a.first];
				anim.reinterpolate(a.second.position.rest);
			}
			autotimer.stop();
			isFinished = true;
		}

		void onFrame(float delta) override {
			if (isFinished) return;
			if (!autoplay) {
				if (
					input.isButtonJustPressed(keys["next"])
					|| input.isButtonHeld(keys["next"])
				) nextMessage();
			}
		}

		$tev Signal<size_t>	onMessage	= $tsignal(size_t index) {};
		$evt Signal			onDialogEnd	= $signal {queueDestroy();};

		KeyBinds keys;

		$mki InputManager input;

		void nextMessage() {
			autotimer.reset();
			input.update();
			if (isFinished) return;
			if (++current > (messages.size()-1)) {
				end();
				return;
			}
			Message& msg = messages[current];
			String title = "";
			for (auto& actor: msg.actors) {
				auto& anim = animator[actor];
				auto& a = actors[actor];
				anim.tweenStep = msg.easing;
				anim.reinterpolate(msg.leaving ? a.position.out : a.position.rest, time);
			}
			autoplay = msg.autoplay;
			autotimer.delay = msg.duration;
			showText(msg.title, msg.text);
		}

		size_t time = 100;
	private:
		void showText(String title, String text) {
			//TODO: this
		}
		$evt Timer autotimer;
		std::map<String, $twn Tween<Vector3>> animator;
		size_t current	= 0;
		bool autoplay	= false;
		bool isFinished	= false;
	};
}
