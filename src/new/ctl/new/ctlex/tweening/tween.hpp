#ifndef CTL_EX_TWEENING_TWEEN_H
#define CTL_EX_TWEENING_TWEEN_H

#include "../../ctl/exnamespace.hpp"
#include "../../ctl/container/arguments.hpp"
#include "../../ctl/container/list.hpp"
#include "../math/ease.hpp"
#include "../event/timer.hpp"

CTL_EX_NAMESPACE_BEGIN

/**
* A 'Tweenable' type is defined as:
*	a)	An 'Operatable' type that can be constructed
		from a single floating-point value.
*/
template <typename T>
concept Tweenable = Math::Operatable<T> && Type::Constructible<T, float>;

template <Tweenable T = float>
class Tween;

typedef Event::PeriodicEvent<Tween<>> PeriodicTween;

struct Stepable {
	virtual usize		getStep()					{return step;	}
	virtual Stepable&	setStep(usize const&)		{return *this;	}
	virtual Stepable&	setStepCount(usize const&)	{return *this;	}

protected:
	usize step = 0;
};

struct PlayableTween: Playable {
	virtual PlayableTween& conclude()	{return *this;	}
	virtual PlayableTween& halt()		{return *this;	}

	Signal<> onCompleted = Event::DEF_SIGNAL;

private:
	virtual PlayableTween& stop()		{return *this;	}
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
	public PeriodicTween,
	public Playable,
	public Stepable {
public:
	typedef T DataType;

	using
		PeriodicTween::PeriodicTween
	;

	/// The tween's easing function.
	Ease::Mode easeMode = Ease::linear;

	/// The tween's starting value.
	T from;

	/// The tween's target value.
	T to;

	/// Targetless Constructor.
	Tween(T const& from, T const& to, usize const& step, Ease::EaseMode const& easeMode, bool const& manual = false)
	: PeriodicTween(manual) {
		setInterpolation(from, to, step, easeMode);
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
		paused = other.paused;
		onCompleted = other.onCompleted;
		isFinished = other.isFinished;
		factor = other.factor;
		other.setManual();
	}

	/// Destructor.
	virtual ~Tween() {}

	/// Calculates (and if targeted, applies) a step.
	void onUpdate(usize const& delta) override final {
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
				current = Math::lerp(from, to, T(factor));
			}
			// Else, set target to end
			else current = to;
			// Clamp step to prevent overflow
			step = (step > stop ? stop : step);
			// If done, fire signal
			if (isFinished)
				onCompleted();
		}
	}

	/// Sets the interpolation.
	Tween<T>& setInterpolation(T const& from, T const& to, usize const& step, Ease::EaseMode const& easeMode) {
		isFinished = false;
		this->step = 0;
		this->from = from;
		this->to = to;
		stop = step;
		value = from;
		this->easeMode = easeMode;
		factor = 0.0f;
		return *this;
	}

	/// Sets the interpolation with a target.
	Tween<T>& setInterpolation(T const& from, T const& to, usize const& step, Ease::EaseMode const& easeMode) {
		target = targetVar;
		isFinished = false;
		this->step = 0;
		this->from = from;
		this->to = to;
		stop = step;
		value = from;
		this->easeMode = easeMode;
		factor = 0.0f;
		return *this;
	}

	/// Sets the interpolation to a new value, while maintaining the easing function.
	Tween<T>& reinterpolateTo(T const& to) {
		paused = false;
		setInterpolation(value, to, step, easeMode);
		return *this;
	}

	/// Sets the interpolation to a new value, while maintaining the easing dunction.
	Tween<T>& reinterpolate(T const& from, T const& to) {
		paused = false;
		setInterpolation(from, to, step, easeMode);
		return *this;
	}

	/// Sets the interpolation to a new value, while maintaining the easing function.
	Tween<T>& reinterpolateTo(T const& to, usize const& step) {
		paused = false;
		setInterpolation(value, to, step, easeMode);
		return *this;
	}

	/// Sets the interpolation to a new value, while maintaining the easing dunction.
	Tween<T>& reinterpolate(T const& from, T const& to, usize const& step) {
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

	T value() const {
		return current;
	}

private:
	using
		Stepable::step,
		Playable::isFinished
	;

	T current;

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
	Ease::Mode ease				= Ease::linear;
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
	public PeriodicTween,
	public Playable,
	public Stageable {
public:
	typedef T DataType;

	typedef StageData<DataType>	Stage;
	typedef List<Stage>			StageList;
	typedef Arguments<Stage>	StageArguments;

	using
		PeriodicTween::PeriodicTween
	;

	Event::Signal onCompleted = Event::DEF_SIGNAL;

	bool paused = false;

	TweenChain(StageList const& stages, bool const& manual = false)
	: PeriodicTween(manual) {
		setInterpolation(stages);
	}

	TweenChain(StageArguments const& stages, bool const& manual = false)
	: PeriodicTween(manual) {
		setInterpolation(stages);
	}

	void onUpdate(usize const& delta) override final {
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
				currentValue = Math::lerp(current.from, current.to, T(factor));
			}
			// Else, set target to end
			currentValue = current.to;
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
		step = stages.front().step;
		return halt();
	}

	T value() const {
		return currentValue;
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

	T currentValue;

	float factor = 1.0f;
};

#undef CASE_FUNC

CTL_EX_NAMESPACE_END

#endif // CTL_EX_TWEENING_TWEEN_H
