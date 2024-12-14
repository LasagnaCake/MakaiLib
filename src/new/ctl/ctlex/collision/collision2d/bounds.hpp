#ifndef CTL_EX_COLLISION_COLLISION2D_BOUNDS_H
#define CTL_EX_COLLISION_COLLISION2D_BOUNDS_H

#include "../../../ctl/ctl.hpp"
#include "../../math/vector.hpp"
#include "../gjk.hpp"

// TODO: Update system to use GJK & IBound2D

CTL_EX_NAMESPACE_BEGIN

/// @brief Collision facilities.
namespace Collision {}

/// @brief Two-dimensional collision.
namespace Collision::C2D {
	template<usize UUID> struct Bounded;
}

/// @brief Two-dimensional collision type constraints.
namespace Type::Ex::Collision::C2D {
	/// @brief Type must be a valid collision bound.
	template<class T>
	concept Collidable = CTL::Type::Subclass<T, CTL::Ex::Collision::C2D::Bounded<T::ID>>;
}

/// @brief Two-dimensional collision.
namespace Collision::C2D {
	namespace {
		using
			Math::center,
			Math::Transform2D,
			Math::Vector2
		;
	}

	/// @brief Tas the deriving class as a bounding area of some form.
	template<usize UUID>
	struct Bounded {
		/// @brief ID of the bounding area.
		constexpr static usize ID = UUID;
		/// @brief ID of the bounding area that postceding it.
		typedef Bounded<ID - 1> Previous;
		/// @brief ID of the bounding area that precedes it.
		typedef Bounded<ID + 1> Next;

		/// @brief Empty constructor.
		constexpr Bounded()		{}
		/// @brief Empty destructor.
		constexpr ~Bounded()	{}
	};

	/// @brief Implementations.
	namespace Impl {
		template<typename T>
		struct FollowingType;

		template<Type::Ex::Collision::C2D::Collidable T>
		struct FollowingType<T> {
			typedef Bounded<T::Next::ID> Type;
		};

		template<>
		struct FollowingType<void> {
			typedef Bounded<0> Type;
		};
	}

	using IBound2D = IBound<2>;

	/// @brief Declares the bound as postceding another bound.
	template<typename T>
	using Follows = typename Impl::FollowingType<T>::Type;

	/// @brief Box bound.
	struct Box: Follows<void> {
		using Bounded::Bounded;
		
		/// @brief Constructs a box bound.
		/// @param position Position.
		/// @param size Size.
		constexpr Box(
			Vector2 const& position,
			Vector2 const& size
		):
			position(position),
			size(size) {}

		/// @brief Copy constructor (defaulted).
		constexpr Box(Box const& other)	= default;
		/// @brief Move constructor (defaulted).
		constexpr Box(Box&& other)		= default;
		
		/// @brief Returns the top-left corner of the box.
		/// @return Top-left corner.
		constexpr Vector2 min() const {return position - size;}
		/// @brief Returns the bottom-right corner of the box.
		/// @return Bottom-right corner.
		constexpr Vector2 max() const {return position + size;}

		/// @brief Box position.
		Vector2 position;
		/// @brief Box size.
		Vector2 size;
	};

	/// @brief "Circle" bound.
	/// @details
	///		Not truly a circle - actually an ellipse.
	///
	///		Though, a circle is also technically an ellipse with equal major and minor axes.
	struct Circle: Follows<Box> {
		using Bounded::Bounded;

		/// @brief Constructs a "circle" bound.
		/// @param position Position.
		/// @param radius Radius. By default, it is 1.
		/// @param rotation Rotation. By default, it is 0.
		constexpr Circle(
			Vector2 const& position,
			Vector2 const& radius = 1,
			float rotation = 0
		):
			position(position),
			radius(radius),
			rotation(rotation) {}

		/// @brief Copy constructor (defaulted).
		constexpr Circle(Circle const& other)	= default;
		/// @brief Move constructor (defaulted).
		constexpr Circle(Circle&& other)		= default;

		/// @brief Returns the circle's radius at a given angle.
		/// @param angle Angle to get the radius for.
		/// @return Radius at the given angle.
		constexpr float radiusAt(float const angle) const {
			float as, ac;
			CTL::Math::absincos(angle + rotation, as, ac);
			return (as * radius.x) + (ac * radius.y);
		}

		/// @brief Circle position.
		Vector2 position;
		/// @brief Circle radius.
		Vector2 radius = 1;
		/// @brief Circle rotation.
		float rotation = 0;
	};

