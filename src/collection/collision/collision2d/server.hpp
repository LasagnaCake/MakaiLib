#ifndef COLLISION_COLLISION2D_SERVER_H
#define COLLISION_COLLISION2D_SERVER_H

#include "area.hpp"
#include "../../definitions.hpp"
#include "../../referential.hpp"

namespace Collision2D {
	namespace {
		using
			VecMath::angleTo,
			VecMath::center,
			VecMath::Transform2D
		;
	}

	template<usize>
	struct CollisionServer {
		typedef Functor<void(Area&)> CollisionEvent;

		struct Collider: Area {
			constexpr ~Collider() {
				parent->unbind(this);
			}

			constexpr Collider() {}
			constexpr Collider(Area const& other): Area{other} {}

			constexpr Collider(Collider const& other)	= delete;
			constexpr Collider(Collider&& other)		= delete;

			constexpr void handleCollision(Collider const& other) {
				switch(colliding(other)) {
					default:
					case Direction::CD_NONE: return;
					case Direction::CD_FORWARDS:
						other.onCollision(*this);
					return;
					case Direction::CD_BACKWARDS:
						onCollision(other);
					return;
					case Direction::CD_BOTH:
						other.onCollision(*this);
						onCollision(other);
					return;
				}
			}

			CollisionEvent onCollision = []{};

		private:
			template<usize> friend class ::Collision2D::CollisionServer;

			CollisionServer* parent;
		};

		typedef Instance<Collider>	ColliderInstance;
		typedef Handle<Collider>	ColliderHandle;

		constexpr CollisionServer() {}

		constexpr static ColliderInstance createInstance() {
			ColliderInstance instance = new Collider();
			bind(instance);
			return instance;
		}

		constexpr static ColliderInstance createInstance(Area const& shape) {
			ColliderInstance instance = new Collider(shape);
			bind(instance);
			return instance;
		}

		constexpr static void check(Collider const& area, LayerMask const& layers) {
			if (!area.affects.match(layers).overlap()|| !area.enabled) return;
			for (Collider* c : colliders)
				if (c->enabled && c->affectedBy.match(layers).overlap())
					area.handleCollision(*c);
		}

		constexpr static void check(Collider const& area) {
			for (Collider* c : colliders)
				area.handleCollision(*c);
		}

		static void process() {
			usize const stop = colliders.size();
			for SRANGE(start, 1, stop) {
				Collider* c = colliders[start-1];
				for SRANGE(i, start, stop)
					c->handleCollision(colliders[start]);
			}
		}

	private:
		constexpr static void bind(Collider* const& collider) {
			colliders.push_back(collider);
			collider->parent = this;
		}

		constexpr static void unbind(Collider* const& collider) {
			ERASE_IF(colliders, elem == collider);
		}

		friend class Collider;

		static List<Collider*> colliders;
	};

	typedef CollisionServer<0> GlobalServer;
}

#endif // COLLISION_COLLISION2D_SERVER_H
