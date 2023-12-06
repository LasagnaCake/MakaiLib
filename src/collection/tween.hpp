#ifndef TWEEN_TWEENABLE_H
#define TWEEN_TWEENABLE_H

#include "event.hpp"
#include "algebra.hpp"
#include "vectorn.hpp"
#include "conceptual.hpp"
#include "helper.hpp"
#include "easing.hpp"
#include <vector>
#include <functional>
#include <string>

#define CASE_FUNC(F_NAME) if ( type == #F_NAME ) return F_NAME

namespace Tweening {
	namespace {
		using TypedEvent::Signal;
		List<const Function<void(float)>*> tweenList;
	}

	/**
	* A 'Tweenable' type is defined as:
	*	a)	An 'Operatable' type that can be constructed
			from a single floating-point value.
	*/
	template <typename T>
	concept Tweenable = Math::Operatable<T> && Type::Constructible<T, float>;

	/// Yields all available non-manual tweens.
	void yieldAllTweens(float delta = 1) {
		// Loop through tweens and step them
		if (tweenList.size())
			for(const Signal<float>* func : tweenList)
				(*func)(delta);
	}

	/**
	*****************
	*               *
	*  Tween Class  *
	*               *
	*****************
	*/
	template <Tweenable T = float>
	class Tween {
	public:
		typedef T DataType;

		/// The tween's target variable.
		T* value = &defaultVar;

		/// The tween's easing function.
		Ease::EaseMode easeMode = Ease::linear;

		/// The signal to be fired upon completion.
		Event::Signal onCompleted = Event::DEF_SIGNAL;

		/// Whether the tween is paused.
		bool paused = false;

		/// The tween's starting value.
		T from;

		/// The tween's target value.
		T to;

		/// Empty Constructor.
		Tween(bool manual = false) {
			if (!manual)
				tweenList.push_back(&_yield);
			this->manual = manual;
		}

		/// Targetless Constructor.
		Tween(T from, T to, unsigned long step, Ease::EaseMode easeMode, bool manual = false) {
			setInterpolation(from, to, step, easeMode);
			if (!manual)
				tweenList.push_back(&_yield);
			this->manual = manual;
		}

		/// Targeted Constructor.
		Tween(T from, T to, unsigned long step, Ease::EaseMode easeMode, T* targetVar, bool manual = false) {
			setInterpolation(from, to, step, easeMode, targetVar);
			if (!manual)
				tweenList.push_back(&_yield);
			this->manual = manual;
		}

		/// Copy constructor
		Tween(Tween& other)
		: Tween(
			other.from,
			other.to,
			other.step,
			other.easeMode,
			other.manual
		) {
			if (other.value != &other.defaultVar)
				value = other.value;
			paused = other.paused;
			onCompleted = other.onCompleted;
			isFinished = other.isFinished;
			factor = other.factor;
			other.setManual();
		}

		/// Copy constructor
		Tween(Tween&& other): Tween(other) {}

		/// Destructor.
		~Tween() {
			// Loop through tween calls and delete if matches
			if (!tweenList.empty()) std::erase_if(tweenList, [&](auto& e){return e == &_yield;});
			value = &defaultVar;
		}

		void setManual() {
			if (manual) return;
			// Loop through tween calls and delete if matches
			if (!tweenList.empty()) std::erase_if(tweenList, [&](auto& e){return e == &_yield;});
			manual = true;
		}

		void setAutomatic() {
			if (!manual) return;
			tweenList.push_back(&_yield);
		}

		/// Calculates (and if targeted, applies) a step.
		void yield(float delta = 1) {
			// If value pointer is null, point to default var
			if (!value) value = &defaultVar;
			// If paused, return
			if (paused) return;
			// If not finished...
			if (!isFinished)
			{
				// Check if finished, then increment step
				isFinished = step >= stop;
				step += delta;
				// If begin != end, calculate step
				if (from != to) {
					factor = easeMode(float(step)/float(stop));
					*value = Math::lerp(from, to, T(factor));
				}
				// Else, set value to end
				else *value = to;
				// Clamp step to prevent overflow
				step = (step > stop ? stop : step);
				// If done, fire signal and clear it
				if (isFinished) {
					onCompleted();
				}
			}
			// // Else, clamp value to between required values
			// else *value = (*value > to ? to : (*value < from ? from : *value))
		}

