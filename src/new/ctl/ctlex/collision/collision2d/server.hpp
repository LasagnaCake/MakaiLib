#ifndef CTL_EX_COLLISION_COLLISION2D_SERVER_H
#define CTL_EX_COLLISION_COLLISION2D_SERVER_H

#include "area.hpp"

CTL_EX_NAMESPACE_BEGIN

/// @brief Two-dimensional collision.
namespace Collision::C2D {
	namespace {
		using
			Math::angleTo,
			Math::center,
			Math::Transform2D
		;
	}

	template<usize>
	struct CollisionServer {
		typedef Functor<void(Area&)> CollisionEvent;

		struct Collider: Area {
			constexpr ~Collider() {
				CollisionServer::unbind(this);
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
			template<usize> friend class CollisionServer;

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
			for (usize start = 1; start < stop; ++start) {
				Collider* c = colliders[start-1];
				for (usize i = start; i < stop; ++i)
					c->handleCollision(colliders[start]);
			}
		}

	private:
		constexpr static void bind(Collider* const& collider) {
			colliders.pushBack(collider);
		}

		constexpr static void unbind(Collider* const& collider) {
			colliders.eraseLike(collider);
		}

		friend class Collider;

		static List<Collider*> colliders;
	};

	typedef CollisionServer<0> GlobalServer;
}

CTL_EX_NAMESPACE_END

#endif // CTL_EX_COLLISION_COLLISION2D_SERVER_H
