#ifndef VALUE_SPLINES_H
#define VALUE_SPLINES_H

#include "vectorn.hpp"
#include "algebra.hpp"
#include "helper.hpp"

namespace Spline {
	template<class T>
	class Linear {
	public:
		List<T> points;

		constexpr Linear() {}

		constexpr Linear(List<T> const& ps) {
			points = ps;
		}

		constexpr Linear(Arguments<T> const& ps) {
			for (T& p: ps)
				points.push_back(p);
		}

		template<size_t N>
		constexpr Linear(const T (&ps)[N]) {
			for (T& p: ps)
				points.push_back(p);
		}

		constexpr T interpolate(float by) {
			by = Math::clamp<float>(by, 0, 1);
			if (by == 1.0) return points.end();
			size_t curp = Math::floor(by * points.size());
			return Math::lerp(points[curp], points[curp+1], by);
		}
	};

	namespace Bezier {
		template <Math::Operatable T, size_t N>
		struct Section {
			T points[N];
		};

		template <Math::Operatable T, size_t N>
		using SectionList = List<Section<T, N>>;

		template<Math::Operatable T, size_t N>
		class Spline {
		public:
			constexpr Spline() {}

			constexpr Spline(SectionList<T, N> const& secs) {
				sections = secs;
			}

			constexpr Spline(Arguments<Section<T, N>> const& secs) {
				sections.reserve(N);
				for (Section<T, N>& s: secs)
					sections.push_back(s);
			}

			template <size_t P>
			constexpr Spline(const T (&points)[P][N]) {
				for SSRANGE(i, 0, P) {
					Section<T, N> sec;
					for SSRANGE(j, 0, N)
						sec.points[j] = points[i][j];
					sections.push_back(sec);
				}
			}

			template <size_t P>
			constexpr Spline(const T (&points)[P]) {
				static_assert(P % N == 0, "Point count is not a multiple of N!");
				for RANGE(i, 0, P, N) {
					Section<T, N> sec;
					for SSRANGE(j, 0, N)
						sec.points[j] = points[i+j];
					sections.push_back(sec);
				}
			}

			SectionList<T, N> sections;

			constexpr T interpolate(float by) {
				by = Math::clamp<float>(by, 0, 1);
				if (by == 1.0) return sections.end()[0];
				size_t sec = Math::floor(by * sections.size());
				return lerpSection(sections[sec], sections[sec+1].points[0], by);
			}

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

		template<Math::Operatable T> using Quadratic	= Spline<T, 2>;
		template<Math::Operatable T> using Cubic		= Spline<T, 3>;
		template<Math::Operatable T> using Quartic		= Spline<T, 4>;
		template<Math::Operatable T> using Quintic		= Spline<T, 5>;

		namespace Hermite {
			template<Math::Operatable T>
			struct Section {
				T position;
				T velocity;
			};

			template<typename T>
			using SectionList = List<Section<T>>;

			template<Math::Operatable T>
			class Spline {
			public:
				List<Section<T>> sections;

				constexpr T interpolate(float by) {
					by = Math::clamp<float>(by, 0, 1);
					if (by == 1.0) return sections.end().position;
					size_t sec = Math::floor(by * sections.size());
					return lerpSection(sections[sec], sections[sec+1], by);
				}

			private:
				constexpr T lerpSection(Section<T> const& sec, Section<T> const& next, float const& by) {
					T const pos[2] = {
						sec.position + sec.velocity,
						next.position - next.velocity
					};
					T const p1[3] = {
						Math::lerp(sec.position, pos[0], T(by)),
						Math::lerp(pos[0], pos[1], T(by)),
						Math::lerp(pos[1], next.position, T(by))
					};
					T const p2[2] = {
						Math::lerp(p1[0], p1[1], T(by)),
						Math::lerp(p1[1], p1[2], T(by))
					};
					return Math::lerp(p2[0], p2[1], T(by));
				}
			};
		}
	}
}

#endif // VALUE_SPLINES_H
