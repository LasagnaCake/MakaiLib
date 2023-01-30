namespace Dialog {
	struct ActorPosition {
		Vector2	talking;
		Vector2	rest;
		Vector2	out;
	};

	struct Actor {
		$ref AnimatedPlane*	sprite;
		ActorPosition		position;
	};

	struct ActorData {
		String		name;
		Vector2		frame;
		Vector4		tint = Color::WHITE;
		bool		leaving = false;
		$evt Signal	action = $evt DEF_SIGNAL;
	};

	typedef std::vector<ActorData>	ActorDataList;

	struct Message {
		ActorDataList	actors;
		String			title;
		String			text;
		$twn EaseFunc	easing		= $twn ease.out.cubic;
		size_t			duration	= 600;
		bool			autoplay	= false;
	};

	typedef std::vector<Message>				MessageList;
	typedef std::unordered_map<String, Actor>	ActorList;

	struct Dialog: public Entity {
		DERIVED_CLASS(Dialog, Entity)

		DERIVED_CONSTRUCTOR(Dialog, Entity, {
			addToGame(this, "Dialog");
			autotimer.onSignal = $signal {
				nextMessage();
			};
			isFinished = true;
			autotimer.repeat = true;
			autotimer.stop();
			box.shape.setRenderLayer($layer(DIALOG) - 1);
			box.title.setRenderLayer($layer(DIALOG));
			box.message.setRenderLayer($layer(DIALOG));
		})

		MessageList	messages;
		ActorList	actors;

		void setCurrentMessage(size_t index) {
			current = --index;
			nextMessage();
		}

		void begin() {
			for (auto& [aName, actor]: actors) {
				actor.sprite->local.position = actor.position.out;
				animator[aName].value = &actor.sprite->local.position;
			}
			isFinished = false;
		}

		void end() {
			for (auto& [aName, actor]: actors) {
				auto& anim = animator[aName];
				anim.reinterpolate(actor.position.rest);
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

		struct {
			$rdt Renderable	shape;
			$txt Label		title;
			$txt Label		message;
		} box;

		void nextMessage() {
			autotimer.reset();
			input.update();
			if (isFinished) return;
			if (++current > (messages.size()-1)) {
				end();
				return;
			}
			Message& msg = messages[current];
			for (auto& [actor, _]: actors) {
				auto& anim = animator[actor];
				auto& a = actors[actor];
				anim.tweenStep = msg.easing;
				anim.reinterpolate(a.position.rest, time);
				a.sprite->setColor(Color::GRAY);
			}
			for (auto& actor: msg.actors) {
				auto& anim = animator[actor.name];
				auto& a = actors[actor.name];
				a.sprite->frame = actor.frame;
				anim.reinterpolate(actor.leaving ? a.position.out : a.position.talking, time);
				actor.action();
				a.sprite->setColor(actor.tint);
			}
			autoplay = msg.autoplay;
			autotimer.delay = msg.duration;
			showText(msg.title, msg.text);
		}

		size_t time = 100;
	private:
		void showText(String title, String text) {
			box.title.text.content		= title;
			box.message.text.content	= text;
		}
		$evt Timer autotimer;
		std::unordered_map<String, $twn Tween<Vector3>> animator;
		size_t current	= 0;
		bool autoplay	= false;
		bool isFinished	= false;
	};
}
