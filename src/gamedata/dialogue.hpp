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
	};

	typedef List<ActorData>	ActorDataList;

	struct Message {
		ActorDataList	actors;
		String			title;
		String			text;
		Vector4			titleColor	= Color::WHITE;
		Vector4			textColor	= Color::WHITE;
		Tween::EaseFunc	easing		= Tween::ease.out.cubic;
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

		TypedSignal<size_t>	onMessage	= T_SIGNAL(size_t index) {};
		Signal				onDialogEnd	= SIGNAL {queueDestroy();};

		virtual void onAction(Message const& msg) {}

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
			if (!isActionTag(msg.title)) {
				box.shape.active	=
				box.title.active	=
				box.message.active	= true;
				for (ActorData& actor: last.actors) {
					auto& anim = animator[actor.name];
					auto& a = actors[actor.name];
					if (!a.sprite) continue;
					anim.tweenStep = msg.easing;
					anim.reinterpolateTo(actor.leaving ? a.position.out: a.position.rest, time);
					a.sprite->setColor(standbyColor);
				}
				for (ActorData& actor: msg.actors) {
					if (!actors.contains(actor.name)) continue;
					auto& anim = animator[actor.name];
					auto& a = actors[actor.name];
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
				last = msg;
				executeAction(msg);
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
					// Check for metatag
					message.title = msg["title"].get<String>();
					// Message packet actor data
					if (msg["actors"].is_array()) {
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
							else if (a["tint"].is_string())
								actor.tint = Color::fromHexCodeString(a["tint"].get<String>());
							if (a["leaving"].is_boolean())
								actor.leaving = a["leaving"].get<bool>();
							message.actors.push_back(actor);
						}
					}
					// Main message packet data
					if (!isActionTag(message.title))
						message.text	= msg["text"].get<String>();
					else if (msg["text"].is_string())
						message.text	= msg["text"].get<String>();
					// Title color data
					if (msg["titleColor"].is_array())
						message.titleColor	= Vector4(
							msg["titleColor"][0].get<float>(),
							msg["titleColor"][1].get<float>(),
							msg["titleColor"][2].get<float>(),
							msg["titleColor"][3].get<float>()
						);
					else if (msg["titleColor"].is_string())
						message.titleColor = Color::fromHexCodeString(msg["titleColor"].get<String>());
					// Text color data
					if (msg["textColor"].is_array())
						message.textColor	= Vector4(
							msg["textColor"][0].get<float>(),
							msg["textColor"][1].get<float>(),
							msg["textColor"][2].get<float>(),
							msg["textColor"][3].get<float>()
						);
					else if (msg["textColor"].is_string())
						message.textColor = Color::fromHexCodeString(msg["textColor"].get<String>());
					// Other stuff
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
			DEBUGLN("[ Finishing dialog... ]");
			if (isFinished || !endtimer.finished()) return;
			endtimer.repeat		= false;
			endtimer.delay		= time;
			endtimer.onSignal	= [this](){this->end();};
			endtimer.start();
			autotimer.stop();
			isFinished = true;
			actionExit();
		}

		void executeAction(Message const& msg) {
			if		(msg.title == "@:exit:")		actionExit();
			else if	(msg.title == "@:reenter:")		actionReenter();
			else if	(msg.title == "@:wait:")		actionWait();
			else if	(msg.title == "@:finish:")		actionFinish();
			else if	(msg.title == "@:setframe:")	actionSetFrame();
			else if	(msg.title == "@:setcolor:")	actionSetColor();
			else	onAction(msg);
		}

		void loadFromDefinitionFile(String const& path) {
			loadFromDefinition(FileLoader::loadJSON(path));
		}

		size_t time = 60;

		Vector4 standbyColor = Color::GRAY;

	private:
		void actionSetColor() {
			StringList tag = Helper::splitString(last.text, '=');
			Vector4 color = Color::fromHexCodeString(tag[1]);
			if		(tag[0] == "standby")	standbyColor = color;
			else if	(tag[0] == "title")		box.title.material.color	= color;
			else if	(tag[0] == "text")		box.message.material.color	= color;
		}

		void actionReenter() {
			for (auto& [actor, isInScene]: inScene)
				if (isInScene) {
					auto& anim = animator[actor];
					auto& a = actors[actor];
					if (!a.sprite) continue;
					anim.tweenStep = last.easing;
					anim.reinterpolateTo(a.position.rest, last.duration);
					a.sprite->setColor(standbyColor);
				}
			actionWait();
		}

		void actionExit() {
			for (auto& [actor, _]: actors) {
				auto& anim = animator[actor];
				auto& a = actors[actor];
				if (!a.sprite) continue;
				anim.tweenStep = last.easing;
				anim.reinterpolateTo(a.position.out, last.duration);
				a.sprite->setColor(standbyColor);
			}
			actionWait();
		}

		void actionSetFrame() {
			for (ActorData& actor: last.actors) {
				if (!actors.contains(actor.name)) continue;
				auto& a = actors[actor.name];
				if (!a.sprite) continue;
				a.sprite->frame = actor.frame;
			}
			nextMessage();
		}

		void actionFinish() {
			time = last.duration;
			finish();
		}

		void actionWait() {
			autoplay		= true;
			autotimer.delay	= last.duration;
			autotimer.reset();
			box.shape.active	=
			box.title.active	=
			box.message.active	= false;
		}

		HashMap<String, bool> inScene;

		Message last;

		Event::Timer endtimer;

		inline bool isActionTag(String const& tag) {return tag[0] == '@' && tag[tag.size()-1] == ':' && tag[1] == ':';}

		void showText(String title, String text) {
			box.title.text.content		= title;
			box.message.text.content	= text;
		}

		Event::Timer autotimer;
		FuzzyHashMap<String, Tween::Tween<Vector3>> animator;

		size_t current	= 0;
		bool autoplay	= false;
		bool isFinished	= false;
	};
}
