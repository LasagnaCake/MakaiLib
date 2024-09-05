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
	/**
	* A 'Tweenable' type is defined as:
	*	a)	An 'Operatable' type that can be constructed
			from a single floating-point value.
	*/
	template <typename T>
	concept Tweenable = Math::Operatable<T> && Type::Constructible<T, float>;

	template <Tweenable T = float>
	class Tween;

	typedef Event::PeriodicEvent<Tween<>> Tweener;

	template<typename T>
	struct Targetable {
		typedef T DataType;

		virtual Targetable&	setTarget(T* const& newTarget)	{if (newTarget) target = newTarget; return *this;			}
		virtual Targetable&	clearTarget()					{defaultVar = *target; target = &defaultVar; return *this;	}
		virtual T			getValue()						{return *target;											}

		T* target = &defaultVar;

	protected:
		DataType defaultVar;
	};

	struct Stepable {
		virtual usize		getStep()					{return step;	}
		virtual Stepable&	setStep(usize const&)		{return *this;	}
		virtual Stepable&	setStepCount(usize const&)	{return *this;	}

	protected:
		usize step = 0;
	};

	struct Playable: Event::Playable {
		virtual Playable& conclude()	{return *this;	}
		virtual Playable& halt()		{return *this;	}

		Event::Signal onCompleted = Event::DEF_SIGNAL;

	private:
		virtual Playable& stop()		{return *this;	}
	};

	/**
	*****************
	*               *
	*  Tween Class  *
	*               *
	*****************
	*/
	template <Tweenable T>
	class Tween:
		public Tweener,
		public Targetable<T>,
		public Playable,
		public Stepable {
	public:
		typedef T DataType;

		using
			Tweener::Tweener,
			Targetable<T>::target
		;

		/// The tween's easing function.
		Ease::EaseMode easeMode = Ease::linear;

		/// The tween's starting value.
		T from;

		/// The tween's target value.
		T to;

		/// Targetless Constructor.
		Tween(T from, T to, usize step, Ease::EaseMode easeMode, bool manual = false)
		: Tweener(manual) {
			setInterpolation(from, to, step, easeMode);
		}

		/// Targeted Constructor.
		Tween(T from, T to, usize step, Ease::EaseMode easeMode, T* targetVar, bool manual = false)
		: Tweener(manual) {
			setInterpolation(from, to, step, easeMode, targetVar);
		}

		/// Copy constructor.
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

		/// Destructor.
		virtual ~Tween() {}

		/// Calculates (and if targeted, applies) a step.
		void yield(usize delta = 1) override final {
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
		Tween<T>& setInterpolation(T from, T to, usize step, Ease::EaseMode easeMode) {
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
		Tween<T>& setInterpolation(T from, T to, usize step, Ease::EaseMode easeMode, T* targetVar) {
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
		Tween<T>& reinterpolateTo(T to, usize step) {
			paused = false;
			setInterpolation(*target, to, step, easeMode);
			return *this;
		}

		/// Sets the interpolation to a new value, while maintaining the easing dunction.
		Tween<T>& reinterpolate(T from, T to, usize step) {
			paused = false;
			setInterpolation(from, to, step, easeMode);
			return *this;
		}

		/// Sets the current tween step.
		Tween<T>& setStep(usize const& step) override final {
			this->step = step > stop ? stop : step;
			yield();
			return *this;
		}

		/// Sets the current tween step count.
		Tween<T>& setStepCount(usize const& stop) override final {
			this->stop = stop;
			return *this;
		}

		/// Sets the tween's target variable.
		Tween<T>& setTarget(T* const& target) override final {
			Targetable<T>::setTarget(target);
			return *this;
		}

		/// Removes the tween's target variable.
		Tween<T>& clearTarget() override final {
			Targetable<T>::clearTarget();
			return *this;
		}

		/// Halts the tween's execution, and sets it to its end value.
		Tween<T>& conclude() override final {
			*target = to;
			step = stop;
			factor = 1.0f;
			isFinished = true;
			return *this;
		}

		/// Starts the tween with its current state.
		Tween<T>& play() override final {
			factor = 1.0f;
			paused = false;
			isFinished = false;
			return *this;
		}

		/// Pauses the tween.
		Tween<T>& pause() override final {
			paused = true;
			return *this;
		}

		/// Halts the tween's execution.
		Tween<T>& halt() override final {
			factor = 1.0f;
			isFinished = true;
			return *this;
		}

	private:
		using
			Targetable<T>::defaultVar,
			Stepable::step,
			Playable::isFinished
		;

		float factor = 1.0f;

		/// The amount of steps to be executed.
		usize stop;
	};

	template<Tweenable T = float>
	struct StageData {
		typedef T DataType;

		/// The starting & ending values.
		DataType from, to;
		/// The amount of steps to take.
		usize step;
		/// The easing function to use.
		Ease::EaseMode ease			= Ease::linear;
		/// The target for this specific stage.
		DataType* target			= nullptr;
		/// Action to do when this stage is concluded.
		Event::Signal onCompleted	= Event::DEF_SIGNAL;
	};

	struct Stageable: Stepable {
		virtual Stageable&	setStage(usize const&)		{return *this;	}
		virtual usize		getStage()					{return stage;	}

	protected:
		usize stage = 0;

	private:
		virtual Stageable&	setStepCount(usize const&)	{return *this;	}
	};

	template<Tweenable T = float>
	class TweenChain:
		public Tweener,
		public Targetable<T>,
		public Playable,
		public Stageable {
	public:
		typedef T DataType;

		typedef StageData<DataType>	Stage;
		typedef List<Stage>			StageList;
		typedef Arguments<Stage>	StageArguments;

		using
			Tweener::Tweener,
			Targetable<T>::target
		;

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

		void yield(usize delta = 1) override final {
			// If target pointer is null, point to default var
			if (!target) target = &defaultVar;
			// If paused, return
			if (paused) return;
			// If not finished...
			if (!isFinished) {
				// If current stage is done...
				if (step >= current.step) {
					// Fire stage's onCompleted signal;
					current.onCompleted();
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

		TweenChain& setInterpolation(StageList const& stages) {
			this->stages = stages;
			return start();
		}

		TweenChain& setInterpolation(StageArguments const& stages) {
			return setInterpolation(StageList(stages));
		}

		TweenChain& setInterpolation(StageList const& stages, DataType* const& target) {
			setInterpolation(stages);
			setTarget(target);
			return start();
		}

		TweenChain& setInterpolation(StageArguments const& stages, DataType* const& target) {
			return setInterpolation(StageList(stages), target);
		}

		/// Sets the current tween step.
		TweenChain& setStep(usize const& step) override final {
			this->step = (step-1) > current.step ? current.step : (step-1);
			yield();
			return *this;
		}

		/// Sets the current tween stage.
		TweenChain& setStage(usize const& stage) override final {
			this->stage = stage >= stages.size() ? stages.size() : stage;
			yield();
			return *this;
		}

		/// Gets the current tween stage number.
		usize getStage() override final {
			return stage;
		}

		/// Sets the tween's target variable.
		TweenChain& setTarget(T* const& target) override final {
			Targetable<T>::setTarget(target);
			return *this;
		}

		/// Removes the tween's target variable.
		TweenChain& clearTarget() override final {
			Targetable<T>::clearTarget();
			return *this;
		}

		/// Gets the current tween step number.
		usize getStep() override final {
			return step;
		}

		/// Starts the tween with its current state.
		TweenChain& play() override final {
			factor = 1.0f;
			paused = false;
			isFinished = false;
			return *this;
		}

		/// Pauses the tween.
		TweenChain& pause() override final {
			paused = true;
			return *this;
		}

		/// Starts the tween from the beginning.
		TweenChain& start() override final {
			step = 0;
			stage = 0;
			current = stages[stage++];
			return play();
		}

		/// Halts the tween's execution.
		TweenChain& halt() override final {
			factor = 1.0f;
			isFinished = true;
			return *this;
		}

		/// Halts the tween's execution, and sets it to its end value.
		TweenChain& conclude() override final {
			*target = stages.front().to;
			step = stages.front().step;
			return halt();
		}

		StageList stages;

	private:
		using
			Targetable<T>::defaultVar,
			Stageable::step,
			Stageable::stage,
			Playable::isFinished
		;

		Stage current;

		float factor = 1.0f;
	};
}

#undef CASE_FUNC

#endif // TWEEN_TWEENABLE_H
