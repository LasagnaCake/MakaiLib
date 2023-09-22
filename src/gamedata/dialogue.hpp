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
		$twn EaseFunc	easing		= Tween::ease.out.cubic;
		size_t			duration	= 600;
		bool			autoplay	= false;
	};

	typedef List<Message>				MessageList;
	typedef FuzzyHashMap<String, Actor>	ActorGroup;

	struct DialogPlayer: public Entity {
		DERIVED_CLASS(DialogPlayer, Entity)

		DERIVED_CONSTRUCTOR(DialogPlayer, Entity, {
			addToGame(this, "Dialog");
			autotimer.onSignal = [this](){this->nextMessage();};
			isFinished = true;
			autotimer.repeat	= true;
			endtimer.repeat		= false;
			autotimer.stop();
			endtimer.stop();
			box.shape.setRenderLayer(DIALOG_LAYER - 1);
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
		ActorGroup	actors;

		void setCurrentMessage(size_t index) {
			current = index;
			nextMessage();
		}

		void begin() {
			input.update();
			animator.clear();
			autotimer.start(999999);
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
			if (!isMetaTag(msg.title)) {
				box.shape.active	=
				box.title.active	=
				box.message.active	= true;
				for (ActorData& actor: last.actors) {
					auto& anim = animator[actor.name];
					auto& a = actors[actor.name];
					if (!a.sprite) continue;
					anim.tweenStep = msg.easing;
					anim.reinterpolateTo(actor.leaving ? a.position.out: a.position.rest, time);
					a.sprite->setColor(Color::GRAY);
				}
				for (ActorData& actor: msg.actors) {
					if (!actors.contains(actor.name)) continue;
					auto& anim = animator[actor.name];
					auto& a = actors[actor.name];
					actor.action();
					if (!a.sprite) continue;
					anim.tweenStep = msg.easing;
					a.sprite->frame = actor.frame;
					anim.reinterpolateTo(actor.leaving ? a.position.out : a.position.talking, time);
					inScene[actor.name] = !actor.leaving;
					a.sprite->setColor(actor.tint);
				}
				onMessage(current);
				showText(msg.title, msg.text);
				autoplay = msg.autoplay;
				autotimer.delay = msg.duration;
			} else {
				last.duration	= msg.duration;
				last.text		= msg.text;
				last.title		= msg.title;
				last.actors		= msg.actors;
				if (msg.title == "@:exit:")		exitDialog();
				if (msg.title == "@:reenter:")	reEnterDialog();
				if (msg.title == "@:end:")		finish();
			}
			current++;
			last = msg;
		}

		void loadFromDefinition(JSONData def) {
			try {
				time = def["transitionTime"].get<size_t>();
				MessageList messages;
				Message global;
				if (def["easing"].is_string()) {
					StringList ease	= Helper::splitString(def["easing"].get<String>(), '.');
					global.easing	= Tween::ease[ease[0]][ease[1]];
				}
				if (def["duration"].is_number_integer())
					global.duration	= def["duration"].get<size_t>();
				if (def["autoplay"].is_boolean())
					global.autoplay	= def["autoplay"].get<bool>();
				for(JSONData msg: def["messages"].get<List<JSONData>>()) {
					Message message = global;
					// Message packet actor data
					List<JSONData> adat = msg["actors"].get<List<JSONData>>();
					if (!adat.empty()) for(JSONData a: adat) {
						ActorData actor;
						actor.name	= a["name"].get<String>();
						actor.frame	= Vector2(
							a["frame"][0].get<float>(),
							a["frame"][1].get<float>()
						);
						if (a["tint"].is_array())
							actor.tint	= Vector4(
								a["tint"][0].get<float>(),
								a["tint"][1].get<float>(),
								a["tint"][2].get<float>(),
								a["tint"][3].get<float>()
							);
						if (a["leaving"].is_boolean())
							actor.leaving = a["leaving"].get<bool>();
						message.actors.push_back(actor);
					}
					// Main message packet data
					// Check for metatag
					message.title		= msg["title"].get<String>();
					if (!isMetaTag(message.title))
						message.text	= msg["text"].get<String>();
					else if (msg["text"].is_string())
						message.text	= msg["text"].get<String>();
					if (msg["easing"].is_string()) {
						StringList ease		= Helper::splitString(msg["easing"].get<String>(), '.');
						message.easing		= Tween::ease[ease[0]][ease[1]];
					}
					if (msg["duration"].is_number_integer())
						message.duration	= msg["duration"].get<size_t>();
					if (msg["autoplay"].is_boolean())
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
		HashMap<String, bool> inScene;

		Message last;

		Event::Timer endtimer;

		inline bool isMetaTag(String const& tag) {return tag[0] == '@' && tag[tag.size()-1] == ':' && tag[1] == ':';}

		void showText(String title, String text) {
			box.title.text.content		= title;
			box.message.text.content	= text;
		}

		void reEnterDialog() {
			for (auto& [actor, isInScene]: inScene)
				if (isInScene) {
					auto& anim = animator[actor];
					auto& a = actors[actor];
					if (!a.sprite) continue;
					anim.tweenStep = last.easing;
					anim.reinterpolateTo(a.position.rest, last.duration);
					a.sprite->setColor(Color::GRAY);
				}
			autoplay		= true;
			autotimer.delay	= last.duration;
			autotimer.reset();
			box.shape.active	=
			box.title.active	=
			box.message.active	= false;
		}

		void exitDialog() {
			for (auto& [actor, _]: actors) {
				auto& anim = animator[actor];
				auto& a = actors[actor];
				if (!a.sprite) continue;
				anim.tweenStep = last.easing;
				anim.reinterpolateTo(a.position.out, last.duration);
				a.sprite->setColor(Color::GRAY);
			}
			autoplay		= true;
			autotimer.delay	= last.duration;
			autotimer.reset();
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
