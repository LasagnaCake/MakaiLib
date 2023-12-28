#ifndef EVENT_SIGNAL_TRIGGER_H
#define EVENT_SIGNAL_TRIGGER_H

#include <functional>
#include <vector>
#include "helper.hpp"
#include "conceptual.hpp"

#define T_SIGNAL_U(...)		(__VA_ARGS__)->void
#define T_TRIGGER_U(...)	(__VA_ARGS__)->bool
#define T_CALL_U(...)		(__VA_ARGS__)->Trigger
#define T_CHAIN_U(...)		(__VA_ARGS__)->Signal

#define	T_SIGNAL(...)	[&] T_SIGNAL_U(__VA_ARGS__)
#define	T_TRIGER(...)	[&] T_TRIGGER_U(__VA_ARGS__)
#define	T_CALL(...)		[&] T_CALL_U(__VA_ARGS__)
#define	T_CHAIN(...)	[&] T_CHAIN_U(__VA_ARGS__)

#define SIGNAL			T_SIGNAL()
#define TRIGGER			T_TRIGGER()
#define CALL			T_CALL()
#define CHAIN			T_CHAIN()

namespace Event{
	namespace {
		List<const Procedure<float>*> timerList;
	}

	/// Yields all available non-manual timers.
	void yieldAllTimers(float delta = 1) {
		// Loop through timers and step them
		if (timerList.size())
			for(const Procedure<float>* func : timerList)
				(*func)(delta);
	}

	/// A signal to be fired, whenever.
	typedef Function<void()>		Signal;

	/// A trigger to wait for.
	typedef Function<bool()>		Trigger;

	/// A call to be fired, which returns a Trigger to wait for.
	typedef Function<Trigger()>		Call;

	/// A signal to be fired whenever, which returns a Signal.
	typedef Function<Signal()>		Chain;

	const Signal	DEF_SIGNAL	=	[]()->void		{};
	const Trigger	DEF_TRIGGER	=	[]()->bool		{return true;};
	const Call		DEF_CALL	=	[]()->Trigger	{return DEF_TRIGGER;};
	const Chain		DEF_CHAIN	=	[]()->Signal	{return DEF_SIGNAL;};

	/**
	*****************
	*               *
	*  Timer Class  *
	*               *
	*****************
	*/
	class Timer {
	public:
		/// The signal to be fired.
		Signal onSignal = Event::DEF_SIGNAL;

		/// Whether the current Timer is paused / completed.
		bool paused	= false;

		/// Whether to repeatedly fire the signal, or just once.
		bool repeat = false;

		/// The delay until firing. the internal counter gets incremented until reaching it.
		float delay = 0;

		/// The amount of times to repeat for. If < 0, loops indefinitely.
		long loopCount = -1;

		/// Empty constructor.
		Timer(bool manual = false) {
			if (!manual)
				timerList.push_back(&_yield);
			this->manual = manual;
		}

		/// Signal + delay + repeat constructor.
		Timer(Signal onSignal, float delay = 1, bool repeat = false, bool manual = false) {
			this->onSignal	= onSignal;
			this->delay		= delay;
			this->repeat	= repeat;
			if (!manual)
				timerList.push_back(&_yield);
			this->manual = manual;
		}

		/// Delay + repeat constructor.
		Timer(float delay, bool repeat = false, bool manual = false) {
			this->delay		= delay;
			this->repeat	= repeat;
			if (!manual)
				timerList.push_back(&_yield);
			this->manual = manual;
		}

		Timer(Timer& other)
		: Timer(
			other.onSignal,
			other.delay,
			other.repeat,
			other.manual
		) {
			paused = other.paused;
			loopCount = other.loopCount;
			isFinished = other.isFinished;
			counter = other.counter;
			other.setManual();
		}

		Timer(Timer&& other): Timer(other) {}

		/// Destructor.
		~Timer() {
			// Loop through timer calls and delete if matches
			if (!timerList.empty()) std::erase_if(timerList, [&](auto& e){return e == &_yield;});
		}

		void setManual() {
			if (manual) return;
			// Loop through timer calls and delete if matches
			if (!timerList.empty()) std::erase_if(timerList, [&](auto& e){return e == &_yield;});
			manual = true;
		}

		void setAutomatic() {
			if (!manual) return;
			timerList.push_back(&_yield);
		}

		/**
		* Yields a cycle.
		*/
		void yield(float delta = 1) {
			// If not paused or not finished...
			if (!isFinished && !paused) {
				// If counter has reached target...
				if(counter >= delay) {
					// If repeating and not done looping, set counter to 0
					if (repeat && loopCount != 0) counter = 0;
					// Else, stop timer
					else isFinished = true;
					// Fire signal
					onSignal();
					// If loop count above zero, decrease it
					if (loopCount > 0) loopCount--;
				}
				// Increment counter
				counter += delta;
			}
		}

		/// Resets the Timer's counter to 0.
		Timer& reset() {
			counter = 0;
			return (*this);
		}

		/// Starts the timer from the beginning.
		Timer& start(float time) {
			counter = 0;
			delay = time;
			isFinished = false;
			return (*this);
		}

		/// Starts the timer from the beginning.
		Timer& start() {
			counter = 0;
			isFinished = false;
			return (*this);
		}

		/// Stops the timer.
		Timer& stop() {
			isFinished = true;
			return (*this);
		}

		/// Unpauses the timer.
		Timer& play() {
			paused = true;
			return (*this);
		}

		/// Pauses the timer.
		Timer& pause() {
			paused = false;
			return (*this);
		}

		/// Gets the counter's current value.
		float getCounter() {
			return counter;
		}

		/// Gets whether the timer is done executing.
		bool finished() {
			return isFinished;
		}

	private:
		/// Internal signal used for automatic processing.
		const Procedure<float> _yield = [&](float delta = 1) {
			this->yield(delta);
		};

		bool manual = false;

		/// Whether the timer is finished.
		bool isFinished = false;

		/// The current yield cycle.
		float counter = 0;
	};
}

namespace TypedEvent {
	/// A signal to be fired, whenever.
	template <typename T = void>
	using Signal = Function<void(T)>;

	/// A trigger to wait for.
	template <typename T = void>
	using Trigger = Function<bool(T)>;

	/// A call to be fired, which returns a Trigger to wait for.
	template <typename T = void, typename T2 = void>
	using Call = Function<Trigger<T>(T2)>;

	/// A call to be fired, which returns a Signal.
	template <typename T = void, typename T2 = void>
	using Chain = Function<Signal<T>(T2)>;
}

#endif // SIGNAL_TRIGGER_H
