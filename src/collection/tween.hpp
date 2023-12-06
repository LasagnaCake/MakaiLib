#ifndef TWEEN_TWEENABLE_H
#define TWEEN_TWEENABLE_H

#include "event.hpp"
#include "algebra.hpp"
#include "vectorn.hpp"
#include "conceptual.hpp"
#include "helper.hpp"
#include <vector>
#include <functional>
#include <string>

#define CASE_FUNC(F_NAME) if ( type == #F_NAME ) return F_NAME

namespace Tween{
	/*
		TODO:
			Rework easing system using https://easings.net/ 's easing functions.
		NOTE:
			Since the function already barely gets used the way it was intended,
				and the current way is close to the above mentioned.
			Probably turn the easing class into a namespace, perhaps?
			But then again, std::function seems to hate when you try to pass a non-lambda...
	*/

	namespace {
		using TypedEvent::Signal;
		List<const Function<void(float)>*> tweenList;
		typedef Function<float(float, float, float, float)> _EaseFunc;
		struct EaseType {
			_EaseFunc linear;
			_EaseFunc sine;
			_EaseFunc quad;
			_EaseFunc cubic;
			_EaseFunc quart;
			_EaseFunc quint;
			_EaseFunc expo;
			_EaseFunc elastic;
			_EaseFunc circ;
			_EaseFunc bounce;
			_EaseFunc back;
			_EaseFunc const& operator[](String const& type) const {
				CASE_FUNC(linear);
				CASE_FUNC(sine);
				CASE_FUNC(quad);
				CASE_FUNC(cubic);
				CASE_FUNC(quart);
				CASE_FUNC(quint);
				CASE_FUNC(expo);
				CASE_FUNC(elastic);
				CASE_FUNC(circ);
				CASE_FUNC(bounce);
				CASE_FUNC(back);
				return linear;
			}
		};
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

	// Shut up for two seconds
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wsequence-point"
	#pragma GCC diagnostic ignored "-Wsubobject-linkage"

	#define EASING [](float step, float from, float to, float stop) -> float
	#define E_FUNC [&](float t, float b, float c, float d) -> float
	#define PI_VALUE Math::pi
	#define EASE_F(TYPE) .TYPE = E_FUNC
	/// Easing function template
	typedef _EaseFunc EaseFunc;