	/// @brief "Capsule" bound.
	/// @details
	///		This one is a bit complex.
	///
	///		This is a "stadium-like" ("2D-capsule-like") shape.
	///	
	///		It would be best described as the "convex hull between two equivalent ellipses".
	///		
	///		Also, the ellipses do not rotate with the shape.
	///		Their angles are separate from the capsule's own angle.
	/// @note Based off of https://en.wikipedia.org/wiki/Stadium_(geometry)
	struct Capsule: Follows<Circle> {
		using Bounded::Bounded;

		/// @brief Constructs a capsule bound.
		/// @param position Position.
		/// @param width Width. By default, it is 1.
		/// @param length Length. By default, it is 0.
		/// @param angle Rotation. By default, it is 0.
		/// @param rotation Cap rotation. By default, it is 0.
		constexpr Capsule(
			Vector2 const& position,
			Vector2 const& width = 1,
			float const length = 0,
			float const angle = 0,
			float const rotation = 0
		):
			position(position),
			width(width),
			length(length),
			angle(angle),
			rotation(rotation) {}

		/// @brief Copy constructor (defaulted).
		constexpr Capsule(Capsule const& other)	= default;
		/// @brief Move constructor (defaulted).
		constexpr Capsule(Capsule&& other)		= default;

		/// @brief Capsule position.
		Vector2 position;
		/// @brief Capsule width.
		Vector2 width = 1;
		/// @brief Capsule length.
		float length = 1;
		/// @brief Capsule rotation.
		float angle = 0;
		/// @brief Capsule cap rotation.
		float rotation = 0;
	};

	/// @brief Raycast bound.
	struct Ray: Follows<Capsule> {
		using Bounded::Bounded;

		/// @brief Constructs a raycast bound.
		/// @param position Position.
		/// @param length Length.
		/// @param angle Angle.
		constexpr Ray(
			Vector2 const& position,
			Vector2 const& direction
		):
			position(position),
			direction(direction) {}

		/// @brief Copy constructor (defaulted).
		constexpr Ray(Ray const& other)	= default;
		/// @brief Move constructor (defaulted).
		constexpr Ray(Ray&& other)		= default;

		/// @brief Gets a point in the line at a given distance.
		/// @param distance Distance to line origin.
		/// @return Point at distance.
		constexpr Vector2 pointAt(float const distance) const {
			return direction.normalized() * distance;
		}

		/// @brief Ray position.
		Vector2 position;
		/// @brief Ray direction.
		Vector2 direction;
	};

	/// @brief Convex shape bound with dynamic vertex count.
	struct Shape: Follows<Ray> {
		/// @brief Empty constructor.
		constexpr Shape() {}

		/// @brief Allocates space for the shape's vertices.
		/// @param size Vertex count.
		constexpr Shape(usize const size): points(size) {}

		/// @brief Constructs the shape from an array of points.
		/// @tparam S Array size.
		/// @param trans Shape transform.
		/// @param points Vertices.
		template<usize S>
		constexpr Shape(Decay::AsType<Vector2[S]> const& points): points(points)	{}

		/// @brief Constructs the shape from a set of points.
		/// @param trans Shape transform.
		/// @param points Vertices.
		constexpr Shape(Span<Vector2> const& points): points(points)				{}

		/// @brief Copy constructor (defaulted).
		constexpr Shape(Shape const& other)	= default;
		/// @brief Move constructor (defaulted).
		constexpr Shape(Shape&& other)		= default;

		/*constexpr Vector2 furthest(Vector2 const& direction) const override {
			Vector2  maxPoint;
			float maxDistance = CTL::NumberLimit<float>::LOWEST;
			for (Vector2 const& vertex: points) {
				float distance = vertex.dot(direction);
				if (distance > maxDistance) {
					maxDistance = distance;
					maxPoint = vertex;
				}
			}
			return maxPoint;
		}*/
		
		/// @brief Shape transform.
		Transform2D		trans;
		/// @brief Shape vertices.
		List<Vector2>	points;
	};

	/// @brief Bound-agnostic collision data.
	union CollisionData {
		/// @brief Box bound.
		Box		box;
		/// @brief Circle bound.
		Circle	circle;
		/// @brief Capsule bound.
		Capsule	capsule;
		/// @brief Ray bound.
		Ray		ray;
		/// @brief Shape bound.
		Shape	shape;

		/// @brief Empty constructor.
		constexpr CollisionData()	{}

		/// @brief Constructs the data from a box bound.
		/// @param value Bound to copy from.
		constexpr CollisionData(Box const& value):		box(value)		{}
		/// @brief Constructs the data from a circle bound.
		/// @param value Bound to copy from.
		constexpr CollisionData(Circle const& value):	circle(value)	{}
		/// @brief Constructs the data from a capsule bound.
		/// @param value Bound to copy from.
		constexpr CollisionData(Capsule const& value):	capsule(value)	{}
		/// @brief Constructs the data from a ray bound.
		/// @param value Bound to copy from.
		constexpr CollisionData(Ray const& value):		ray(value)		{}
		/// @brief Constructs the data from a shape bound.
		/// @param value Bound to copy from.
		constexpr CollisionData(Shape const& value):	shape(value)	{}

