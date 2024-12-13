#ifndef CTL_EX_COLLISION_COLLISION2D_WITHINBOUNDS_H
#define CTL_EX_COLLISION_COLLISION2D_WITHINBOUNDS_H

#include "bounds.hpp"

// Based off of https://winter.dev/articles/gjk-algorithm

// TODO: Document this

CTL_EX_NAMESPACE_BEGIN

/// @brief Dimension-agnostic collision implementations.
namespace Collision::Impl {

/// @brief Implementation of the Gilbert-Johnson-Keerthi (GJK) algorithm for collision detection.
namespace GJK {
	namespace {
		using Math::Vector;
	}

	template<usize D>
	struct Simplex {
		static_assert(D > 0 && D < 5);

		Simplex(): count(0) {}

		Simplex(List<VectorType> const& ps): Simplex() {
			for (;count < ps.size() && count < MAX_POINTS; ++count)
				points[count] = ps[count];
		}

		constexpr usize DIMENSION	= D;
		constexpr usize MAX_POINTS	= DIMENSION+1;

		using VectorType = Vector<DIMENSION>;
		
		constexpr Iterator<VectorType>	begin() const	{return points.begin();							}
		constexpr Iterator<VectorType>	end() const		{return points.end() - (MAX_POINTS - count);	}
		constexpr VectorType*			data() const	{return points.data();							}
		constexpr usize					size() const	{return count;									}

		constexpr VectorType operator[](ssize const index) const {
			if (count == 0)
				throw OutOfBoundsException("Simplex is empty!");
			return points[index];
		}

		constexpr Simplex& pushFront(VectorType const& vec) {
			points = {vec, points[0], points[1], points[2]};
			if (count < MAX_POINTS)
				++count;
			return *this;
		}

		constexpr bool makeNext(VectorType& direction) {
			switch (count) {
				case 2: return line(direction);
				case 3: return triangle(direction);
				case 4: return tetrahedron(direction);
			}
			return false;
		}

		constexpr static bool same(VectorType const& direction, VectorType const& ao) {
			return dot(direction, ao) > 0;
		}

	private:
		Array<VectorType, D+1>	points;
		usize					count;

		constexpr bool line(VectorType& direction) {
			VectorType a = points[0];
			VectorType b = points[1];
			VectorType ab = b - a;
			VectorType ao =   - a;
			if (same(ab, ao))
				direction = ab.cross(ao).cross(ab);
			else {
				points = { a, 0, 0, 0 };
				direction = ao;
			}
			return DIMENSION == 1;
		}

		constexpr bool triangle(VectorType& direction) {
			VectorType a = points[0];
			VectorType b = points[1];
			VectorType c = points[2];
			VectorType ab = b - a;
			VectorType ac = c - a;
			VectorType ao =   - a;
			VectorType abc = ab.cross(ac);
			if (same(abc.cross(ac), ao)) {
				if (same(ac, ao)) {
					points = {a, c, 0, 0};
					direction = ac.cross(ao).cross(ac);
				} else {
					points = {a, b, 0, 0};
					return line({a, b}, direction);
				}
			} else if (same(ab.cross(abc), ao)) {
				points = {a, b, 0, 0};
				return line({a, b}, direction);
			} else if (same(abc, ao)) {
				direction = abc;
			} else {
				points = {a, c, b, 0};
				direction = -abc;
			}
			return DIMENSION == 2;
		}

		constexpr bool tetrahedron(VectorType& direction) {
			VectorType a = points[0];
			VectorType b = points[1];
			VectorType c = points[2];
			VectorType d = points[3];
			VectorType ab = b - a;
			VectorType ac = c - a;
			VectorType ad = d - a;
			VectorType ao =   - a;
			VectorType abc = ab.cross(ac);
			VectorType acd = ac.cross(ad);
			VectorType adb = ad.cross(ab);
			if (same(abc, ao)) {
				points = {a, b, c, 0};
				return triangle({a, b, c}, direction);
			}
			if (same(acd, ao)) {
				points = {a, c, d, 0};
				return triangle({a, c, d}, direction);
			}
			if (same(adb, ao)) {
				points = {a, d, b, 0};
				return triangle({a, d, b}, direction);
			}
			return DIMENSION == 3;
		}
	};

	template<usize D>
	constexpr Vector<D> furthest(List<Vector<D>> const& points, Vector<D> const& direction) {
		TVec  maxPoint;
		float maxDistance = NumberLimit<float>::LOWEST;
		for (TVec const& vertex: points) {
			float distance = vertex.dot(direction);
			if (distance > maxDistance) {
				maxDistance = distance;
				maxPoint = vertex;
			}
		}
		return maxPoint;
	}

	template<usize D>
	constexpr Vector<D> support(
		List<Vector<D>> const& a,
		List<Vector<D>> const& b,
		Vector<D> const& direction
	) {
		return furthest(a, direction) - furthest(b, -direction);
	}

	template<usize D>
	constexpr bool check(
		List<Vector<D>> const& a,
		List<Vector<D>> const& b
	) {
		using VectorType = Vector<D>;
		VectorType sup = support(a, b, VectorType(1, 0));
		Simplex<D> pts;
		pts.pushFront(sup);
		VectorType d = -sup;
		while (true) {
			sup = support(a, b, d);
			if (sup.dot(d) <= 0)
				return false;
			pts.pushFront(sup);
			if (pts.next(direction)) {
				return true;
			}
		}
	}
}

}

CTL_EX_NAMESPACE_END

#endif // CTL_EX_COLLISION_COLLISION2D_WITHINBOUNDS_H