	// These are a port of https://github.com/jesusgollonet/ofpennereasing.
	/**
	******************
	*                *
	*  Easing Class  *
	*                *
	******************
	*/
	struct Easing {
		/// Ease IN functions
		const EaseType in {
			// Linear
			EASE_F(linear) {return c * t/d + b;},
			// Sine
			EASE_F(sine) {return -c * cos(t/d * (PI_VALUE/2)) + c + b;},
			// Quad
			EASE_F(quad) {return c*(t/=d)*t + b;},
			// Cubic
			EASE_F(cubic) {return c*(t/=d)*t*t + b;},
			// Quart
			EASE_F(quart) {return c*(t/=d)*t*t*t + b;},
			// Quint
			EASE_F(quint) {return c*((t=t/d-1)*t*t*t*t + 1) + b;},
			// Expo
			EASE_F(expo) {return (t==0) ? b : c * pow(2, 10 * (t/d - 1)) + b;},
			// Elastic
			EASE_F(elastic) {
				if (t==0) return b;
				if ((t/=d)==1) return b+c;
				float p=d*.3f;
				float a=c;
				float s=p/4;
				float postFix =a*pow(2,10*(t-=1));
				return -(postFix * sin((t*d-s)*(2*PI_VALUE)/p )) + b;
			},
			// Circ
			EASE_F(circ) {return -c * (sqrt(1 - (t/=d)*t) - 1) + b;},
			// Bounce
			EASE_F(bounce) {
				float s = 1.70158f;
				return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
			},
			// Back
			EASE_F(back) {
				float s = 1.70158f;
				float postFix = t/=d;
				return c*(postFix)*t*((s+1)*t - s) + b;
			}
		};
		/// Ease OUT functions
		const EaseType out {
			// Linear
			EASE_F(linear) {return c * t/d + b;},
			// Sine
			EASE_F(sine) {return c * sin(t/d * (PI_VALUE/2)) + b;},
			// Quad
			EASE_F(quad) {return -c *(t/=d)*(t-2) + b;},
			// Cubic
			EASE_F(cubic) {return c*((t=t/d-1)*t*t + 1) + b;},
			// Quart
			EASE_F(quart)  {return -c * ((t=t/d-1)*t*t*t - 1) + b;},
			// Quint
			EASE_F(quint) {return c*(t/=d)*t*t*t*t + b;},
			// Expo
			EASE_F(expo) {return (t==d) ? b+c : c * (-pow(2, -10 * t/d) + 1) + b;},
			// Elastic
			EASE_F(elastic) {
				if (t==0) return b;
				if ((t/=d)==1) return b+c;
				float p=d*.3f;
				float a=c;
				float s=p/4;
				return (a*pow(2,-10*t) * sin( (t*d-s)*(2*PI_VALUE)/p ) + c + b);
			},
			// Circ
			EASE_F(circ) {return c * sqrt(1 - (t=t/d-1)*t) + b;},
			// Bounce
			EASE_F(bounce) {
				float s = 1.70158f;
				float postFix = t/=d;
				return c*(postFix)*t*((s+1)*t - s) + b;
			},
			// Back
			EASE_F(back) {
				float s = 1.70158f;
				return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
			}
		};
		/// Ease IN-OUT functions
		const EaseType inOut {
			// Linear
			EASE_F(linear) {return c * t/d + b;},
			// Sine
			EASE_F(sine) {return -c/2 * (cos(PI_VALUE*t/d) - 1) + b;},
			// Quad
			EASE_F(quad) {
				if ((t/=d/2) < 1)
					return ((c/2)*(t*t)) + b;
				return -c/2 * (((t-2)*(--t)) - 1) + b;
			},
			// Cubic
			EASE_F(cubic) {
				if ((t/=d/2) < 1)
					return c/2*t*t*t + b;
				return c/2*((t-=2)*t*t + 2) + b;
			},
			// Quart
			EASE_F(quart) {
				if ((t/=d/2) < 1)
					return c/2*t*t*t*t + b;
				return -c/2 * ((t-=2)*t*t*t - 2) + b;
			},
			// Quint
			EASE_F(quint) {
				if ((t/=d/2) < 1)
					return c/2*t*t*t*t*t + b;
				return c/2*((t-=2)*t*t*t*t + 2) + b;
			},
			// Expo
			EASE_F(expo) {
				if (t==0) return b;
				if (t==d) return b+c;
				if ((t/=d/2) < 1)
					return c/2 * pow(2, 10 * (t - 1)) + b;
				return c/2 * (-pow(2, -10 * --t) + 2) + b;
			},
			// Elastic
			EASE_F(elastic) {
				if (t==0) return b;
				if ((t/=d/2)==2) return b+c;
				float p=d*(.3f*1.5f);
				float a=c;
				float s=p/4;
				if (t < 1) {
					float postFix =a*pow(2,10*(t-=1));
					return -.5f*(postFix* sin( (t*d-s)*(2*PI_VALUE)/p )) + b;
				}
				float postFix =  a*pow(2,-10*(t-=1));
				return postFix * sin( (t*d-s)*(2*PI_VALUE)/p )*.5f + c + b;
			},
			// Circ
			EASE_F(circ) {
				if ((t/=d/2) < 1)
					return -c/2 * (sqrt(1 - t*t) - 1) + b;
				return c/2 * (sqrt(1 - t*(t-=2)) + 1) + b;
			},
			// Bounce
			EASE_F(bounce) {
				float s = 1.70158f;
				if ((t/=d/2) < 1)
					return c/2*(t*t*(((s*=(1.525f))+1)*t - s)) + b;
				float postFix = t-=2;
				return c/2*((postFix)*t*(((s*=(1.525f))+1)*t + s) + 2) + b;
			},
			// Back
			EASE_F(back) {
				float s = 1.70158f;
				if ((t/=d/2) < 1)
					return c/2*(t*t*(((s*=(1.525f))+1)*t - s)) + b;
				float postFix = t-=2;
				return c/2*((postFix)*t*(((s*=(1.525f))+1)*t + s) + 2) + b;
			}
		};
		/// Ease OUT-IN functions
		#define OUTIN_F(TYPE)\
			EASE_F(TYPE) {\
				if ((t/d) < 0.5)\
					return this->out.TYPE(t*2, b, c/2, d);\
				return this->in.TYPE((t-0.5)*2, b+c/2, c, d);\
			}
		const EaseType outIn {
			// Linear
			EASE_F(linear) {return c * t/d + b;},
			// Sine
			OUTIN_F(sine),
			// Quad
			OUTIN_F(quad),
			// Cubic
			OUTIN_F(cubic),
			// Quart
			OUTIN_F(quart),
			// Quint
			OUTIN_F(quint),
			// Expo
			OUTIN_F(expo),
			// Elastic
			OUTIN_F(elastic),
			// Circ
			OUTIN_F(circ),
			// Bounce
			OUTIN_F(bounce),
			// Back
			OUTIN_F(back)
		};
		#undef OUTIN_F
		/// List accessing
		EaseType const& operator[](String const& type) const {
			CASE_FUNC(in);
			CASE_FUNC(out);
			CASE_FUNC(inOut);
			CASE_FUNC(outIn);
			return out;
		}
	};

