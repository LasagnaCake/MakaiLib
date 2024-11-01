#ifndef CTL_EX_COLLISION_COLLISION2D_BOUNDS_H
#define CTL_EX_COLLISION_COLLISION2D_BOUNDS_H

#include "../../../ctl/ctl.hpp"
#include "../../math/vector.hpp"

CTL_EX_NAMESPACE_BEGIN

namespace Collision::C2D {
	template<size_t UUID> struct Bounds;

	template<usize S> struct Shape;
}

namespace Type::Ex::Collision::C2D {
	template<class T>
	concept Collidable = CTL::Type::Subclass<T, CTL::Ex::Collision::C2D::Bounds<T::ID>>;
	
	template<class T>
	concept Shape = requires {
		T::SIZE;
	} && ::CTL::Type::Equal<T, CTL::Ex::Collision::C2D::Shape<T::SIZE>>;
}

namespace Collision::C2D {
	namespace {
		using
			Math::angleTo,
			Math::center,
			Math::Transform2D,
			Math::Vector2
		;
	}

	template<size_t UUID>
	struct Bounds {
		constexpr static size_t ID = UUID;
		typedef Bounds<ID - 1> Previous;
		typedef Bounds<ID + 1> Next;

		constexpr Bounds()	{}
		constexpr ~Bounds()	{}
	};

	template<typename T>
	struct FollowingType;

	template<Type::Ex::Collision::C2D::Collidable T>
	struct FollowingType<T> {
		typedef Bounds<T::Next::ID> Type;
	};

	template<>
	struct FollowingType<void> {
		typedef Bounds<0> Type;
	};

	template<typename T>
	using Follows = typename FollowingType<T>::Type;

	struct Box: Follows<void> {
		using Bounds::Bounds;

		constexpr Box(
			Vector2 const& min,
			Vector2 const& max
		):
			min(min),
			max(max) {}

		constexpr Box(Box const& other):
			Box(
				other.min,
				other.max
			) {}

		Vector2 min;
		Vector2 max;
	};

	struct Circle: Follows<Box> {
		using Bounds::Bounds;

		constexpr Circle(
			Vector2 const& position,
			Vector2 const& radius = 1,
			float rotation = 0
		):
			position(position),
			radius(radius),
			rotation(rotation) {}

		constexpr Circle(Circle const& other):
			Circle(
				other.position,
				other.radius,
				other.rotation
			) {}

		Vector2 position;
		Vector2 radius = 1;
		float rotation = 0;
	};

	struct Capsule: Follows<Circle> {
		using Bounds::Bounds;

		constexpr Capsule(
			Vector2 const& position,
			Vector2 const& width = 1,
			float const& length = 0,
			float const& angle = 0,
			float const& rotation = 0
		):
			position(position),
			width(width),
			length(length),
			angle(angle),
			rotation(rotation) {}

		constexpr Capsule(Capsule const& other):
			Capsule(
				other.position,
				other.width,
				other.length,
				other.angle,
				other.rotation
			) {}

		Vector2 position;
		Vector2 width = 1;
		float length = 1;
		float angle = 0;
		float rotation = 0;
	};

	struct Ray: Follows<Capsule> {
		using Bounds::Bounds;

		constexpr Ray(
			Vector2 const& position,
			float const& length = 1,
			float const& angle = 0
		):
			position(position),
			length(length),
			angle(angle) {}

		constexpr Ray(Ray const& other):
			Ray(
				other.position,
				other.length,
				other.angle
			) {}

		Vector2 position;
		float length = 1;
		float angle = 0;
	};

	typedef Shape<0> Figure;

	template<>
	struct Shape<0>: Follows<Ray> {
		constexpr static usize SIZE = 0;

		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wpedantic"
		using PointArray = Decay::AsType<Vector2[0]>;
		#pragma GCC diagnostic pop

		constexpr ~Shape() {
			if (points) delete[] points;
		}

		constexpr Shape(): points(nullptr), count(0) {}

		constexpr Shape(usize const& size):
			points(new Vector2[size]),
			count(size) {
			for (usize i = 0; i < count; ++i)
				points[i] = 0;
		}

		template<usize S>
		constexpr Shape(
			Transform2D const& trans,
			Decay::AsType<Vector2[S]> const& points
		): Shape(S) {
			this->trans = trans;
			for (usize i = 0; i < count; ++i)
				this->points[i] = points[i];
		}

		constexpr Shape(
			Transform2D const& trans,
			Vector2* const& points,
			usize const& size
		): Shape(size) {
			this->trans = trans;
			for (usize i = 0; i < count; ++i)
				this->points[i] = points[i];
		}

		template<usize S>
		constexpr Shape(Shape<S> const& other):
			Shape(
				other.trans,
				other.points,
				other.size()
			) {}

		template<usize S>
		constexpr operator Shape<S>() const {return Figure(data(), size());}

		constexpr Vector2* data() const	{return points;	}
		constexpr usize size() const	{return SIZE;	}

