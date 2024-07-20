#ifndef COLLISION_COLLISION2D_AREA_H
#define COLLISION_COLLISION2D_AREA_H

#include "bounds.hpp"
#include "withinbounds.hpp"

namespace Collision2D {
	namespace {
		using
			VecMath::angleTo,
			VecMath::center,
			VecMath::Transform2D
		;
	}

	typedef BitMask<uint16, 4> LayerMask;

	enum class Direction {
		CD_NONE,
		CD_FORWARDS,
		CD_BACKWARDS,
		CD_BOTH
	};

	constexpr Direction asDirection(bool const& forwards, bool const& backwards) {
		return Direction(
			usize(forwards)
		|	(usize(backwards) << 1)
		);
	}

	struct Area {
		CollisionShape	shape;
		bool			enabled	= true;
		LayerMask		affects, affectedBy;

		constexpr Direction colliding(Area const& other) const {
			return check(*this, other);
		}

		/*
			Directions:
				FORWARD:	A --> B
				BACKWARD:	A <-- B
				BOTH:		A <-> B
		*/
		constexpr static Direction check(Area const& a, Area const& b) {
			if (
				a.enabled
			&&	b.enabled
			) {
				Direction dir = asDirection(
					a.affects.match(b.affectedBy).overlap(),
					b.affectedBy.match(a.affects).overlap()
				);
				if (
					dir != Direction::CD_NONE
				&&	withinBounds(a.shape, b.shape)
				) return dir;
				return Direction::CD_NONE;
			}
		}
	};
}

#endif // COLLISION_COLLISION2D_AREA_H
