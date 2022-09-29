#ifndef EVENT_SIGNAL_TRIGGER_H
#define EVENT_SIGNAL_TRIGGER_H

#include <functional>
#include <vector>

#define $_signal	()->void
#define $_trigger	()->void
#define $_call		()->void
#define $_chain		()->void

#define	$signal		[&] $_signal
#define	$trigger	[&] $_trigger
#define	$call		[&] $_call
#define	$chain		[&] $_chain

namespace Event{
	namespace {
		using std::function, std::vector;
		vector<const function<void()>*> timerList;
	}
	#define $$FUNC function<void()>
	/// Yields all available non-manual timers.
	void yieldAllTimers() {
		// Loop through timers and step them
		if (timerList.size())
			for(const $$FUNC* func : timerList)
				(*func)();
	}

	/// A signal to be fired, whenever.
	typedef function<void()>	Signal;

	/// A trigger to wait for.
	typedef function<bool()>	Trigger;

	/// A call to be fired, which returns a Trigger to wait for.
	typedef function<Trigger()>	Call;

	/// A signal to be fired whenever, which returns a Signal.
	typedef function<Signal()>	Chain;

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
		unsigned int delay = 0;

		/// Empty constructor.
		Timer(bool manual = false) {
			if (!manual)
				timerList.push_back(&yield);
		}

		/// Signal + delay + repeat constructor.
		Timer(Signal onSignal, unsigned int delay = 1, bool repeat = false, bool manual = false) {
			this->onSignal	= onSignal;
			this->delay		= delay;
			this->repeat	= repeat;
			if (!manual)
				timerList.push_back(&yield);
		}

		/// Delay + repeat constructor.
		Timer(unsigned int delay, bool repeat = false, bool manual = false) {
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
		const $$FUNC yield = [&]() {
			// If not paused...
			if (!paused) {
				// If counter has reached target...
				if(counter >= delay) {
					// If repeating, set counter to 0
					if (repeat) counter = 0;
					// Else, stop timer
					else paused = true;
					// Fire signal
					onSignal();
				}
				// Increment counter
				counter++;
			}
		};

		/// Resets the Timer's counter to 0.
		void reset() {
			counter = 0;
		}

		/// Gets the counter's current value.
		unsigned int getCounter() {
			return counter;
		}

	private:
		/// The current yield cycle.
		unsigned int counter = 0;
	};
	#undef $$FUNC
}

#endif // SIGNAL_TRIGGER_H
