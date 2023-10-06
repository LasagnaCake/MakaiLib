namespace Menu {
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
		size_t				nextID = 0;
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
			input.update();
			keys["select"]		= SDL_SCANCODE_Z;
			keys["return"]		= SDL_SCANCODE_X;
			keys["next"]		= SDL_SCANCODE_DOWN;
			keys["previous"]	= SDL_SCANCODE_UP;
			keys["exit"]		= SDL_SCANCODE_ESCAPE;
			input.threshold		= 30;
			nextOptTimer.onSignal	= SIGNAL {this->nextOption();};
			nextOptTimer.delay		= 5;
			nextOptTimer.paused		= true;
			nextOptTimer.repeat		= true;
			prevOptTimer.onSignal	= SIGNAL {this->previousOption();};
			prevOptTimer.delay		= 5;
			prevOptTimer.paused		= true;
			prevOptTimer.repeat		= true;
		})

		virtual ~BaseMenu() {
			clearOptions();
		}

		void onCreate() override {}

		void onFrame(float delta) override {
			input.update();
			if (action("exit", true))		{setTopLevelOption(); return;}
			if (action("select", true))		{selectOption(); return;}
			if (action("return", true))		{returnOption(); return;}
			if (action("next", true))		nextOption();
			if (action("previous", true))	previousOption();
			nextOptTimer.paused = !(actionHeld("next") && !action("previous"));
			prevOptTimer.paused = !(actionHeld("previous") && !action("next"));
			for(auto& o : currentSet)
				o->onFrame(o);
		}

		Option* createOption(Option* prev = nullptr) {
			Option* o = new Option{id++, this, prev};
			all.push_back(o);
			return o;
		}

		void removeOption(Option* o) {
			o->onDelete(o);
			ERASE_IF(all, elem->id == o->id);
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
			if (last) last->onFocusLost(last);
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
			if (!current) return;
			current->onSelected(current);
			if (current->next.empty()) {
				onFinalOption(current);
				return;
			}
			setCurrentOptionSet(current->next);
			if (!setOption(current->nextID)) {
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

		virtual void onReturn()															{DEBUGLN("Return!");}
		virtual void onExit()															{DEBUG("Menu Exited!");}
		virtual void onFinalOption(Option* opt)											{DEBUG("Final Option Selected: "); if (opt) {DEBUGLN(opt->id);} else {DEBUGLN("NULL");}}
		virtual void onOptionSelected(Option* opt)										{DEBUG("Option Selected: "); if (opt) {DEBUGLN(opt->id);} else {DEBUGLN("NULL");}}
		virtual void onOptionSetChanged(OptionSet const& prev, OptionSet const& next)	{DEBUGLN("Option Set Changed!");}
		virtual void onOptionChanged(Option* prev, Option* next) {
			DEBUG("Option Changed: ");
			if (prev) {DEBUG(prev->id);}
			else {DEBUG("NULL");}
			DEBUG(" -> ");
			if (next) {DEBUGLN(next->id);}
			else {DEBUGLN("NULL");}
		}

		bool action(std::string what, bool justPressed = false) {
			if (justPressed)
				return input.isButtonJustPressed(keys[what]);
			else
				return input.isButtonDown(keys[what]);
		}

		bool actionHeld(std::string what) {
			return input.isButtonHeld(keys[what]);
		}

		size_t actionState(std::string what) {
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
