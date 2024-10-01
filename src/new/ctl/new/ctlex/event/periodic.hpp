#ifndef CTL_EX_EVENT_PERIODIC_H
#define CTL_EX_EVENT_PERIODIC_H

#include "../../ctl/namespace.hpp"
#include "../../ctl/container/list.hpp"
#include "../../ctl/container/function.hpp"

CTL_EX_NAMESPACE_BEGIN

template<class _ = void, Type::Number TDelta = usize>
class Periodic {
public:
	using EventWrapper	= Procedure<TDelta>;
	using EventList		= List<EventWrapper const*>;

	/// Empty constructor.
	Periodic(bool const& manual = false): manual(manual) {
		if (!manual)
			events.pushBack(&doYield);
	}

	/// Yields all available non-manual periodic events.
	static void process(TDelta const& delta = 1) {
		// Loop through timers and step them
		if (events.size())
			for(EventWrapper const* func : events)
				(*func)(delta);
	}

	void setManual() {
		if (manual) return;
		// Loop through tween calls and delete if matches
		if (!events.empty())
			std::erase_if(events, [&](auto& e){return e == &doYield;});
		manual = true;
	}

	void setAutomatic() {
		if (!manual) return;
		events.push_back(&doYield);
		manual = false;
	}

	virtual ~PeriodicEvent() {
		// Loop through tween calls and delete if matches
		if (!manual && !events.empty())
			events.eraseIf([&](auto& e){return e == &doYield;});
	}

	virtual void onUpdate(usize const& delta) = 0;

	bool isManual() {return manual;}

private:
	const EventWrapper update = [&](TDelta const& delta) {this->onUpdate(delta);};

	bool manual = false;

	inline static EventList events;
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_EVENT_PERIODIC_H