		constexpr Vector2* cbegin() const	{return data();				}
		constexpr Vector2* cend() const		{return data() + size();	}

		constexpr Vector2* begin() const	{return cbegin();			}
		constexpr Vector2* end() const		{return cend();				}

		Transform2D trans;

	private:
		Vector2* points = nullptr;

		usize count = 0;
	};

	template<usize S>
	struct Shape: Follows<Ray> {
		static_assert(S > 0);

		constexpr static usize SIZE = S;

		typedef Decay::AsType<Vector2[SIZE]> PointArray;

		constexpr ~Shape() {}

		constexpr Shape() {}

		constexpr Shape(
			Transform2D const& trans,
			PointArray const& points
		): trans(trans) {
			this->trans = trans;
			for (usize i = 0; i < SIZE; ++i)
				this->points[i] = points[i];
		}
		constexpr operator Figure() const {return Figure(trans, data(), size());}

		constexpr Vector2* data() const	{return points;	}
		constexpr usize size() const	{return SIZE;	}

		constexpr Vector2* cbegin() const	{return data();				}
		constexpr Vector2* cend() const		{return data() + size();	}

		constexpr Vector2* begin() const	{return cbegin();			}
		constexpr Vector2* end() const		{return cend;				}

		Transform2D trans;

	private:
		constexpr Shape(
			Transform2D const& trans,
			Vector2* const& points,
			usize const& size
		): trans(trans) {
			const usize sz = (size < SIZE) ? size : SIZE;
			for (usize i = 0; i < sz; ++i)
				this->points[i] = points[i];
		}

		constexpr Shape(Figure const& other):
			Shape(
				other.trans,
				other.points,
				other.size()
			) {}

		PointArray points = {0};
	};

	typedef Shape<3> Triangle;
	typedef Shape<4> Quad;

	struct Polygon: Follows<Figure> {
		constexpr Polygon(
			Transform2D const& trans,
			List<Triangle> const& triangles
		):
			trans(trans),
			triangles(triangles) {}

		constexpr Polygon(Polygon const& other):
			Polygon(
				other.trans,
				other.triangles
			) {}

		Transform2D trans;
		List<Triangle> triangles;
	};

	union CollisionData {
		Box		box;
		Circle	circle;
		Capsule	capsule;
		Ray		ray;
		Figure	shape;
		Polygon	polygon;

		constexpr CollisionData()	{}

		constexpr CollisionData(Box const& value):		box(value)		{}
		constexpr CollisionData(Circle const& value):	circle(value)	{}
		constexpr CollisionData(Capsule const& value):	capsule(value)	{}
		constexpr CollisionData(Ray const& value):		ray(value)		{}
		constexpr CollisionData(Figure const& value):	shape(value)	{}
		constexpr CollisionData(Polygon const& value):	polygon(value)	{}

		constexpr CollisionData(CollisionData const& value) {
			// Trivially-predictable nightmares...
			MX::memcpy((void*)this, (void*)&value, sizeof(CollisionData));
		}

		constexpr ~CollisionData()	{}
	};

	enum class CollisionType {
		CT_NULL		= -1,
		CT_BOX		= Box::ID,
		CT_CIRCLE	= Circle::ID,
		CT_CAPSULE	= Capsule::ID,
		CT_RAY		= Ray::ID,
		CT_SHAPE	= Figure::ID,
		CT_POLYGON	= Polygon::ID
	};

	struct CollisionShape {
		constexpr CollisionShape(): shape(CollisionType::CT_NULL) {}

		template<Type::Ex::Collision::C2D::Collidable T>
		constexpr CollisionShape(T const& bounds): data(bounds), shape((CollisionType)T::ID) {}

		constexpr ~CollisionShape()	{}

		constexpr CollisionData const& value() const	{return data;	}
		constexpr CollisionType type() const			{return shape;	}

		template<CTL::Type::Equal<Box> T>			constexpr Box		asType() const	{return data.box;			}
		template<CTL::Type::Equal<Circle> T>		constexpr Circle	asType() const	{return data.circle;		}
		template<CTL::Type::Equal<Capsule> T>		constexpr Capsule	asType() const	{return data.capsule;		}
		template<CTL::Type::Equal<Ray> T>			constexpr Ray		asType() const	{return data.ray;			}
		template<CTL::Type::Equal<Polygon> T>		constexpr Polygon	asType() const	{return data.polygon;		}
		template<Type::Ex::Collision::C2D::Shape T>	constexpr T			asType() const	{return data.shape;			}

		template<Type::Ex::Collision::C2D::Collidable T>
		constexpr explicit operator T() const	{return asType<T>();	}

	private:
		CollisionData	data;
		CollisionType	shape;

		constexpr CollisionShape(CollisionShape const& other): data(other.data), shape(other.shape) {}
	};
}

CTL_EX_NAMESPACE_END

#endif // CTL_EX_COLLISION_COLLISION2D_BOUNDS_H
