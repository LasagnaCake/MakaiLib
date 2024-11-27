#ifndef CTL_EX_EVENT_PERIODIC_H
#define CTL_EX_EVENT_PERIODIC_H

#include "../../ctl/exnamespace.hpp"
#include "../../ctl/container/list.hpp"
#include "../../ctl/container/function.hpp"

CTL_EX_NAMESPACE_BEGIN

/// @brief Event that fires periodically.
/// @tparam _ Class associated with the event.
/// @tparam TDelta Delta type.
template<class _ = void, CTL::Type::Number TDelta = usize>
class Periodic {
public:
	/// @brief Event wrapper.
	using EventWrapper	= Function<void(TDelta const&)>;
	/// @brief Event list.
	using EventList		= List<EventWrapper const*>;
	/// @brief Delta type.
	using DeltaType		= TDelta;

	/// @brief Constructs the periodic.
	/// @param manual Whether the event is fired manually. By default, it is `false`.
	Periodic(bool const& manual = false): manual(manual) {
		if (!manual)
			events.pushBack(&update);
	}

	/// @brief Yields all available non-manual periodic events.
	/// @param delta Delta between events.
	static void process(DeltaType const& delta = 1) {
		if (events.size())
			for(EventWrapper const* func : events)
				(*func)(delta);
	}

	/// @brief Sets the periodic event to be manually executed.
	void setManual() {
		if (manual) return;
		if (!events.empty())
			events.eraseIf([&](auto const& e){return e == &update;});
		manual = true;
	}

	/// @brief Sets the periodic event to be automatically executed.
	void setAutomatic() {
		if (!manual) return;
		events.pushBack(&update);
		manual = false;
	}

	/// @brief Destructor.
	virtual ~Periodic() {
		if (!manual && !events.empty())
			events.eraseIf([&](auto const& e){return e == &update;});
	}

	/// @brief Called when the event needs to be fired.
	/// @param delta Delta between events.
	virtual void onUpdate(DeltaType const& delta) = 0;

	/// @brief Returns whether the periodic event is manually executed.
	/// @return Whether the periodic event is manually executed.
	bool isManual() {return manual;}

private:
	/// @brief Event wrapper.
	const EventWrapper update = [this](DeltaType const& delta) {onUpdate(delta);};

	/// @brief Whether the periodic event is manually executed.
	bool manual = false;

	/// @brief All automatic events.
	inline static EventList events;
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_EVENT_PERIODIC_H
