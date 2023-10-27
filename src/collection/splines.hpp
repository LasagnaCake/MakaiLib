#ifndef VALUE_SPLINES_H
#define VALUE_SPLINES_H

#include "vectorn.hpp"
#include "algebra.hpp"
#include "helper.hpp"

namespace Spline {
	namespace Linear {
	}

	namespace Bezier {
		template <Math::Operatable T, size_t N>
		struct Section {
			T points[N];
		};

		template <Math::Operatable T, size_t N>
		using SectionList = List<Section<T, N>>;

		template<Math::Operatable T, size_t N>
		class Bezieroid {
		public:
			constexpr Bezieroid() {}

			constexpr Bezieroid(SectionList<T, N> const& secs) {
				sections.reserve(N);
				for (Section<T, N>& s: secs)
					sections.push_back(s);
			}

			constexpr Bezieroid(Arguments<Section<T, N>> const& secs) {
				sections.reserve(N);
				for (Section<T, N>& s: secs)
					sections.push_back(s);
			}

			template <size_t P>
			constexpr Bezieroid(const T (&points)[P][N]) {
				for SSRANGE(i, 0, P) {
					Section<T, N> sec;
					for SSRANGE(j, 0, N)
						sec.points[j] = points[i][j];
					sections.push_back(sec);
				}
			}

			template <size_t P>
			constexpr Bezieroid(const T (&points)[P]) {
				static_assert(P % N == 0, "Point count is not a multiple of N!");
				for RANGE(i, 0, P, N) {
					Section<T, N> sec;
					for SSRANGE(j, 0, N)
						sec.points[j] = points[i+j];
					sections.push_back(sec);
				}
			}

			SectionList<T, N> sections;

			constexpr T lerp(float by) {
				by = Math::clamp<float>(by, 0, 1);
				size_t sec = Math::floor(by * sections.size());
				return lerpSection(sections[sec], sections[sec+1].points[0], by);
			}

		protected:

		private:
			template <size_t S>
			constexpr T lerpSection(Section<T, S> const& sec, T const& end, float const& by) {
				if constexpr(S < 2)
					return Math::lerp(sec.points[0], end, T(by));
				else {
					Section<T, S-1> res;
					T newEnd = Math::lerp(sec.points[S-1], end, T(by));
					for (size_t i = 0; i < S-2; i++)
						res.points[i] = Math::lerp(sec.points[S], sec.points[S+1], T(by));
					return lerpSection(res, newEnd, by);
				}
			}
		};

		template<class T>
		using QuadraticBezier = Bezieroid<T, 3>;

		template<class T>
		using CubicBezier = Bezieroid<T, 3>;
	}
}

#endif // VALUE_SPLINES_H