		/// @brief Destructor.
		constexpr ~CollisionData()	{}
	};

	/// @brief Type of collison bound.
	enum class CollisionType {
		CT_NULL		= -1,
		CT_BOX		= Box::ID,
		CT_CIRCLE	= Circle::ID,
		CT_CAPSULE	= Capsule::ID,
		CT_RAY		= Ray::ID,
		CT_SHAPE	= Shape::ID
	};

	/// @brief Gets a bound type by its ID.
	/// @tparam ID 
	template<usize ID>
	using Bounds = CTL::Meta::NthType<
		ID,
		Box,
		Circle,
		Capsule,
		Ray,
		Shape
	>;

	/// @brief Bound-agnostic collision shape.
	struct CollisionShape {
		/// @brief Empty constructor.
		constexpr CollisionShape(): shape(CollisionType::CT_NULL) {}

		/// @brief Constructs the collision shape from a bound.
		/// @tparam T Bound type.
		/// @param bound Bound to construct from.
		template<Type::Ex::Collision::C2D::Collidable T>
		constexpr CollisionShape(T const& bound): data(bound), shape((CollisionType)T::ID) {}

		/// @brief Copy constructor.
		/// @param other `CollisionShape` to copy from.
		constexpr CollisionShape(CollisionShape const& other): data(other.value()), shape(other.shape)	{}
		/// @brief Move constructor.
		/// @param other `CollisionShape` to move.
		constexpr CollisionShape(CollisionShape&& other): data(other.value()), shape(other.shape)		{}

		/// @brief Destructor.
		constexpr ~CollisionShape()	{}

		/// @brief Returns the underlying collision data.
		/// @return Underlying collision data.
		constexpr CollisionData value() const {
			switch (shape) {
				using enum CollisionType;
				case CT_NULL:
					throw Error::InvalidValue(
						"No shape was bound!",
						__FILE__,
						toString(__LINE__),
						"CollisionShape::value"
					);
				case CT_BOX:		return data.box;
				case CT_CIRCLE:		return data.circle;
				case CT_CAPSULE:	return data.capsule;
				case CT_RAY:		return data.ray;
				case CT_SHAPE:		return data.shape;
			}
		}
		/// @brief Returns the bound type of the collision data.
		/// @return Collision bound type.
		constexpr CollisionType type() const	{return shape;	}

		/// @brief Returns the underlying collision data as a collision bound.
		/// @tparam T Bound type.
		/// @return Data as bound.
		template <Type::Ex::Collision::C2D::Collidable T>
		constexpr T as() const {
			if (shape != (CollisionType)T::ID)
				throw Error::InvalidType(
					"Collision type doesn't match stored collision type!",
					__FILE__,
					toString(__LINE__),
					toString("Requested type ID: ", T::ID, "\nStored type ID: ", enumcast(shape))
				);
			return asType<T>();
		}

		/// @brief Returns the underlying collision data as a collision bound.
		/// @tparam T Bound type.
		/// @return Data as bound.
		template<Type::Ex::Collision::C2D::Collidable T>
		constexpr explicit operator T() const {return as<T>();}

	private:
		/// @brief Returns the underlying collision data as a box bound.
		/// @return Data as bound.
		template<CTL::Type::Equal<Box> T>		constexpr Box		asType() const	{return data.box;			}
		/// @brief Returns the underlying collision data as a circle bound.
		/// @return Data as bound.
		template<CTL::Type::Equal<Circle> T>	constexpr Circle	asType() const	{return data.circle;		}
		/// @brief Returns the underlying collision data as a capsule bound.
		/// @return Data as bound.
		template<CTL::Type::Equal<Capsule> T>	constexpr Capsule	asType() const	{return data.capsule;		}
		/// @brief Returns the underlying collision data as a ray bound.
		/// @return Data as bound.
		template<CTL::Type::Equal<Ray> T>		constexpr Ray		asType() const	{return data.ray;			}
		/// @brief Returns the underlying collision data as a shape bound.
		/// @return Data as bound.
		template<CTL::Type::Equal<Shape> T>		constexpr Shape		asType() const	{return data.shape;			}

		/// @brief Underlying collision data.
		CollisionData	data;
		/// @brief Underlying collision bound type.
		CollisionType	shape;
	};
}

CTL_EX_NAMESPACE_END

#endif // CTL_EX_COLLISION_COLLISION2D_BOUNDS_H
