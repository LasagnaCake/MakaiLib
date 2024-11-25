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

	/// @brief Automatic collision handler.
	/// @tparam Server ID.
	template<usize I>
	struct CollisionServer {
		/// @brief Server ID.
		constexpr static usize ID = I;

		/// @brief Collision event signal. Gets called when a collision event occurs.
		typedef Functor<void(Area&, Direction const&)> CollisionEvent;

		template<usize II>
		using ColliderType = typename CollisionServer<II>::Collider;

		/// @brief Server collision object.
		struct Collider: Area {
			/// @brief Destructor.
			constexpr ~Collider() {
				CollisionServer::unbind(this);
			}

			using Instance	= Instance<Collider>;
			using Handle	= Handle<Collider>;

			/// @brief Copy constructor (deleted). 
			constexpr Collider(Collider const& other)	= delete;
			/// @brief Move constructor (deleted).
			constexpr Collider(Collider&& other)		= delete;

			/// @brief
			///		Checks collision between objects,
			///		and fires the appropriate collision events,
			///		depending on its result.
			/// @param other Collider to check against.
			template<usize II>
			constexpr void process(ColliderType<II> const& other) const {
				switch(colliding(other)) {
					using enum Direction;
					default:
					case CD_NONE: break;
					case CD_FORWARDS:
						other.onCollision(*this, CD_FORWARDS);
					break;
					case CD_BACKWARDS:
						onCollision(other, CD_BACKWARDS);
					break;
					case CD_BOTH:
						other.onCollision(*this, CD_BOTH);
						onCollision(other, CD_BOTH);
					break;
				}
			}

			/// @brief Event to fire on collision.
			CollisionEvent onCollision = []{};

		private:
			/// @brief Empty constructor.
			constexpr Collider()								{CollisionServer::bind(this);}
			/// @brief Constructs the collider from a collision area.
			/// @param other Collision area to construct from.
			constexpr Collider(Area const& other): Area{other}	{CollisionServer::bind(this);}

			template<usize> friend class CollisionServer;
		};

		/// @brief Empty constructor.
		constexpr CollisionServer() {}

		/// @brief Creates a collider in the server.
		/// @return Collider instance.
		constexpr static Instance<Collider> newCollider() {
			Instance<Collider> instance = new Collider();
			return instance;
		}

		/// @brief Creates a collider in the server.
		/// @param area Collision area to construct collider from.
		/// @return Collider instance.
		constexpr static Instance<Collider> newCollider(Area const& area) {
			Instance<Collider> instance = new Collider(area);
			return instance;
		}

		/// @brief Handles collision between a given collider, and a set of layers.
		/// @param area Collider to check.
		/// @param layers Layers to check against.
			template<usize II>
		constexpr static void check(ColliderType<II> const& area, LayerMask const& layers) {
			if (!area.affects.match(layers).overlap()|| !area.enabled) return;
			for (Collider* c : colliders)
				if (c->enabled && c->affectedBy.match(layers).overlap())
					area.process(*c);
		}

		/// @brief Handles for a given collider.
		/// @param area Collider to check.
			template<usize II>
		constexpr static void check(ColliderType<II> const& area) {
			for (Collider* c : colliders)
				area.process(*c);
		}

		/// @brief Processes (and handles) collision for all colliders in the server.
		constexpr static void process() {
			usize const stop = colliders.size();
			for (usize start = 1; start < stop; ++start) {
				Collider* c = colliders[start-1];
				for (usize i = start; i < stop; ++i)
					c->process(colliders[start]);
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

		/// @brief Server collider objects.
		static List<Collider*> colliders;

		void *operator new(usize);
		void operator delete(pointer);
		void *operator new[](usize);
		void operator delete[](pointer);
	};
	/// @brief Default collision server.
	using Server = CollisionServer<0>;
}

CTL_EX_NAMESPACE_END

#endif // CTL_EX_COLLISION_COLLISION2D_SERVER_H
