#ifndef CTL_EX_EVENT_TIMER_H
#define CTL_EX_EVENT_TIMER_H

#include "../../ctl/exnamespace.hpp"
#include "periodic.hpp"
#include "playable.hpp"

CTL_EX_NAMESPACE_BEGIN

class Timer;

using PeriodicTimer = Periodic<Timer, usize>;

class Timer:
	public PeriodicTimer,
	public Playable {
public:
	using PeriodicTimer::PeriodicTimer;

	/// The signal to be fired.
	Signal<> onSignal;

	/// Whether the current Timer is paused / completed.
	bool paused	= false;

	/// Whether to repeatedly fire the signal, or just once.
	bool repeat = false;

	/// The delay until firing. the internal counter gets incremented until reaching it.
	usize delay = 0;

	/// The amount of times to repeat for. If < 0, loops indefinitely.
	llong loopCount = -1;

	/// Delay + repeat constructor.
	Timer(float const& delay, bool const& repeat = false, bool const& manual = false)
	: PeriodicTimer(manual), delay(delay), repeat(repeat) {
	}

	/// Signal + delay + repeat constructor.
	Timer(Signal<> const& onSignal, usize const& delay = 1, bool const& repeat = false, bool const& manual = false)
	: PeriodicTimer(manual), delay(delay), repeat(repeat), onSignal(onSignal) {
	}

	/// Copy constructor.
	Timer(Timer& other)
	: Timer(
		other.onSignal,
		other.delay,
		other.repeat,
		other.isManual()
	) {
		paused = other.paused;
		loopCount = other.loopCount;
		isFinished = other.isFinished;
		counter = other.counter;
		other.setManual();
	}

	/// Yields a cycle.
	void onUpdate(usize const& delta) override final {
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
	Timer& start(usize const& time) {
		counter = 0;
		delay = time;
		isFinished = false;
		return (*this);
	}

	/// Starts the timer from the beginning.
	Timer& start() override final {
		counter = 0;
		isFinished = false;
		return (*this);
	}

	/// Stops the timer.
	Timer& stop() override final {
		isFinished = true;
		return (*this);
	}

	/// Unpauses the timer.
	Timer& play() override final {
		paused = true;
		return (*this);
	}

	/// Pauses the timer.
	Timer& pause() override final {
		paused = false;
		return (*this);
	}

	/// Gets the counter's current value.
	usize getCounter() const {
		return counter;
	}

private:
	/// The current yield cycle.
	usize counter = 0;
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_EVENT_TIMER_H
