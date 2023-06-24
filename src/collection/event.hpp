#ifndef EVENT_SIGNAL_TRIGGER_H
#define EVENT_SIGNAL_TRIGGER_H

#include <functional>
#include <vector>
#include "conceptual.hpp"

#define $_tsignal(...)	(...)->void
#define $_ttrigger(...)	(...)->bool
#define $_tcall(...)	(...)->Trigger
#define $_tchain(...)	(...)->Signal

#define	$tsignal(...)	[&] $_tsignal(...)
#define	$ttrigger(...)	[&] $_ttrigger(...)
#define	$tcall(...)		[&] $_tcall(...)
#define	$tchain(...)	[&] $_tchain(...)

#define $signal			$tsignal()
#define $trigger		$ttrigger()
#define $call			$tcall()
#define $chain			$tchain()

namespace Event{
	namespace {
		using std::function, std::vector;
		vector<const function<void(float)>*> timerList;
	}
	#define $$FUNC function<void(float)>
	/// Yields all available non-manual timers.
	void yieldAllTimers(float delta = 1) {
		// Loop through timers and step them
		if (timerList.size())
			for(const $$FUNC* func : timerList)
				(*func)(delta);
	}

	/// A signal to be fired, whenever.
	typedef function<void()>		Signal;

	/// A trigger to wait for.
	typedef function<bool()>		Trigger;

	/// A call to be fired, which returns a Trigger to wait for.
	typedef function<Trigger()>		Call;

	/// A signal to be fired whenever, which returns a Signal.
	typedef function<Signal()>		Chain;

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
				timerList.push_back(&yield);
			this->manual = manual;
		}

		/// Signal + delay + repeat constructor.
		Timer(Signal onSignal, float delay = 1, bool repeat = false, bool manual = false) {
			this->onSignal	= onSignal;
			this->delay		= delay;
			this->repeat	= repeat;
			if (!manual)
				timerList.push_back(&yield);
			this->manual = manual;
		}

		/// Delay + repeat constructor.
		Timer(float delay, bool repeat = false, bool manual = false) {
			this->delay		= delay;
			this->repeat	= repeat;
			if (!manual)
				timerList.push_back(&yield);
			this->manual = manual;
		}

		/// Destructor.
		~Timer() {
			// Loop through timers and...
			for (size_t i = 0; i < timerList.size(); i++)
				// If timer matches...
				if (timerList[i] == &yield) {
					// Remove timer from list and end loop
					timerList.erase(timerList.begin() + i);
					break;
			}
		}

		void setManual() {
			if (manual) return;
			// Loop through tweens and...
			for (size_t i = 0; i < timerList.size(); i++)
				// If tween matches...
				if (timerList[i] == &yield) {
					// Remove tween from list and end loop
					timerList.erase(timerList.begin() + i);
					break;
			}
			manual = true;
		}

		void setAutomatic() {
			if (!manual) return;
			timerList.push_back(&yield);
		}

		/**
		* Yields a cycle.
		*/
		const $$FUNC yield = [&](float delta = 1) {
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
		};

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
		bool manual = false;

		/// Whether the timer is finished.
		bool isFinished = false;

		/// The current yield cycle.
		float counter = 0;
	};
	#undef $$FUNC
}

namespace TypedEvent {
	namespace {
		using std::function, std::vector;
	}

	/// A signal to be fired, whenever.
	template <typename T = void>
	using Signal = function<void(T)>;

	/// A trigger to wait for.
	template <typename T = void>
	using Trigger = function<bool(T)>;

	/// A call to be fired, which returns a Trigger to wait for.
	template <typename T = void, typename T2 = void>
	using Call = function<Trigger<T>(T2)>;

	/// A call to be fired, which returns a Signal.
	template <typename T = void, typename T2 = void>
	using Chain = function<Signal<T>(T2)>;
}

#define $evt Event::
#define $tev TypedEvent::

#endif // SIGNAL_TRIGGER_H
