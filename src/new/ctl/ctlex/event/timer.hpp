#ifndef CTL_EX_EVENT_TIMER_H
#define CTL_EX_EVENT_TIMER_H

#include "../../ctl/exnamespace.hpp"
#include "periodic.hpp"
#include "playable.hpp"

CTL_EX_NAMESPACE_BEGIN

class Timer;

/// @brief Timer-specific periodic event.
using PeriodicTimer = Periodic<Timer, usize>;

/// @brief Timer-based periodic event.
class Timer:
	public PeriodicTimer,
	public Playable {
public:
	using PeriodicTimer::PeriodicTimer;

	/// @brief Whether the current Timer is paused / completed.
	bool paused	= false;

	/// @brief Whether to repeatedly fire the event.
	bool repeat = false;

	/// @brief Time between signal events.
	usize delay = 0;

	/// @brief The amount of times to repeat for. If less than 0, loops indefinitely.
	llong loopCount = -1;

	/// @brief Constructs the timer.
	/// @param delay Time between signal events.
	/// @param repeat Whether to repeatedly fire the event.
	/// @param manual Whether the timer should be manually updated.
	Timer(usize const delay, bool const repeat = false, bool const manual = false)
	: PeriodicTimer(manual), repeat(repeat), delay(delay) {
	}

	/// @brief Move constructor.
	/// @param other `Timer` to move.
	Timer(Timer&& other):
	paused(CTL::move(other.paused)),
	repeat(CTL::move(other.repeat)),
	delay(CTL::move(other.delay)),
	loopCount(CTL::move(other.loopCount)),
	counter(CTL::move(other.counter)) {
		isFinished = CTL::move(other.isFinished);
	}

	/// @brief Copy constructor (deleted).
	Timer(Timer const& other) = delete;

	/// @brief Yields a cycle.
	/// @param delta Delta between cycles.
	void onUpdate(usize delta) override final {
		// If not paused or not finished...
		if (!isFinished && !paused) {
			// If counter has reached target...
			if(counter >= delay) {
				// If repeating and not done looping, set counter to 0
				if (repeat && loopCount != 0) counter = 0;
				// Else, stop timer
				else isFinished = true;
				// Fire signal
				onEvent();
				// If loop count above zero, decrease it
				if (loopCount > 0) loopCount--;
			}
			// Increment counter
			counter += delta;
		}
	}

	/// @brief Virtual destructor.
	virtual ~Timer() {}

	/// @brief Event to fire.
	virtual void onEvent() = 0;

	/// @brief Resets the internal counter to the start.
	/// @return Reference to self.
	Timer& reset() {
		counter = 0;
		return (*this);
	}

	/// @brief Starts the timer from the beginning, with a given delay.
	/// @brief time Time between signal events.
	/// @return Reference to self.
	Timer& start(usize const time) {
		delay = time;
		return start();
	}

	/// @brief Starts the timer from the beginning.
	/// @return Reference to self.
	Timer& start() override final {
		counter = 0;
		isFinished = false;
		return (*this);
	}

	/// @brief Stops the timer.
	/// @return Reference to self.
	Timer& stop() override final {
		isFinished = true;
		return (*this);
	}

	/// @brief Unpauses the timer.
	/// @return Reference to self.
	Timer& play() override final {
		paused = true;
		return (*this);
	}

	/// @brief Pauses the timer.
	/// @return Reference to self.
	Timer& pause() override final {
		paused = false;
		return (*this);
	}

	/// @brief Returns the internal counter's current value.
	/// @return Value of internal counter.
	usize getCounter() const {
		return counter;
	}

private:
	/// @brief Internal counter.
	usize counter = 0;
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_EVENT_TIMER_H
