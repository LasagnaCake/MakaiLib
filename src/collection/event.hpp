#ifndef EVENT_SIGNAL_TRIGGER_H
#define EVENT_SIGNAL_TRIGGER_H

#include <functional>
#include <vector>

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

	/*
	Remember children! If you're going to test your code,
	make sure the testing function returns a value of its type
	(when non-void)! You don't want to spend hours on trying to
	figure out why your code behaves oddly to then find out it is
	because you HAD SET THE FUNCTION TYPE TO NON-VOID ACCIDENTALLY,
	AND IT WAS CAUSING THE PROGRAM TO MISBEHAVE AND BREAK DOWN!
	*/
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
		}

		/// Signal + delay + repeat constructor.
		Timer(Signal onSignal, float delay = 1, bool repeat = false, bool manual = false) {
			this->onSignal	= onSignal;
			this->delay		= delay;
			this->repeat	= repeat;
			if (!manual)
				timerList.push_back(&yield);
		}

		/// Delay + repeat constructor.
		Timer(float delay, bool repeat = false, bool manual = false) {
			this->delay		= delay;
			this->repeat	= repeat;
			if (!manual)
				timerList.push_back(&yield);
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

		/**
		* Yields a cycle.
		*/
		const $$FUNC yield = [&](float delta = 1) {
			// If not paused...
			if (!paused) {
				// If counter has reached target...
				if(counter >= delay) {
					// If repeating and not done looping, set counter to 0
					if (repeat && loopCount != 0) counter = 0;
					// Else, stop timer
					else paused = true;
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
		void reset() {
			counter = 0;
		}

		/// Starts the timer from the beginning.
		void start(float time) {
			counter = 0;
			paused = false;
			delay = time;
		}

		/// Starts the timer from the beginning.
		void start() {
			counter = 0;
			paused = false;
		}

		/// Stops/pauses the timer.
		void stop() {
			paused = true;
		}

		/// Unpauses the timer.
		void play() {
			paused = false;
		}

		/// Gets the counter's current value.
		float getCounter() {
			return counter;
		}

	private:
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
