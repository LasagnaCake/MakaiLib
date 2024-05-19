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

	struct Tweener {
		Tweener(bool manual = false) {
			if (!manual) setAutomatic();
			this->manual = true;
		}

		void setManual() {
			if (manual) return;
			// Loop through tween calls and delete if matches
			if (!tweenList.empty())
				std::erase_if(tweenList, [&](auto& e){return e == &_yield;});
			manual = true;
		}

		void setAutomatic() {
			if (!manual) return;
			tweenList.push_back(&_yield);
			manual = false;
		}

		virtual ~Tweener() {
			// Loop through tween calls and delete if matches
			if (!manual && !tweenList.empty())
				std::erase_if(tweenList, [&](auto& e){return e == &_yield;});
		}

		virtual void yield(unsigned long delta = 1) = 0;

		bool isManual() {return manual;}

	private:
		const Signal<float> _yield = [&](float delta = 1) {
			this->yield(delta);
		};

		bool manual = false;
	};

	/**
	*****************
	*               *
	*  Tween Class  *
	*               *
	*****************
	*/
	template <Tweenable T = float>
	class Tween: public Tweener {
	public:
		typedef T DataType;

		using Tweener::Tweener;

		/// The tween's target variable.
		T* target = &defaultVar;

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

		/// Targetless Constructor.
		Tween(T from, T to, unsigned long step, Ease::EaseMode easeMode, bool manual = false)
		: Tweener(manual) {
			setInterpolation(from, to, step, easeMode);
		}

		/// Targeted Constructor.
		Tween(T from, T to, unsigned long step, Ease::EaseMode easeMode, T* targetVar, bool manual = false)
		: Tweener(manual) {
			setInterpolation(from, to, step, easeMode, targetVar);
		}

		/// Copy constructor
		Tween(Tween& other)
		: Tween(
			other.from,
			other.to,
			other.step,
			other.easeMode,
			other.isManual()
		) {
			if (other.target != &other.defaultVar)
				target = other.target;
			paused = other.paused;
			onCompleted = other.onCompleted;
			isFinished = other.isFinished;
			factor = other.factor;
			other.setManual();
		}

		/// Copy constructor
		Tween(Tween&& other)
		: Tween(
			other.from,
			other.to,
			other.step,
			other.easeMode,
			other.isManual()
		) {
			if (other.target != &other.defaultVar)
				target = other.target;
			paused = other.paused;
			onCompleted = other.onCompleted;
			isFinished = other.isFinished;
			factor = other.factor;
			other.setManual();
		}

		/// Destructor.
		virtual ~Tween() {}

		/// Calculates (and if targeted, applies) a step.
		void yield(unsigned long delta = 1) override final {
			// If target pointer is null, point to default var
			if (!target) target = &defaultVar;
			// If paused, return
			if (paused) return;
			// If not finished...
			if (!isFinished) {
				// Check if finished, then increment step
				isFinished = step >= stop;
				step += delta;
				// If begin != end, calculate step
				if (from != to) {
					factor = easeMode(float(step)/float(stop));
					*target = Math::lerp(from, to, T(factor));
				}
				// Else, set target to end
				else *target = to;
				// Clamp step to prevent overflow
				step = (step > stop ? stop : step);
				// If done, fire signal
				if (isFinished)
					onCompleted();
			}
		}

		/// Sets the interpolation.
		Tween<T>& setInterpolation(T from, T to, unsigned long step, Ease::EaseMode easeMode) {
			isFinished = false;
			this->step = 0;
			this->from = from;
			this->to = to;
			stop = step;
			*target = from;
			this->easeMode = easeMode;
			factor = 0.0f;
			return *this;
		}

		/// Sets the interpolation with a target.
		Tween<T>& setInterpolation(T from, T to, unsigned long step, Ease::EaseMode easeMode, T* targetVar) {
			target = targetVar;
			isFinished = false;
			this->step = 0;
			this->from = from;
			this->to = to;
			stop = step;
			*target = from;
			this->easeMode = easeMode;
			factor = 0.0f;
			return *this;
		}

		/// Sets the interpolation to a new value, while maintaining the easing function.
		Tween<T>& reinterpolateTo(T to) {
			paused = false;
			setInterpolation(*target, to, step, easeMode);
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
			setInterpolation(*target, to, step, easeMode);
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
			this->target = target;
			return *this;
		}

		/// Removes the tween's target variable.
		Tween<T>& clearTarget() {
			defaultVar = *target;
			target = &defaultVar;
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
			return *target;
		}

		/// Returns whether the tween is finished.
		bool finished() {
			return isFinished;
		}

		/// Halts the tween's execution, and sets it to its end value.
		Tween<T>& conclude() {
			*target = to;
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

	template<Tweenable T = float>
	struct StageData {
		typedef T DataType;

		/// The starting & ending values.
		DataType from, to;
		/// The amount of steps to take.
		unsigned long step;
		/// The easing function to use.
		Ease::EaseMode ease			= Ease::linear;
		/// The target for this specific stage.
		DataType* target			= nullptr;
		/// Action to do when this stage is concluded.
		Event::Signal onCompleted	= Event::DEF_SIGNAL;
	};

	template<Tweenable T = float>
	class TweenChain: Tweener {
	public:
		typedef T DataType;

		typedef StageData<DataType>	Stage;
		typedef List<Stage>			StageList;
		typedef Arguments<Stage>	StageArguments;

		using Tweener::Tweener;

		DataType* target = &defaultVar;

		Event::Signal onCompleted = Event::DEF_SIGNAL;

		bool paused = false;

		TweenChain(StageList const& stages, bool const& manual = false)
		: Tweener(manual) {
			setInterpolation(stages);
		}

		TweenChain(StageArguments const& stages, bool const& manual = false)
		: Tweener(manual) {
			setInterpolation(stages);
		}

		void yield(unsigned long delta = 1) override final {
			// If target pointer is null, point to default var
			if (!target) target = &defaultVar;
			// If paused, return
			if (paused) return;
			// If not finished...
			if (!isFinished) {
				// If current stage is done...
				if (step >= current.step) {
					// Fire stage's onCompleted signal;
					current->onCompleted();
					// Check if truly finished, else continue on to next stage
					if (!(isFinished = stage >= stages.size())) {
						current	= stages[stage++];
						step	= 0;
					}
				}
				// Increment step counter
				step += delta;
				// If begin != end, calculate step
				if (current.from != current.to) {
					factor = current.ease(float(step)/float(current.step));
					if (current.target)
						*current.target = Math::lerp(current.from, current.to, T(factor));
					else
						*target = Math::lerp(current.from, current.to, T(factor));
				}
				// Else, set target to end
				else if (current.target)
					*current.target = current.to;
				else *target = current.to;
				// Clamp step to prevent overflow
				step = (step > current.step ? current.step : step);
				// If done, fire signal
				if (isFinished)
					onCompleted();
			}
		}

		TweenChain& setTarget(DataType* const& target) {
			this->target = target;
			return *this;
		}

		TweenChain& clearTarget() {
			defaultVar = *target;
			target = &defaultVar;
			return *this;
		}

		TweenChain& setInterpolation(StageList const& stages) {
			this->stages = stages;
			return start();
		}

		TweenChain& setInterpolation(StageArguments const& stages) {
			return setInterpolation(StageList(stages));
		}

		/// Sets the current tween step.
		TweenChain& setStep(unsigned long step) {
			step--;
			this->step = step > current.step ? current.step : step;
			yield();
			return *this;
		}

		/// Sets the current tween stage.
		TweenChain& setStage(unsigned long stage) {
			this->stage = stage >= stages.size() ? stages.size() : stage;
			yield();
			return *this;
		}

		/// Gets the current tween stage number.
		T getStep() {
			return stage;
		}

		/// Gets the current tween value.
		T getValue() {
			return *target;
		}

		/// Returns whether the tween is finished.
		bool finished() {
			return isFinished;
		}

		/// Starts the tween with its current state.
		TweenChain& play() {
			factor = 1.0f;
			isFinished = false;
			return *this;
		}

		/// Starts the tween from the beginning.
		TweenChain& start() {
			step = 0;
			stage = 0;
			current = stages[stage++];
			return play();
		}

		/// Halts the tween's execution.
		TweenChain& halt() {
			factor = 1.0f;
			isFinished = true;
			return *this;
		}

		/// Halts the tween's execution, and sets it to its end value.
		TweenChain& conclude() {
			*target = stages.front().to;
			step = stages.front().step;
			return halt();
		}

		StageList stages;

	private:
		Stage current;

		unsigned long step = 0, stage = 0;

		float factor = 1.0f;

		bool isFinished = true;

		DataType defaultVar;
	};
}

#undef CASE_FUNC

#endif // TWEEN_TWEENABLE_H
