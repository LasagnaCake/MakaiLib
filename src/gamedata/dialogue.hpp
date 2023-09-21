namespace Dialog {
	struct ActorPosition {
		Vector2	talking;
		Vector2	rest;
		Vector2	out;
	};

	struct Actor {
		ActorPosition	position;
		AnimatedPlane*	sprite = nullptr;
	};

	struct ActorData {
		String		name;
		Vector2		frame;
		Vector4		tint = Color::WHITE;
		bool		leaving = false;
		$evt Signal	action = $evt DEF_SIGNAL;
	};

	typedef List<ActorData>	ActorDataList;

	struct Message {
		ActorDataList	actors;
		String			title;
		String			text;
		$twn EaseFunc	easing		= $twn ease.out.cubic;
		size_t			duration	= 600;
		bool			autoplay	= false;
	};

	typedef List<Message>				MessageList;
	typedef FuzzyHashMap<String, Actor>	ActorList;

	struct DialogPlayer: public Entity {
		DERIVED_CLASS(DialogPlayer, Entity)

		DERIVED_CONSTRUCTOR(DialogPlayer, Entity, {
			addToGame(this, "Dialog");
			autotimer.onSignal = $signal {
				nextMessage();
			};
			isFinished = true;
			autotimer.repeat = true;
			autotimer.stop();
			endtimer.stop();
			box.shape.setRenderLayer(DIALOG_BOTTOM_LAYER);
			box.title.setRenderLayer(DIALOG_LAYER);
			box.message.setRenderLayer(DIALOG_LAYER);
			autotimer.setManual();
			endtimer.setManual();
			keys["next"]	= SDL_SCANCODE_Z;
			keys["skip"]	= SDL_SCANCODE_X;
			input.update();
			input.threshold = 120;
		})

		MessageList	messages;
		ActorList	actors;

		void setCurrentMessage(size_t index) {
			current = index;
			nextMessage();
		}

		void begin() {
			input.update();
			animator.clear();
			box.shape.active	=
			box.title.active	=
			box.message.active	= true;
			for (auto& [aName, actor]: actors) {
				if (!actor.sprite) continue;
				animator[aName].from			=
				animator[aName].to				=
				actor.sprite->local.position	= actor.position.out;
				if (actor.sprite) animator[aName].value = &(actor.sprite->local.position);
				animator[aName].setManual();
			}
			isFinished	= false;
			current		= 0;
		}

		void end() {
			input.update();
			onDialogEnd();
			autotimer.stop();
			endtimer.stop();
			isFinished = true;
		}

		void onFrame(float delta) override {
			endtimer.yield();
			input.update();
			autotimer.yield();
			for (auto& [aName, aTween]: animator) {
				aTween.yield();
			}
			if (isFinished || !endtimer.finished()) return;
			if (!autoplay) {
				if (
					input.isButtonJustPressed(keys["next"])
				||	input.isButtonDown(keys["skip"])
				) nextMessage();
			}
		}

		TypedSignal<size_t>	onMessage	= $tsignal(size_t index) {};
		Signal				onDialogEnd	= $signal {queueDestroy();};

		KeyBinds keys;

		InputManager input;

		struct {
			Renderable	shape;
			Label		title;
			Label		message;
		} box;

		void nextMessage() {
			input.update();
			autotimer.reset();
			if (isFinished || messages.empty() || !endtimer.finished()) return;
			if (current > (messages.size()-1)) {
				end();
				return;
			}
			Message& msg = messages[current];
			size_t tsize = msg.title.size();
			if (
				msg.title[0] != '@'
			&&	msg.title[tsize-1] != ':' && msg.title[1] != ':'
			) {
				for (ActorData& actor: last.actors) {
					auto& anim = animator[actor.name];
					auto& a = actors[actor.name];
					if (!a.sprite) continue;
					anim.tweenStep = msg.easing;
					anim.reinterpolateTo(a.position.rest, time);
					a.sprite->setColor(Color::GRAY);
				}
				for (ActorData& actor: msg.actors) {
					if (!actors.contains(actor.name)) continue;
					auto& anim = animator[actor.name];
					auto& a = actors[actor.name];
					actor.action();
					if (!a.sprite) continue;
					a.sprite->frame = actor.frame;
					anim.reinterpolateTo(actor.leaving ? a.position.out : a.position.talking, time);
					a.sprite->setColor(actor.tint);
				}
				onMessage(current);
				showText(msg.title, msg.text);
			} else {
				last = msg;
				if (msg.title == "@:exit:")	exitDialog();
				if (msg.title == "@:end:")	finish();
			}
			current++;
			autoplay = msg.autoplay;
			autotimer.delay = msg.duration;
			last = msg;
		}

		void loadFromDefinition(JSONData def) {
			try {
				time = def["transitionTime"].get<size_t>();
				MessageList messages;
				for(JSONData msg: def["messages"].get<List<JSONData>>()) {
					Message message;
					// Message packet actor data
					List<JSONData> adat = msg["actors"].get<List<JSONData>>();
					if (!adat.empty()) for(JSONData a: adat) {
						ActorData actor;
						actor.name	= a["name"].get<String>();
						actor.frame	= Vector2(
							a["frame"][0].get<float>(),
							a["frame"][1].get<float>()
						);
						actor.tint	= Vector4(
							a["tint"][0].get<float>(),
							a["tint"][1].get<float>(),
							a["tint"][2].get<float>(),
							a["tint"][3].get<float>()
						);
						actor.leaving = a["leaving"].get<bool>();
						message.actors.push_back(actor);
					}
					// Main message packet data
					StringList ease		= Helper::splitString(msg["easing"].get<String>(), '.');
					message.title		= msg["title"].get<String>();
					message.text		= msg["text"].get<String>();
					message.easing		= Tween::ease[ease[0]][ease[1]];
					message.duration	= msg["duration"].get<size_t>();
					message.autoplay	= msg["autoplay"].get<bool>();
					messages.push_back(message);
				}
				this->messages = messages;
			} catch (JSON::exception e) {
				throw Error::FailedAction(
					"Failed at parsing dialog definition!",
					__FILE__,
					toString(__LINE__),
					"extendFromDefinition",
					e.what(),
					"Please check to see if values are correct!"
				);
			}
		}

		void finish() {
			$debug("[ Finishing dialog... ]");
			if (isFinished || !endtimer.finished()) return;
			exitDialog();
			endtimer.repeat		= false;
			endtimer.delay		= time;
			endtimer.onSignal	= [this](){this->end();};
			endtimer.start();
			autotimer.stop();
			isFinished = true;
		}

		void loadFromDefinitionFile(String const& path) {
			loadFromDefinition(FileLoader::loadJSON(path));
		}

		size_t time = 60;
	private:
		Message last;

		Event::Timer endtimer;

		void showText(String title, String text) {
			box.title.text.content		= title;
			box.message.text.content	= text;
		}

		void exitDialog() {
			for (auto& [actor, _]: actors) {
				auto& anim = animator[actor];
				auto& a = actors[actor];
				if (!a.sprite) continue;
				anim.tweenStep = last.easing;
				anim.reinterpolateTo(a.position.out, time);
				a.sprite->setColor(Color::GRAY);
			}
			box.shape.active	=
			box.title.active	=
			box.message.active	= false;
		}

		Event::Timer autotimer;
		FuzzyHashMap<String, Tween::Tween<Vector3>> animator;

		size_t current	= 0;
		bool autoplay	= false;
		bool isFinished	= false;
	};
}
