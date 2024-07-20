#ifndef COLLISION_COLLISION2D_SERVER_H
#define COLLISION_COLLISION2D_SERVER_H

#include "area.hpp"

namespace Collision2D {
	namespace {
		using
			VecMath::angleTo,
			VecMath::center,
			VecMath::Transform2D
		;
	}

	struct CollisionServer {

		struct CollisionObject {
			Area area;

			void bind() {
			}

			void unbind() {
			}
		};

	private:
		static List<CollisionObject> objects;
	};
}

#endif // COLLISION_COLLISION2D_SERVER_H
