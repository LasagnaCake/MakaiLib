namespace UI {
	struct Option;
	struct BaseMenu;

	typedef std::vector<Option*>		OptionSet;
	typedef TypedEvent::Signal<Option*>	OptionAction;

	const OptionAction OPTION_DEF_ACTION = [](Option* o){};

	struct Option {
		const size_t		id;
		BaseMenu* const		menu;
		Option* const 		previous;
		OptionSet			next;
		void*				data			= nullptr;
		bool 				enabled			= true;
		OptionAction		onFrame			= OPTION_DEF_ACTION;
		OptionAction		onSelected		= OPTION_DEF_ACTION;
		OptionAction		onFocusGained	= OPTION_DEF_ACTION;
		OptionAction		onFocusLost		= OPTION_DEF_ACTION;
		OptionAction		onDelete		= OPTION_DEF_ACTION;
	};

	struct BaseMenu: public Entity {
		DERIVED_CLASS(BaseMenu, Entity)

		DERIVED_CONSTRUCTOR(BaseMenu, Entity, {
			keys["select"]		= SDL_SCANCODE_Z;
			keys["return"]		= SDL_SCANCODE_X;
			keys["next"]		= SDL_SCANCODE_UP;
			keys["previous"]	= SDL_SCANCODE_DOWN;
			keys["exit"]		= SDL_SCANCODE_ESCAPE;
			input.threshold		= 300;
			nextOptTimer.onSignal	= $signal {this->nextOption();};
			nextOptTimer.delay		= 4;
			nextOptTimer.paused		= true;
			prevOptTimer.onSignal	= $signal {this->previousOption();};
			prevOptTimer.delay		= 4;
			prevOptTimer.paused		= true;
		})

		~BaseMenu() {
			clearOptions();
		}

		void onCreate() override {}

		void onFrame(float delta) override {
			input.update();
			if (action("select", true))		selectOption();
			if (action("return", true))		returnOption();
			if (action("exit", true))		setTopLevelOption();
			if (action("next", true))		nextOption();
			if (action("previous", true))	nextOption();
			nextOptTimer.paused = !(actionHeld("next") && !action("previous"));
			prevOptTimer.paused = !(actionHeld("previous") && !action("next"));
			for(auto& o : all)
				o->onFrame(o);
		}

		Option* createOption(void* data = nullptr, Option* prev = nullptr) {
			Option* o = new Option{id++, this, prev};
			all.push_back(o);
			return o;
		}

		void removeOption(Option* o) {
			o->onDelete(o);
			all.erase(
				std::find_if(
					all.begin(),
					all.end(),
					[&](auto& e){
						return e->id == o->id;
					}
				)
			);
			delete o;
		}

		void clearOptions() {
			for(Option* o : all) delete o;
			all.clear();
			currentSet.clear();
		}

		Option* getCurrentOption() {
			return current;
		}

		Option* getLastOption() {
			return last;
		}

		bool setOption(size_t option = 0) {
			if (currentSet.empty()) return false;
			if (!currentSet[option]->enabled)
				return false;
			this->option = option;
			last	= current;
			current	= currentSet[option];
			last->onFocusLost(last);
			current->onFocusGained(current);
			onOptionChanged(last, current);
			return true;
		}

		void nextOption() {
			if (currentSet.empty()) return;
			if (option < currentSet.size() - 1) option++;
			else option = 0;
			if (!currentSet[option]->enabled)
				nextOption();
			else
				setOption(option);
		}

		void previousOption() {
			if (currentSet.empty()) return;
			if (option > 0) option--;
			else option = currentSet.size() - 1;
			if (!currentSet[option]->enabled)
				previousOption();
			else
				setOption(option);
		}

		void returnOption() {
			if (!current->previous) {onExit(); return;}
			Option* now		= current->previous->previous;
			Option* prev	= current->previous;
			setCurrentOptionSet(now ? now->next : top);
			setOption(
				std::find_if(
					currentSet.begin(),
					currentSet.end(),
					[&](auto& e){
						return e->id == prev->id;
					}
				) - currentSet.begin()
			);
			onReturn();
		}

		void setTopLevelOption() {do returnOption(); while (current->previous);}

		void selectOption() {
			current->onSelected(current);
			if (current->next.empty()) {
				onFinalOption(current);
				return;
			}
			setCurrentOptionSet(current->next);
			if (!setOption()) {
				option = 0;
				nextOption();
			}
		}

		void setCurrentOptionSet(OptionSet const& o) {
			lastSet		= currentSet;
			currentSet	= o;
			onOptionSetChanged(lastSet, currentSet);
		}

		OptionSet getCurrentOptionSet()	{	return currentSet;	}
		OptionSet getLastOptionSet()	{	return lastSet;		}

		OptionSet top;

		InputManager input;

		KeyBinds keys;

		virtual void onReturn()														{}
		virtual void onExit()														{}
		virtual void onFinalOption(Option* opt)										{}
		virtual void onOptionSelected(Option* opt)									{}
		virtual void onOptionChanged(Option* prev, Option* next)					{}
		virtual void onOptionSetChanged(OptionSet const& prev, OptionSet const& next)	{}

		bool action(std::string what, bool justPressed = false) {
			if (keys[what] == SDL_SCANCODE_UNKNOWN)
				return false;
			if (justPressed)
				return input.isButtonJustPressed(keys[what]);
			else
				return input.getButtonDown(keys[what]);
		}

		bool actionHeld(std::string what) {
			if (keys[what] == SDL_SCANCODE_UNKNOWN)
				return false;
			return input.isButtonHeld(keys[what]);
		}

		size_t actionState(std::string what) {
			if (keys[what] == SDL_SCANCODE_UNKNOWN)
				return false;
			return input.getButtonState(keys[what]);
		}

	private:
		OptionSet		currentSet, lastSet, all;
		Event::Timer	nextOptTimer, prevOptTimer;

		Option*	current	= nullptr;
		Option*	last	= nullptr;

		size_t option = 0;

		size_t id = 0;
	};
}

#define $gui GameData::UI::