		/// Sets the interpolation.
		Tween<T>& setInterpolation(T from, T to, unsigned long step, Ease::EaseMode easeMode) {
			isFinished = false;
			this->step = 0;
			this->from = from;
			this->to = to;
			stop = step;
			*value = from;
			this->easeMode = easeMode;
			factor = 0.0f;
			return *this;
		}

		/// Sets the interpolation with a target.
		Tween<T>& setInterpolation(T from, T to, unsigned long step, Ease::EaseMode easeMode, T* targetVar) {
			value = targetVar;
			isFinished = false;
			this->step = 0;
			this->from = from;
			this->to = to;
			stop = step;
			*value = from;
			this->easeMode = easeMode;
			factor = 0.0f;
			return *this;
		}

		/// Sets the interpolation to a new value, while maintaining the easing function.
		Tween<T>& reinterpolateTo(T to) {
			paused = false;
			setInterpolation(*value, to, step, easeMode);
			return *this;
		}

		/// Sets the interpolation to a new value, while maintaining the easing dunction.
		Tween<T>& reinterpolate(T from, T to) {
			paused = false;
			setInterpolation(from, to, step, easeMode);
			return *this;
		}

		/// Sets the interpolation to a new value, while maintaining the easing function.
		Tween<T>& reinterpolateTo(T to, unsigned long step) {
			paused = false;
			setInterpolation(*value, to, step, easeMode);
			return *this;
		}

		/// Sets the interpolation to a new value, while maintaining the easing dunction.
		Tween<T>& reinterpolate(T from, T to, unsigned long step) {
			paused = false;
			setInterpolation(from, to, step, easeMode);
			return *this;
		}

		/// Sets the tween's target variable.
		Tween<T>& setTarget(T* target) {
			value = target;
			return *this;
		}

		/// Removes the tween's target variable.
		Tween<T>& clearTarget() {
			defaultVar = *value;
			value = &defaultVar;
			return *this;
		}

		/// Sets the current tween step.
		Tween<T>& setStep(unsigned long step) {
			step--;
			this->step = step > stop ? stop : step;
			yield();
			return *this;
		}

		/// Sets the current tween step count.
		Tween<T>& setStepCount(unsigned long stop) {
			this->stop = stop;
			return *this;
		}

		/// Gets the current tween step number.
		T getStep() {
			return step;
		}

		/// Gets the current tween value.
		T getValue() {
			return *value;
		}

		/// Returns whether the tween is finished.
		bool finished() {
			return isFinished;
		}

		/// Halts the tween's execution, and sets it to its end value.
		Tween<T>& conclude() {
			*value = to;
			step = stop;
			factor = 1.0f;
			isFinished = true;
			return *this;
		}

		/// Starts the tween with its current state.
		Tween<T>& play() {
			factor = 1.0f;
			isFinished = false;
			return *this;
		}

		/// Halts the tween's execution.
		Tween<T>& halt() {
			factor = 1.0f;
			isFinished = true;
			return *this;
		}

	private:
		/// Internal signal used for automatic processing.
		const Signal<float> _yield = [&](float delta = 1) {
			this->yield(delta);
		};

		bool manual = false;

		float factor = 1.0f;

		/// Whether the tween is finished.
		bool isFinished = true;

		/// The current tween step.
		unsigned long step;

		/// The amount of steps to be executed.
		unsigned long stop;

		/// The tween's default target.
		T defaultVar;
	};
}

#undef CASE_FUNC

#endif // TWEEN_TWEENABLE_H
