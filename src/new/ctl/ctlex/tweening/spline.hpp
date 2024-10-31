#ifndef CTL_EX_TWEENING_SPLINES_H
#define CTL_EX_TWEENING_SPLINES_H

#include "../math/vector.hpp"
#include "../../ctl/math/core.hpp"
#include "../../ctl/container/list.hpp"
#include "../../ctl/container/arguments.hpp"
#include "../../ctl/exnamespace.hpp"

CTL_EX_NAMESPACE_BEGIN

namespace Spline {
	template<CTL::Type::Math::Operatable T>
	struct Splinoid {
		typedef T DataType;

		constexpr virtual T interpolate(float const& by) = 0;
	};
}

namespace Type::Spline {
	template<class T>
	concept Spline = CTL::Type::Subclass<T, Ex::Spline::Splinoid<typename T::DataType>>;
}

namespace Spline {

	template<class T>
	class Linear: public Splinoid<T> {
	public:
		List<T> points;

		constexpr Linear() {}

		constexpr Linear(List<T> const& ps) {
			points = ps;
		}

		constexpr Linear(Arguments<T> const& ps) {
			points.resize(ps.size());
			for (T& p: ps)
				points.pushBack(p);
		}

		template<usize N>
		constexpr Linear(T const(& ps)[N]) {
			points.resize(N);
			for (T& p: ps)
				points.pushBack(p);
		}

		constexpr T interpolate(float by) final {
			by = ::CTL::Math::clamp<float>(by, 0, 1);
			if (by == 1.0) return points.end();
			usize curp = ::CTL::Math::floor(by * points.size());
			return Math::lerp(points[curp], points[curp+1], by);
		}
	};

	namespace Bezier {
		template<CTL::Type::Math::Operatable T, usize N>
		struct Section {
			T points[N];
		};

		template<CTL::Type::Math::Operatable T, usize N>
		using SectionList = List<Section<T, N>>;

		template<CTL::Type::Math::Operatable T, usize N>
		class Spline: public Splinoid<T> {
		public:
			constexpr Spline() {}

			constexpr Spline(SectionList<T, N> const& secs) {
				sections = secs;
			}

			constexpr Spline(Arguments<Section<T, N>> const& secs) {
				sections.resize(secs.size());
				for (Section<T, N>& s: secs)
					sections.pushBack(s);
			}

			template <usize P>
			constexpr Spline(T const(& points)[P][N]) {
				sections.resize(P);
				for (usize i = 0; i < P; ++i) {
					Section<T, N> sec;
					for (usize j = 0; j < N; ++j) 
						sec.points[j] = points[i][j];
					sections.pushBack(sec);
				}
			}

			template <usize P>
			constexpr Spline(T const(& points)[P]) {
				static_assert(P % N == 0, "Point count is not a multiple of N!");
				sections.resize(P/N);
				for (usize i = 0; i < P; i += N) {
					Section<T, N> sec;
					for (usize j = 0; j < N; ++j) 
						sec.points[j] = points[i+j];
					sections.pushBack(sec);
				}
			}

			SectionList<T, N> sections;

			constexpr T interpolate(float by) final {
				by = ::CTL::Math::clamp<float>(by, 0, 1);
				if (by == 1.0) return sections.end()[0];
				usize sec = ::CTL::Math::floor(by * sections.size());
				return lerpSection(sections[sec], sections[sec+1].points[0], by);
			}

		private:
			template<usize S>
			constexpr T lerpSection(Section<T, S> const& sec, T const& end, float const& by) {
				if constexpr(S < 2)
					return ::CTL::Math::lerp(sec.points[0], end, T(by));
				else {
					Section<T, S-1> res;
					T newEnd = Math::lerp(sec.points[S-1], end, T(by));
					for (usize i = 0; i < S-2; ++i)
						res.points[i] = ::CTL::Math::lerp(sec.points[S], sec.points[S+1], T(by));
					return lerpSection(res, newEnd, by);
				}
			}
		};

		template<CTL::Type::Math::Operatable T> using Quadratic	= Spline<T, 2>;
		template<CTL::Type::Math::Operatable T> using Cubic		= Spline<T, 3>;
		template<CTL::Type::Math::Operatable T> using Quartic	= Spline<T, 4>;
		template<CTL::Type::Math::Operatable T> using Quintic	= Spline<T, 5>;
	}

	namespace Hermite {
		template<CTL::Type::Math::Operatable T>
		struct Section {
			T position;
			T velocity;
		};

		template<typename T>
		using SectionList = List<Section<T>>;

		template<CTL::Type::Math::Operatable T>
		class Spline: public Splinoid<T> {
		public:
			constexpr Spline() {}

			constexpr Spline(SectionList<T> const& secs) {
				sections = secs;
			}

			constexpr Spline(Arguments<Section<T>> const& secs) {
				sections.resize(secs.size());
				for (Section<T>& s: secs)
					sections.pushBack(s);
			}

			template <usize P>
			constexpr Spline(Decay::AsType<T const[P][2]> const& points) {
				sections.resize(P);
				for (usize i = 0; i < P; ++i) {
					sections.pushBack(
						Section<T> {
							points[i][0],
							points[i][1]
						}
					);
				}
			}

			template<usize P>
			constexpr Spline(Decay::AsType<T[P]> const& points) {
				sections.resize(P/2);
				static_assert(P % 2 == 0, "Point count is not a multiple of 2!");
				for (usize i = 0; i < P; i += 2) {
					sections.pushBack(
						Section<T> {
							points[i],
							points[i+1]
						}
					);
				}
			}

			SectionList<T> sections;

			constexpr T interpolate(float by) final {
				by = ::CTL::Math::clamp<float>(by, 0, 1);
				if (by == 1.0) return sections.end().position;
				usize sec = ::CTL::Math::floor(by * sections.size());
				return lerpSection(sections[sec], sections[sec+1], by);
			}

		private:
			constexpr T lerpSection(Section<T> const& sec, Section<T> const& next, float const& by) {
				T const pos[2] = {
					sec.position + sec.velocity,
					next.position - next.velocity
				};
				T const p1[3] = {
					::CTL::Math::lerp(sec.position, pos[0], T(by)),
					::CTL::Math::lerp(pos[0], pos[1], T(by)),
					::CTL::Math::lerp(pos[1], next.position, T(by))
				};
				T const p2[2] = {
					::CTL::Math::lerp(p1[0], p1[1], T(by)),
					::CTL::Math::lerp(p1[1], p1[2], T(by))
				};
				return ::CTL::Math::lerp(p2[0], p2[1], T(by));
			}
		};
	}
}

CTL_EX_NAMESPACE_END

#endif // CTL_EX_TWEENING_SPLINES_H
