#ifndef TWEEN_TWEENABLE_H
#define TWEEN_TWEENABLE_H

#include "event.hpp"
#include <vector>
#include <functional>

namespace Tween{
	namespace {
		using std::function, std::vector;
		vector<const function<float()>*> tweenList;
	}
	#define $$FUNC function<float()>
	/// Yields all available non-manual tweens.
	void yieldAllTweens() {
		// Loop through tweens and step them
		if (tweenList.size())
			for(const $$FUNC* func : tweenList)
				(*func)();
	}

	/// Easing function template
	typedef function<float(float, float, float, float)> EaseFunc;
	/**
	******************
	*                *
	*  Easing Class  *
	*                *
	******************
	*/
	class Easing
	{
	public:
		//TODO: This mess
	};

	/**
	*****************
	*               *
	*  Tween Class  *
	*               *
	*****************
	*/
	class Tween
	{
	public:
		/// The tween's target variable.
		float *value = &defaultVar;

		/// The tween's easing function.
		EaseFunc tweenStep;

		/// The signal to be fired upon completion.
		Event::Signal onCompleted = Event::DEF_SIGNAL;

		/// Empty Constructor.
		Tween(bool manual = false) {
			if (!manual)
				tweenList.push_back(&yield);
		}

		/// Targetless Constructor.
		Tween(float from, float to, unsigned int step, EaseFunc tweenStep, bool manual = false) {
			setInterpolation(from, to, step, tweenStep);
			if (!manual)
				tweenList.push_back(&yield);
		}

		/// Targeted Constructor.
		Tween(float from, float to, unsigned int step, EaseFunc tweenStep, float* targetVar, bool manual = false) {
			setInterpolation(from, to, step, tweenStep, targetVar);
			if (!manual)
				tweenList.push_back(&yield);
		}

		/// Destructor.
		~Tween() {
			// Loop through tweens and...
			for (size_t i = 0; i < tweenList.size(); i++)
				// If tween matches...
				if (tweenList[i] == &yield) {
					// Remove tween from list and end loop
					tweenList.erase(tweenList.begin() + i);
					break;
			}
			value = &defaultVar;
		}

		/// Calculates (and if targeted, applies) a step.
		const $$FUNC yield = [&]() {
			// If value pointer is null, point to default var
			if (!value) value = &defaultVar;
			// If not finished...
			if (!isFinished)
			{
				// Check if finished, then increment step
				isFinished = step++ >= stop;
				// If begin != end, calculate step
				if (from != to) *value = -tweenStep(step, from, range, stop);
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
			// Return value
			return *value;
		};

		/// Sets the interpolation.
		void setInterpolation(float from, float to, unsigned int step, EaseFunc tweenStep) {
			isFinished = false;
			this->step = 0;
			this->from = from;
			this->to = to;
			stop = step;
			range = from - to;
			*value = from;
			this->tweenStep = tweenStep;
		}

		/// Sets the interpolation with a target.
		void setInterpolation(float from, float to, unsigned int step, EaseFunc tweenStep, float* targetVar) {
			value = targetVar;
			isFinished = false;
			this->step = 0;
			this->from = from;
			this->to = to;
			stop = step;
			range = from - to;
			*value = from;
			this->tweenStep = tweenStep;
		}

		/// Sets the interpolation to a new value, while maintaining the easing function.
		void reinterpolate(float to, unsigned int step) {
			setInterpolation(*value, to, step, tweenStep);
		}
		/// Sets the interpolation to a new value, while maintaining the easing dunction.
		void reinterpolate(float from, float to, unsigned int step) {
			setInterpolation(from, to, step, tweenStep);
		}
		/// Removes the tween's target variable;
		void clearTarget() {
			defaultVar = *value;
			value = &defaultVar;
		}

		/// Sets the current tween step.
		void setStep(unsigned int step) {
			step--;
			this->step = step > stop ? stop : step;
			yield();
		}

		/// Gets the current tween step number.
		float getStep() {
			return step;
		}

		/// Gets the current tween value.
		float getValue() {
			return *value;
		}

		/// Returns whether the tween is finished.
		bool finished() {
			return isFinished;
		}

	private:
		/// Whether the tween is finished.
		bool isFinished;

		/// The tween's starting value.
		float from;

		/// The tween's target value.
		float to;

		/// The difference between from and to (Required for calculations).
		float range;

		/// The current tween step.
		unsigned int step;

		/// The amount of steps to be executed.
		unsigned int stop;

		/// The tween's default target.
		float defaultVar;
	};
	#undef $$FUNC
}

#endif // TWEEN_TWEENABLE_H
