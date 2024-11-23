#ifndef CTL_EX_COLLISION_COLLISION2D_BOUNDS_H
#define CTL_EX_COLLISION_COLLISION2D_BOUNDS_H

#include "../../../ctl/ctl.hpp"
#include "../../math/vector.hpp"

CTL_EX_NAMESPACE_BEGIN

/// @brief Collision facilities.
namespace Collision {}

/// @brief Two-dimensional collision.
namespace Collision::C2D {
	template<size_t UUID> struct Bounded;
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
			Math::angleTo,
			Math::center,
			Math::Transform2D,
			Math::Vector2
		;
	}

	/// @brief Tas the deriving class as a bounding area of some form.
	template<size_t UUID>
	struct Bounded {
		/// @brief ID of the bounding area.
		constexpr static size_t ID = UUID;
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

	/// @brief Declares the bounds as postceding another bounds.
	template<typename T>
	using Follows = typename Impl::FollowingType<T>::Type;

	/// @brief Box bounds.
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

	/// @brief "Circle" bounds.
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
		constexpr float radiusAt(float const& angle) const {
			return
				(CTL::Math::absin(angle) * radius.x)
			+	(CTL::Math::abcos(angle) * radius.y)
			;
		}

		/// @brief Circle position.
		Vector2 position;
		/// @brief Circle radius.
		Vector2 radius = 1;
		/// @brief Circle rotation.
		float rotation = 0;
	};

	/// @brief "Capsule" bounds.
	/// @details
	///		This one is a bit complex.
	///
	///		This is a "stadium-like" ("2D-capsule-like") shape.
	///	
	///		It would be best described as the "convex hull between two ellipsess of the same angle".
	///		
	///		Also, the ellipses do not rotate with the shape.
	///		Their angles are separate from the capsule's own angle.
	/// @link https://en.wikipedia.org/wiki/Stadium_(geometry)
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
			float const& length = 0,
			float const& angle = 0,
			float const& rotation = 0
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

	/// @brief Raycast bounds.
	struct Ray: Follows<Capsule> {
		using Bounded::Bounded;

		/// @brief Constructs a raycast bound.
		/// @param position Position.
		/// @param length Length.
		/// @param angle Angle.
		constexpr Ray(
			Vector2 const& position,
			float const& length = 1,
			float const& angle = 0
		):
			position(position),
			length(length),
			angle(angle) {}

		/// @brief Copy constructor (defaulted).
		constexpr Ray(Ray const& other)	= default;
		/// @brief Move constructor (defaulted).
		constexpr Ray(Ray&& other)		= default;

		/// @brief Ray position.
		Vector2 position;
		/// @brief Ray length.
		float length = 1;
		/// @brief Ray angle.
		float angle = 0;
	};

	/// @brief Convex shape with dynamic vertex count.
	struct Shape: Follows<Ray> {
		constexpr Shape(): points(nullptr), count(0) {}

		constexpr Shape(usize const& size):
			points(new Vector2[size]{0}),
			count(size) {}

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
			Span<Vector2> const& points
		): Shape(points.size()) {
			this->trans = trans;
			for (usize i = 0; i < count; ++i)
				this->points[i] = points[i];
		}

		constexpr Shape(Shape const& other):
			Shape(
				other.trans,
				{other.points, other.count}
			) {}

		constexpr Shape(Shape&& other):
			Shape(
				other.trans,
				{other.points, other.count}
			) {}

		constexpr ~Shape() {
			if (points) delete[] points;
		}

		constexpr Vector2* data() const	{return points;	}
		constexpr usize	size() const	{return count;	}

		constexpr Vector2* cbegin() const	{return data();				}
		constexpr Vector2* cend() const		{return data() + size();	}

		constexpr Vector2* begin() const	{return cbegin();			}
		constexpr Vector2* end() const		{return cend();				}

		Transform2D trans;

	private:
		Vector2*	points	= nullptr;
		usize		count	= 0;
	};

	struct Polygon: Follows<Shape> {
		constexpr Polygon(
			Transform2D const& trans,
			List<Shape> const& pieces
		):
			trans(trans),
			pieces(pieces) {}

		constexpr Polygon(Polygon const& other)	= default;
		constexpr Polygon(Polygon&& other)		= default;

		Transform2D trans;
		List<Shape> pieces;
	};

	union CollisionData {
		Box		box;
		Circle	circle;
		Capsule	capsule;
		Ray		ray;
		Shape	shape;
		Polygon	polygon;

		constexpr CollisionData()	{}

		constexpr CollisionData(Box const& value):		box(value)		{}
		constexpr CollisionData(Circle const& value):	circle(value)	{}
		constexpr CollisionData(Capsule const& value):	capsule(value)	{}
		constexpr CollisionData(Ray const& value):		ray(value)		{}
		constexpr CollisionData(Shape const& value):	shape(value)	{}
		constexpr CollisionData(Polygon const& value):	polygon(value)	{}

		constexpr CollisionData(CollisionData const& value) {
			// Trivially-predictable nightmares...
			MX::memmove((void*)this, (void*)&value, sizeof(CollisionData));
		}

		constexpr ~CollisionData()	{}
	};

	enum class CollisionType {
		CT_NULL		= -1,
		CT_BOX		= Box::ID,
		CT_CIRCLE	= Circle::ID,
		CT_CAPSULE	= Capsule::ID,
		CT_RAY		= Ray::ID,
		CT_SHAPE	= Shape::ID,
		CT_POLYGON	= Polygon::ID
	};

	template<usize ID>
	using Bounds = CTL::Meta::NthType<
		ID,
		Box,
		Circle,
		Capsule,
		Ray,
		Figure,
		Polygon
	>;

	struct CollisionShape {
		constexpr CollisionShape(): shape(CollisionType::CT_NULL) {}

		template<Type::Ex::Collision::C2D::Collidable T>
		constexpr CollisionShape(T const& bounds): data(bounds), shape((CollisionType)T::ID) {}

		constexpr ~CollisionShape()	{}

		constexpr CollisionData value() const	{return data;	}
		constexpr CollisionType type() const	{return shape;	}

		template<CTL::Type::Equal<Box> T>			constexpr Box		asType() const	{return data.box;			}
		template<CTL::Type::Equal<Circle> T>		constexpr Circle	asType() const	{return data.circle;		}
		template<CTL::Type::Equal<Capsule> T>		constexpr Capsule	asType() const	{return data.capsule;		}
		template<CTL::Type::Equal<Ray> T>			constexpr Ray		asType() const	{return data.ray;			}
		template<CTL::Type::Equal<Polygon> T>		constexpr Polygon	asType() const	{return data.polygon;		}
		template<CTL::Type::Equal<Shape> T>			constexpr Shape		asType() const	{return data.shape;			}

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
