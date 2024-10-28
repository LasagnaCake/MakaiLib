#ifndef CTL_EX_COLLISION_COLLISION2D_AREA_H
#define CTL_EX_COLLISION_COLLISION2D_AREA_H

#include "bounds.hpp"
#include "withinbounds.hpp"

CTL_EX_NAMESPACE_BEGIN

namespace Collision::C2D {
	namespace {
		using
			Math::angleTo,
			Math::center,
			Math::Transform2D,
			Math::Vector2
		;
	}

	typedef BitMask<uint16, 4> LayerMask;

	enum class Direction {
		CD_NONE			= 0b00,
		CD_FORWARDS		= 0b01,
		CD_BACKWARDS	= 0b10,
		CD_BOTH			= 0b11
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

CTL_EX_NAMESPACE_END

#endif // CTL_EX_COLLISION_COLLISION2D_AREA_H
