#ifndef TWEEN_TWEENABLE_H
#define TWEEN_TWEENABLE_H

#include "event.hpp"
#include "algebra.hpp"
#include "vectorn.hpp"
#include "conceptual.hpp"
#include <vector>
#include <functional>
#include <string>

#define CASE_FUNC(F_NAME) if ( type == #F_NAME ) return F_NAME

namespace Tween{
	namespace {
		using std::function, std::vector, std::string, TypedEvent::Signal;
		vector<const function<void(float)>*> tweenList;
		typedef function<float(float, float, float, float)> _EaseFunc;
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
			_EaseFunc& operator[](string type) {
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

	#define $easing					[](float step, float from, float to, float stop) -> float
	#define $$EFUNC					[](float t, float b, float c, float d) -> float
	#define PI_VALUE Math::pi
	#define EASE_F(TYPE) .TYPE = $$EFUNC
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
			// sine
			EASE_F(sine) {return -c * cos(t/d * (PI_VALUE/2)) + c + b;},
			// quad
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
			// circ
			EASE_F(circ) {return -c * (sqrt(1 - (t/=d)*t) - 1) + b;},
			// bounce
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
			// sine
			EASE_F(sine) {return c * sin(t/d * (PI_VALUE/2)) + b;},
			// quad
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
			// circ
			EASE_F(circ) {return c * sqrt(1 - (t=t/d-1)*t) + b;},
			// bounce
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
			// sine
			EASE_F(sine) {return -c/2 * (cos(PI_VALUE*t/d) - 1) + b;},
			// quad
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
			// circ
			EASE_F(circ) {
				if ((t/=d/2) < 1)
					return -c/2 * (sqrt(1 - t*t) - 1) + b;
				return c/2 * (sqrt(1 - t*(t-=2)) + 1) + b;
			},
			// bounce
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
		const EaseType outIn {
			// Linear
			EASE_F(linear) {return c * t/d + b;},
		};
		/// List accessing
		const EaseType& operator[](string type) {
			CASE_FUNC(in);
			CASE_FUNC(out);
			CASE_FUNC(inOut);
			CASE_FUNC(outIn);
		}
	};

	const Easing ease;

	#undef $$EFUNC
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
				tweenList.push_back(&yield);
		}

		/// Targetless Constructor.
		Tween(T from, T to, float step, EaseFunc tweenStep, bool manual = false) {
			setInterpolation(from, to, step, tweenStep);
			if (!manual)
				tweenList.push_back(&yield);
		}

		/// Targeted Constructor.
		Tween(T from, T to, float step, EaseFunc tweenStep, float* targetVar, bool manual = false) {
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
		const Signal<float> yield = [&](float delta = 1) {
			this->_yield(delta);
		};

		/// Sets the interpolation.
		Tween<T>& setInterpolation(T from, T to, float step, EaseFunc tweenStep) {
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
		Tween<T>& setInterpolation(T from, T to, float step, EaseFunc tweenStep, T* targetVar) {
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
		Tween<T>& reinterpolate(T to) {
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
		Tween<T>& reinterpolate(T to, float step) {
			paused = false;
			setInterpolation(*value, to, step, tweenStep);
			return *this;
		}

		/// Sets the interpolation to a new value, while maintaining the easing dunction.
		Tween<T>& reinterpolate(T from, T to, float step) {
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
		Tween<T>& setStep(float step) {
			step--;
			this->step = step > stop ? stop : step;
			yield(1);
			return *this;
		}

		/// Sets the current tween step count.
		Tween<T>& setStepCount(float stop) {
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
		float factor = 1.0f;

		void _yield(float delta = 1.0f) {
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

		/// Whether the tween is finished.
		bool isFinished;

		/// The current tween step.
		float step;

		/// The amount of steps to be executed.
		float stop;

		/// The tween's default target.
		T defaultVar;
	};
}

#define $twn Tween::

#undef CASE_FUNC

#endif // TWEEN_TWEENABLE_H
