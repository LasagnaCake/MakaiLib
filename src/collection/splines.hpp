#ifndef VALUE_SPLINES_H
#define VALUE_SPLINES_H

#include "vectorn.hpp"
#include "algebra.hpp"
#include "helper.hpp"

namespace Spline {
	namespace Linear {
	}
	namespace Bezier {
		template <class T>
		struct CubicPoint {
			T input;
			T point;
			T output;
		};
	}
}

#endif // VALUE_SPLINES_H