	const Easing ease;

	#undef E_FUNC
	#undef EASE_F
	#undef PI_VALUE

	#pragma GCC diagnostic pop

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
		EaseFunc tweenStep = ease.in.linear;

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
		Tween(T from, T to, unsigned long step, EaseFunc tweenStep, bool manual = false) {
			setInterpolation(from, to, step, tweenStep);
			if (!manual)
				tweenList.push_back(&_yield);
			this->manual = manual;
		}

		/// Targeted Constructor.
		Tween(T from, T to, unsigned long step, EaseFunc tweenStep, T* targetVar, bool manual = false) {
			setInterpolation(from, to, step, tweenStep, targetVar);
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
			other.tweenStep,
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
					factor = tweenStep(step, 0.0f, 1.0f, stop);
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
		Tween<T>& setInterpolation(T from, T to, unsigned long step, EaseFunc tweenStep) {
			isFinished = false;
			this->step = 0;
			this->from = from;
			this->to = to;
			stop = step;
			*value = from;
			this->tweenStep = tweenStep;
			factor = 0.0f;
			return *this;
		}

		/// Sets the interpolation with a target.
		Tween<T>& setInterpolation(T from, T to, unsigned long step, EaseFunc tweenStep, T* targetVar) {
			value = targetVar;
			isFinished = false;
			this->step = 0;
			this->from = from;
			this->to = to;
			stop = step;
			*value = from;
			this->tweenStep = tweenStep;
			factor = 0.0f;
			return *this;
		}

		/// Sets the interpolation to a new value, while maintaining the easing function.
		Tween<T>& reinterpolateTo(T to) {
			paused = false;
			setInterpolation(*value, to, step, tweenStep);
			return *this;
		}

		/// Sets the interpolation to a new value, while maintaining the easing dunction.
		Tween<T>& reinterpolate(T from, T to) {
			paused = false;
			setInterpolation(from, to, step, tweenStep);
			return *this;
		}

		/// Sets the interpolation to a new value, while maintaining the easing function.
		Tween<T>& reinterpolateTo(T to, unsigned long step) {
			paused = false;
			setInterpolation(*value, to, step, tweenStep);
			return *this;
		}

		/// Sets the interpolation to a new value, while maintaining the easing dunction.
		Tween<T>& reinterpolate(T from, T to, unsigned long step) {
			paused = false;
			setInterpolation(from, to, step, tweenStep);
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
