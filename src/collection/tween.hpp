#ifndef TWEEN_TWEENABLE_H
#define TWEEN_TWEENABLE_H

#include "event.hpp"
#include <vector>
#include <functional>
#include <string>

#define CASE_FUNC(F_NAME) if ( type == #F_NAME ) return F_NAME

namespace Tween{
	namespace {
		using std::function, std::vector, std::string;
		vector<const function<float()>*> tweenList;
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
			_EaseFunc operator[](string type) {
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
	#define $$FUNC function<float()>
	/// Yields all available non-manual tweens.
	void yieldAllTweens() {
		// Loop through tweens and step them
		if (tweenList.size())
			for(const $$FUNC* func : tweenList)
				(*func)();
	}

	#define $easing					[&](float step, float from, float range, float stop) -> float
	#define $$EFUNC					[&](float t, float b, float c, float d) -> float
	#define PI Math::pi
	#define EASE_F(TYPE, CODE) . TYPE = $$EFUNC CODE ,
	/// Easing function template
	typedef _EaseFunc EaseFunc;

	/**
	******************
	*                *
	*  Easing Class  *
	*                *
	******************
	*/
	struct Easing {
		/// Ease IN functions
		EaseType in {
			// Linear
			EASE_F(linear, {return t * b/c + d;})
			// sine
			EASE_F(sine, {return -c * cos(t/d * (PI/2)) + c + b;})
			// quad
			EASE_F(quad, {return c*(t/=d)*t + b;})
			// Cubic
			EASE_F(cubic, {return c*(t/=d)*t*t + b;})
			// Quart
			EASE_F(quart, {return c*(t/=d)*t*t*t + b;})
			// Quint
			EASE_F(quint, {return c*((t=t/d-1)*t*t*t*t + 1) + b;})
			// Expo
			EASE_F(expo, {return (t==0) ? b : c * pow(2, 10 * (t/d - 1)) + b;})
			// Elastic
			EASE_F(elastic, {
				if (t==0) return b;
				if ((t/=d)==1) return b+c;
				float p=d*.3f;
				float a=c;
				float s=p/4;
				float postFix =a*pow(2,10*(t-=1));
				return -(postFix * sin((t*d-s)*(2*PI)/p )) + b;
			})
			// circ
			EASE_F(circ, {return -c * (sqrt(1 - (t/=d)*t) - 1) + b;})
			// bounce
			EASE_F(bounce, {
				float s = 1.70158f;
				return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
			})
			// Back
			EASE_F(back, {
				float s = 1.70158f;
				float postFix = t/=d;
				return c*(postFix)*t*((s+1)*t - s) + b;
			})
		};
		/// Ease OUT functions
		EaseType out {
			// Linear
			EASE_F(linear, {return t * b/c + d;})
			// sine
			EASE_F(sine, {return c * sin(t/d * (PI/2)) + b;})
			// quad
			EASE_F(quad, {return -c *(t/=d)*(t-2) + b;})
			// Cubic
			EASE_F(cubic, {return c*((t=t/d-1)*t*t + 1) + b;})
			// Quart
			EASE_F(quart, {return -c * ((t=t/d-1)*t*t*t - 1) + b;})
			// Quint
			EASE_F(quint, {return c*(t/=d)*t*t*t*t + b;})
			// Expo
			EASE_F(expo, {return (t==d) ? b+c : c * (-pow(2, -10 * t/d) + 1) + b;})
			// Elastic
			EASE_F(elastic, {
				if (t==0) return b;
				if ((t/=d)==1) return b+c;
				float p=d*.3f;
				float a=c;
				float s=p/4;
				return (a*pow(2,-10*t) * sin( (t*d-s)*(2*PI)/p ) + c + b);
			})
			// circ
			EASE_F(circ, {return c * sqrt(1 - (t=t/d-1)*t) + b;})
			// bounce
			EASE_F(bounce, {
				float s = 1.70158f;
				float postFix = t/=d;
				return c*(postFix)*t*((s+1)*t - s) + b;
			})
			// Back
			EASE_F(back, {
				float s = 1.70158f;
				return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
			})
		};
		/// Ease IN-OUT functions
		EaseType inOut {
			// Linear
			EASE_F(linear, {return t * b/c + d;})
			// sine
			EASE_F(sine, {return -c/2 * (cos(PI*t/d) - 1) + b;})
			// quad
			EASE_F(quad, {
				if ((t/=d/2) < 1)
					return ((c/2)*(t*t)) + b;
				return -c/2 * (((t-2)*(--t)) - 1) + b;
			})
			// Cubic
			EASE_F(cubic, {
				if ((t/=d/2) < 1)
					return c/2*t*t*t + b;
				return c/2*((t-=2)*t*t + 2) + b;
			})
			// Quart
			EASE_F(quart, {
				if ((t/=d/2) < 1)
					return c/2*t*t*t*t + b;
				return -c/2 * ((t-=2)*t*t*t - 2) + b;
			})
			// Quint
			EASE_F(quint, {
				if ((t/=d/2) < 1)
					return c/2*t*t*t*t*t + b;
				return c/2*((t-=2)*t*t*t*t + 2) + b;
			})
			// Expo
			EASE_F(expo, {
				if (t==0) return b;
				if (t==d) return b+c;
				if ((t/=d/2) < 1)
					return c/2 * pow(2, 10 * (t - 1)) + b;
				return c/2 * (-pow(2, -10 * --t) + 2) + b;
			})
			// Elastic
			EASE_F(elastic, {
				if (t==0) return b;
				if ((t/=d/2)==2) return b+c;
				float p=d*(.3f*1.5f);
				float a=c;
				float s=p/4;
				if (t < 1) {
					float postFix =a*pow(2,10*(t-=1));
					return -.5f*(postFix* sin( (t*d-s)*(2*PI)/p )) + b;
				}
				float postFix =  a*pow(2,-10*(t-=1));
				return postFix * sin( (t*d-s)*(2*PI)/p )*.5f + c + b;
			})
			// circ
			EASE_F(circ, {
				if ((t/=d/2) < 1)
					return -c/2 * (sqrt(1 - t*t) - 1) + b;
				return c/2 * (sqrt(1 - t*(t-=2)) + 1) + b;
			})
			// bounce
			EASE_F(bounce, {
				float s = 1.70158f;
				if ((t/=d/2) < 1)
					return c/2*(t*t*(((s*=(1.525f))+1)*t - s)) + b;
				float postFix = t-=2;
				return c/2*((postFix)*t*(((s*=(1.525f))+1)*t + s) + 2) + b;
			})
			// Back
			EASE_F(back, {
				float s = 1.70158f;
				if ((t/=d/2) < 1)
					return c/2*(t*t*(((s*=(1.525f))+1)*t - s)) + b;
				float postFix = t-=2;
				return c/2*((postFix)*t*(((s*=(1.525f))+1)*t + s) + 2) + b;
			})
		};
		/// Ease OUT-IN functions
		EaseType outIn {
			// Linear
			EASE_F(linear, {return t * b/c + d;})
		};
		/// List accessing
		EaseType& operator[](string type) {
			CASE_FUNC(in);
			CASE_FUNC(out);
			CASE_FUNC(inOut);
			CASE_FUNC(outIn);
		}
	} ease;
	#undef $$EFUNC
	#undef EASE_F
	#undef PI

	/**
	*****************
	*               *
	*  Tween Class  *
	*               *
	*****************
	*/
	class Tween {
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

#undef CASE_FUNC

#endif // TWEEN_TWEENABLE_H
