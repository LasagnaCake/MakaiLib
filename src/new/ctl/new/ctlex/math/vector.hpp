#ifndef CTL_EX_MATH_VECTOR_H
#define CTL_EX_MATH_VECTOR_H

#include "../../ctl/exnamespace.hpp"
#include "../../ctl/math/core.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"

namespace Type::Vector {
	template<typename T>
	concept ValidBase = requires {
		requires	Type::Arithmetic<T, T>;
		requires	Type::Arithmetic<T, float>;
		requires	Type::HasModulo<T, T>;
		requires	Type::HasModulo<T, float>;
		requires	Type::Constructible<T, float>;
	};

	template<typename T>
	concept ValidVec2 = requires {
		requires	ValidBase<T>;
		requires	Type::Constructible<T, float, float>;
		T::x;
		T::y;
	};

	template<typename T>
	concept ValidVec3 = requires {
		requires	ValidBase<T>;
		requires	Type::Constructible<T, float, float>;
		requires	Type::Constructible<T, float, float, float>;
		T::x;
		T::y;
		T::z;
	};

	template<typename T>
	concept ValidVec4 = requires {
		requires	ValidBase<T>;
		requires	Type::Constructible<T, float, float>;
		requires	Type::Constructible<T, float, float, float>;
		requires	Type::Constructible<T, float, float, float, float>;
		T::x;
		T::y;
		T::z;
		T::w;
	};

	template<typename T>
	concept ValidVector =
		ValidVec2<T>
	||	ValidVec3<T>
	||	ValidVec4<T>
	;

	template<typename T>
	concept Vectorable = Type::Equal<T, float> || ValidVector<T>;
}

CTL_EX_NAMESPACE_BEGIN

namespace Math {

/**
* [-----------------]
* [                 ]
* [  Vector2 Class  ]
* [                 ]
* [-----------------]
*/
#pragma pack(1)
class Vector2
{
	public:
		/// The vector's position.
		union {
			struct {float x, y;		};
			struct {float nx, ny;	};
			struct {float u, v;		};
			float data[2] = {0};
		};

		/// Consturctors.
		constexpr Vector2() {}

		constexpr Vector2(float v):
			x(v), y(v) {}

		constexpr Vector2(float const& x, float const& y = 0.0):
			x(x), y(y) {}

		constexpr Vector2(Vector2 const& vec):
			Vector2(vec.x, vec.y) {}

		constexpr Vector2(Decay::AsType<float[2]> const& data):
			Vector2(data[0], data[1]) {}

		/// Destructor.
		constexpr ~Vector2() {}

		/// Static values.
		constexpr static Vector2 ZERO()		{return 0;					}
		constexpr static Vector2 ONE()		{return 1;					}
		constexpr static Vector2 RIGHT()	{return Vector2(+1, +0);	}
		constexpr static Vector2 LEFT()		{return Vector2(-1, +0);	}
		constexpr static Vector2 UP()		{return Vector2(+0, +1);	}
		constexpr static Vector2 DOWN()		{return Vector2(+0, -1);	}

		/// Vector operator overloading.
		constexpr Vector2 operator+(Vector2 const& vec) const {
			return Vector2(
				x + vec.x,
				y + vec.y
			);
		}

		constexpr Vector2 operator-(Vector2 const& vec) const {
			return Vector2(
				x - vec.x,
				y - vec.y
			);
		}

		constexpr Vector2 operator*(Vector2 const& vec) const {
			return Vector2(
				x * vec.x,
				y * vec.y
			);
		}

		constexpr Vector2 operator/(Vector2 const& vec) const {
			return Vector2(
				x / vec.x,
				y / vec.y
			);
		}

		constexpr Vector2 operator%(Vector2 const& vec) const {
			return Vector2(
				fmod(x, vec.x),
				fmod(y, vec.y)
			);
		}

		constexpr Vector2 operator^(Vector2 const& vec) const {
			return Vector2(
				pow(x, vec.x),
				pow(y, vec.y)
			);
		}

		constexpr Vector2& operator+=(Vector2 const& vec) {
			x += vec.x;
			y += vec.y;
			return *this;
		}

		constexpr Vector2& operator-=(Vector2 const& vec) {
			x -= vec.x;
			y -= vec.y;
			return *this;
		}

		constexpr Vector2& operator*=(Vector2 const& vec) {
			x *= vec.x;
			y *= vec.y;
			return *this;
		}

		constexpr Vector2& operator/=(Vector2 const& vec) {
			x /= vec.x;
			y /= vec.y;
			return *this;
		}

		constexpr Vector2& operator%=(Vector2 const& vec) {
			x = fmod(x, vec.x);
			y = fmod(y, vec.y);
			return *this;
		}

		constexpr Vector2& operator^=(Vector2 const& vec) {
			x = fmod(x, vec.x);
			y = fmod(y, vec.y);
			return *this;
		}

		/// Float operator overloading.
		constexpr Vector2 operator+(float const& val) const {
			return *this + Vector2(val);
		}

		constexpr Vector2 operator-(float const& val) const {
			return *this - Vector2(val);
		}

		constexpr Vector2 operator*(float const& val) const {
			return *this * Vector2(val);
		}

		constexpr Vector2 operator/(float const& val) const {
			return *this / Vector2(val);
		}

		constexpr Vector2 operator%(float const& val) const {
			return *this % Vector2(val);
		}

		constexpr Vector2 operator^(float const& val) const {
			return *this ^ Vector2(val);
		}

		constexpr Vector2& operator+=(float const& val) {
			*this += Vector2(val);
			return *this;
		}

		constexpr Vector2& operator-=(float const& val) {
			*this -= Vector2(val);
			return *this;
		}

		constexpr Vector2& operator*=(float const& val) {
			*this *= Vector2(val);
			return *this;
		}

		constexpr Vector2& operator/=(float const& val) {
			*this /= Vector2(val);
			return *this;
		}

		constexpr Vector2& operator%=(float const& val) {
			*this %= Vector2(val);
			return *this;
		}

		constexpr Vector2& operator^=(float const& val) {
			*this ^= Vector2(val);
			return *this;
		}

		/// Comparison operators

		typedef Helper::PartialOrder OrderType;

		constexpr bool operator==(Vector2 const& other) const {
			return (
				x == other.x
			&&	y == other.y
			);
		}

		constexpr OrderType operator<=>(Vector2 const& other) const {
			OrderType result = OrderType::equivalent;
			if ((result = x <=> other.x) != OrderType::equivalent) return result;
			return y <=> other.y;
		}

		template <typename T>
		constexpr OrderType operator<=>(T const& val) const
		requires Type::Convertible<T, float> {
			return *this <=> Vector2(val);
		}

		/// Unary operators

		constexpr Vector2 operator+() const {
			return *this;
		}

		constexpr Vector2 operator-() const {
			return *this * -1;
		}

		// Other overloads

		constexpr float& operator[](size_t const& pos)		{if (pos > 1) return data[0]; return data[pos];}
		constexpr float operator[](size_t const& pos) const	{if (pos > 1) return data[0]; return data[pos];}

		constexpr float* operator*() {return data;};

		constexpr explicit operator float*()				{return data;}
		constexpr explicit operator const float*() const	{return data;}

		// Extra functions

		/// Gets the sum of the vector's components.
		constexpr float sum() const {
			return x + y;
		}

		/// Gets the average of the vector's components.
		constexpr float average() const {
			return sum() / 2;
		}

		/// Gets the smallest of the vector's components.
		constexpr float min() const {
			return (x < y) ? x : y;
		}

		/// Gets the biggest of the vector's components.
		constexpr float max() const {
			return (x > y) ? x : y;
		}

		/// Gets the vector's length (distance to origin).
		constexpr float length() const {
			return sqrt((x * x) + (y * y));
		}

		/// Gets the vector's length (distance to origin).
		constexpr float lengthSquared() const {
			return ((x * x) + (y * y));
		}

		/// Gets the normalized vector.
		constexpr Vector2 normalized() const {
			if (*this != 0)
				return *this / length();
			else
				return *this;
		}

		/// Normalizes the vector.
		constexpr Vector2& normalize() {
			if (*this != 0)
				return *this /= length();
			else
				return *this;
		}

		/// Gets the distance to another vector.
		constexpr float distanceTo(Vector2 const& vec) const {
			Vector2 diff = vec - *this;
			return diff.length();
		}

		/// Gets the distance to another vector.
		constexpr float squaredDistanceTo(Vector2 const& vec) const {
			Vector2 diff = vec - *this;
			return diff.lengthSquared();
		}

		/// Gets the vector's angle.
		constexpr float angle() const {
			return atan2(x, y) - Math::hpi;
		}

		/// Gets the vector's angle to another vector.
		constexpr float angleTo(Vector2 const& vec) const {
			Vector2 diff = vec - *this;
			return diff.angle();
		}

		/// Gets a normalized vector pointing towards another vector.
		constexpr Vector2 normalTo(Vector2 const& vec) const {
			Vector2 diff = vec - *this;
			return diff.normalized();
		}


		/// Clamps the vector between two values.
		constexpr void clamp(Vector2 const& min, Vector2 const& max) {
			x = Math::clamp(x, min.x, max.x);
			y = Math::clamp(y, min.y, max.y);
		}

		/// Returns the vector clamped between two values.
		constexpr Vector2 clamped(Vector2 const& min, Vector2 const& max) const {
			return Vector2(
				Math::clamp(x, min.x, max.x),
				Math::clamp(y, min.y, max.y)
			);
		}

		/// Gets the tangent of the vector.
		constexpr float tangent() const {
			return x / y;
		}

		/// Returns the dot product with another vector.
		constexpr float dotProd(Vector2 const& vec) const {
			Vector2 mult = (*this) * vec;
			return mult.x + mult.y;
		}

		/// Returns the cross product with another vector.
		constexpr float crossProd(Vector2 const& vec) const {
			return (x * vec.y) - (y * vec.x);
		}

		constexpr Vector2 absolute() const {
			return Vector2(
				abs(x),
				abs(y)
			);
		}

		constexpr Vector2 yx() const {
			return Vector2(y, x);
		}
};

/**
* [-----------------]
* [                 ]
* [  Vector3 Class  ]
* [                 ]
* [-----------------]
*/
#pragma pack(1)
class Vector3
{
	public:
		/// The vector's position.
		union {
			struct {float x, y, z;		};
			struct {float nx, ny, nz;	};
			struct {float r, g, b;		};
			struct {float u, v, t;		};
			float data[3] = {0};
		};

		/// Constructors.

		constexpr Vector3() {}

		constexpr Vector3(float v):
			x(v), y(v), z(v) {}

		constexpr Vector3(float const& x, float const& y, float const& z = 0.0):
			x(x), y(y), z(z) {}

		constexpr Vector3(Vector3 const& vec):
			Vector3(vec.x, vec.y, vec.z) {}

		constexpr Vector3(Vector2 const& vec, float z = 0):
			Vector3(vec.x, vec.y, z) {}

		constexpr Vector3(Decay::AsType<float[3]> const& data):
			Vector3(data[0], data[1], data[2]) {}


		/// Destructor.

		constexpr ~Vector3() {}

		/// Static values.

		constexpr static Vector3 ZERO()		{return 0;						}
		constexpr static Vector3 ONE()		{return 1;						}
		constexpr static Vector3 RIGHT()	{return Vector2::RIGHT();		}
		constexpr static Vector3 LEFT()		{return Vector2::LEFT();		}
		constexpr static Vector3 UP()		{return Vector2::UP();			}
		constexpr static Vector3 DOWN()		{return Vector2::DOWN();		}
		constexpr static Vector3 BACK()		{return Vector3(+0, +0, +1);	}
		constexpr static Vector3 FRONT()	{return Vector3(+0, +0, -1);	}

		/// Vector operator overloading.

		constexpr Vector3 operator+(Vector3 const& vec) const {
			return Vector3(
				x + vec.x,
				y + vec.y,
				z + vec.z
			);
		}

		constexpr Vector3 operator-(Vector3 const& vec) const {
			return Vector3(
				x - vec.x,
				y - vec.y,
				z - vec.z
			);
		}

		constexpr Vector3 operator*(Vector3 const& vec) const {
			return Vector3(
				x * vec.x,
				y * vec.y,
				z * vec.z
			);
		}

		constexpr Vector3 operator/(Vector3 const& vec) const {
			return Vector3(
				x / vec.x,
				y / vec.y,
				z / vec.z
			);
		}

		constexpr Vector3 operator%(Vector3 const& vec) const {
			return Vector3(
				fmod(x, vec.x),
				fmod(y, vec.y),
				fmod(z, vec.z)
			);
		}

		constexpr Vector3 operator^(Vector3 const& vec) const {
			return Vector3(
				pow(x, vec.x),
				pow(y, vec.y),
				pow(z, vec.z)
			);
		}

		constexpr Vector3& operator+=(Vector3 const& vec) {
			x += vec.x;
			y += vec.y;
			z += vec.z;
			return *this;
		}

		constexpr Vector3& operator-=(Vector3 const& vec) {
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
			return *this;
		}

		constexpr Vector3& operator*=(Vector3 const& vec) {
			x *= vec.x;
			y *= vec.y;
			z *= vec.z;
			return *this;
		}

		constexpr Vector3& operator/=(Vector3 const& vec) {
			x /= vec.x;
			y /= vec.y;
			z /= vec.z;
			return *this;
		}

		constexpr Vector3& operator%=(Vector3 const& vec) {
			x = fmod(x, vec.x);
			y = fmod(y, vec.y);
			z = fmod(z, vec.z);
			return *this;
		}

		constexpr Vector3& operator^=(Vector3 const& vec) {
			x = pow(x, vec.x);
			y = pow(y, vec.y);
			z = pow(z, vec.z);
			return *this;
		}

		/// Float operator overloading.

		constexpr Vector3 operator+(float const& val) const {
			return *this + Vector3(val);
		}

		constexpr Vector3 operator-(float const& val) const {
			return *this - Vector3(val);
		}

		constexpr Vector3 operator*(float const& val) const {
			return *this * Vector3(val);
		}

		constexpr Vector3 operator/(float const& val) const {
			return *this / Vector3(val);
		}

		constexpr Vector3 operator%(float const& val) const {
			return *this % Vector3(val);
		}

		constexpr Vector3 operator^(float const& val) const {
			return *this ^ Vector3(val);
		}

		constexpr Vector3& operator+=(float const& val) {
			*this += Vector3(val);
			return *this;
		}

		constexpr Vector3& operator-=(float const& val) {
			*this -= Vector3(val);
			return *this;
		}

		constexpr Vector3& operator*=(float const& val) {
			*this *= Vector3(val);
			return *this;
		}

		constexpr Vector3& operator/=(float const& val) {
			*this /= Vector3(val);
			return *this;
		}

		constexpr Vector3& operator%=(float const& val) {
			*this %= Vector3(val);
			return *this;
		}

		constexpr Vector3& operator^=(float const& val) {
			*this ^= Vector3(val);
			return *this;
		}

		/// Comparison operators

		typedef Helper::PartialOrder OrderType;

		constexpr bool operator==(Vector3 const& other) const {
			return (
				x == other.x
			&&	y == other.y
			&&	z == other.z
			);
		}

		constexpr OrderType operator<=>(Vector3 const& other) const {
			OrderType result = OrderType::equivalent;
			if ((result = x <=> other.x) != OrderType::equivalent) return result;
			if ((result = y <=> other.y) != OrderType::equivalent) return result;
			return z <=> other.z;
		}

		template <typename T>
		constexpr OrderType operator<=>(T const& val) const
		requires Type::Convertible<T, float> {
			return *this <=> Vector3(val);
		}

		/// Unary operators

		constexpr Vector3 operator+() const {
			return *this;
		}

		constexpr Vector3 operator-() const {
			return *this * -1;
		}

		// Other overloads

		constexpr explicit operator float*()				{return data;}
		constexpr explicit operator const float*() const	{return data;}

		constexpr float* operator*() {return data;};

		constexpr float& operator[](size_t const& pos)		{if (pos > 2) return data[0]; return data[pos];}
		constexpr float operator[](size_t const& pos) const	{if (pos > 2) return data[0]; return data[pos];}

		// Extra functions

		/// Gets the sum of the vector's components.
		constexpr float sum() const {
			return x + y + z;
		}

		/// Gets the average of the vector's components.
		constexpr float average() const {
			return sum() / 3;
		}

		/// Gets the smallest of the vector's components.
		constexpr float min() const {
			return Math::min(Math::min(x, y), z);
		}

		/// Gets the biggest of the vector's components.
		constexpr float max() const {
			return Math::max(Math::max(x, y), z);
		}

		/// Gets the vector's length (distance to origin).
		constexpr float length() const {
			return sqrt((x * x) + (y * y) + (z * z));
		}

		/// Gets the vector's length (distance to origin).
		constexpr float lengthSquared() const {
			return ((x * x) + (y * y) + (z * z));
		}

		/// Gets the vector's angle.
		constexpr Vector3 angle() const {
			Vector3 res;
			float mag = length();
			res.x = acos(x/mag);
			res.y = acos(y/mag);
			res.z = acos(z/mag);
			return res;
		}

		/// Gets the vector's angle to another vector.
		constexpr Vector3 angleTo(Vector3 const& vec) const {
			Vector3 diff = vec - *this;
			return diff.angle();
		}

		/// Gets the normalized vector.
		constexpr Vector3 normalized() const {
			if (*this != 0)
				return *this / length();
			else
				return *this;
		}

		/// Normalizes the vector.
		constexpr Vector3& normalize() {
			if (*this != 0)
				return *this /= length();
			else
				return *this;
		}

		/// Gets a normalized vector pointing towards another vector.
		constexpr Vector3 normalTo(Vector3 const& vec) const {
			Vector3 diff = vec - *this;
			return diff.normalized();
		}

		/// Gets the distance to another vector.
		constexpr float distanceTo(Vector3 const& vec) const {
			Vector3 diff = vec - *this;
			return diff.length();
		}

		/// Gets the distance to another vector.
		constexpr float squaredDistanceTo(Vector3 const& vec) const {
			Vector3 diff = vec - *this;
			return diff.lengthSquared();
		}

		/// Clamps the vector between two values.
		constexpr void clamp(Vector3 const& min, Vector3 const& max) {
			x = Math::clamp(x, min.x, max.x);
			y = Math::clamp(y, min.y, max.y);
			z = Math::clamp(z, min.z, max.z);
		}

		/// Returns the vector clamped between two values.
		constexpr Vector3 clamped(Vector3 const& min, Vector3 const& max) const {
			return Vector3(
				Math::clamp(x, min.x, max.x),
				Math::clamp(y, min.y, max.y),
				Math::clamp(z, min.z, max.z)
			);
		}

		/// Returns the dot product with another vector.
		constexpr float dotProd(Vector3 const& vec) const {
			Vector3 mult = (*this) * vec;
			return mult.x + mult.y + mult.z;
		}

		/// Returns the cross product with another vector.
		constexpr Vector3 crossProd(Vector3 const& vec) const {
			return Vector3(
				(y * vec.z) - (z * vec.y),
				(z * vec.x) - (x * vec.z),
				(x * vec.y) - (y * vec.x)
			);
		}

		/// Returns the mixed (scalar triple) product with two other vectors in the order of this->dotProd(a.crossProd(b))).
		constexpr float mixProd(Vector3 const& a, Vector3 const& b) const {
			return dotProd(a.crossProd(b));
		}

		constexpr Vector2 xy() const {
			return Vector2(x, y);
		}

		constexpr Vector2 yz() const {
			return Vector2(x, y);
		}

		constexpr Vector2 xz() const {
			return Vector2(x, y);
		}

		constexpr Vector3 zyx() const {
			return Vector3(z, y, x);
		}

		constexpr Vector3 xzy() const {
			return Vector3(x, z, y);
		}

		constexpr Vector3 yzx() const {
			return Vector3(y, z, x);
		}

		constexpr Vector3 absolute() const {
			return Vector3(
				abs(x),
				abs(y),
				abs(z)
			);
		}
};

/**
* [-----------------]
* [                 ]
* [  Vector4 Class  ]
* [                 ]
* [-----------------]
*/
#pragma pack(1)
class Vector4
{
	public:
		/// The vector's position.
		union {
			struct {float x, y, z, w;		};
			struct {float nx, ny, nz, nw;	};
			struct {float r, g, b, a;		};
			struct {float u, v, t, s;		};
			float data[4] = {0};
		};

		/// Constructors.

		constexpr Vector4() {}

		constexpr Vector4(float v):
			x(v), y(v), z(v), w(v) {}

		constexpr Vector4(float const& x, float const& y, float const& z, float const& w = 0.0):
			x(x), y(y), z(z), w(w) {}

		constexpr Vector4(Vector2 const& v1, Vector2 const& v2):
			Vector4(v1.x, v1.y, v2.x, v2.y) {}

		constexpr Vector4(Vector4 const& vec):
			Vector4(vec.x, vec.y, vec.z, vec.w) {}

		constexpr Vector4(Vector3 const& vec, float w = 0):
			Vector4(vec.x, vec.y, vec.z, w) {}

		constexpr Vector4(Vector2 const& vec, float z = 0, float w = 0):
			Vector4(vec.x, vec.y, z, w) {}

		constexpr Vector4(Decay::AsType<float[4]> const& data):
			Vector4(data[0], data[1], data[2], data[3]) {}

		/// Destructor.

		constexpr ~Vector4() {}

		/// Static values.

		constexpr static Vector4 ZERO()		{return 0;							}
		constexpr static Vector4 ONE()		{return 1;							}
		constexpr static Vector4 RIGHT()	{return Vector2::RIGHT();			}
		constexpr static Vector4 LEFT()		{return Vector2::LEFT();			}
		constexpr static Vector4 UP()		{return Vector2::UP();				}
		constexpr static Vector4 DOWN()		{return Vector2::DOWN();			}
		constexpr static Vector4 BACK()		{return Vector3::BACK();			}
		constexpr static Vector4 FRONT()	{return Vector3::FRONT();			}
		constexpr static Vector4 FUTURE()	{return Vector4(+0, +0, +0, +1);	}
		constexpr static Vector4 PAST()		{return Vector4(+0, +0, +0, -1);	}

		/// Vector operator overloading.

		constexpr Vector4 operator+(Vector4 const& vec) const {
			return Vector4(
				x + vec.x,
				y + vec.y,
				z + vec.z,
				w + vec.w
			);
		}

		constexpr Vector4 operator-(Vector4 const& vec) const {
			return Vector4(
				x - vec.x,
				y - vec.y,
				z - vec.z,
				w - vec.w
			);
		}

		constexpr Vector4 operator*(Vector4 const& vec) const {
			return Vector4(
				x * vec.x,
				y * vec.y,
				z * vec.z,
				w * vec.w
			);
		}

		constexpr Vector4 operator/(Vector4 const& vec) const {
			return Vector4(
				x / vec.x,
				y / vec.y,
				z / vec.z,
				w / vec.w
			);
		}

		constexpr Vector4 operator%(Vector4 const& vec) const {
			return Vector4(
				fmod(x, vec.x),
				fmod(y, vec.y),
				fmod(z, vec.z),
				fmod(w, vec.w)
			);
		}

		constexpr Vector4 operator^(Vector4 const& vec) const {
			return Vector4(
				pow(x, vec.x),
				pow(y, vec.y),
				pow(z, vec.z),
				pow(w, vec.w)
			);
		}

		constexpr Vector4& operator+=(Vector4 const& vec) {
			x += vec.x;
			y += vec.y;
			z += vec.z;
			w += vec.w;
			return *this;
		}

		constexpr Vector4& operator-=(Vector4 const& vec) {
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
			w -= vec.w;
			return *this;
		}

		constexpr Vector4& operator*=(Vector4 const& vec) {
			x *= vec.x;
			y *= vec.y;
			z *= vec.z;
			w *= vec.w;
			return *this;
		}

		constexpr Vector4& operator/=(Vector4 const& vec) {
			x /= vec.x;
			y /= vec.y;
			z /= vec.z;
			w /= vec.w;
			return *this;
		}

		constexpr Vector4& operator%=(Vector4 const& vec) {
			x = fmod(x, vec.x);
			y = fmod(y, vec.y);
			z = fmod(z, vec.z);
			w = fmod(w, vec.w);
			return *this;
		}

		constexpr Vector4& operator^=(Vector4 const& vec) {
			x = pow(x, vec.x);
			y = pow(y, vec.y);
			z = pow(z, vec.z);
			w = pow(w, vec.w);
			return *this;
		}

		/// Float operator overloading.

		constexpr Vector4 operator+(float const& val) const {
			return *this + Vector4(val);
		}

		constexpr Vector4 operator-(float const& val) const {
			return *this - Vector4(val);
		}

		constexpr Vector4 operator*(float const& val) const {
			return *this * Vector4(val);
		}

		constexpr Vector4 operator/(float const& val) const {
			return *this / Vector4(val);
		}

		constexpr Vector4 operator%(float const& val) const {
			return *this % Vector4(val);
		}

		constexpr Vector4 operator^(float const& val) const {
			return *this ^ Vector4(val);
		}

		constexpr Vector4& operator+=(float const& val) {
			*this += Vector4(val);
			return *this;
		}

		constexpr Vector4& operator-=(float const& val) {
			*this -= Vector4(val);
			return *this;
		}

		constexpr Vector4& operator*=(float const& val) {
			*this *= Vector4(val);
			return *this;
		}

		constexpr Vector4& operator/=(float const& val) {
			*this /= Vector4(val);
			return *this;
		}

		constexpr Vector4& operator^=(float const& val) {
			*this ^= Vector4(val);
			return *this;
		}

		constexpr Vector4& operator%=(float const& val) {
			*this %= Vector4(val);
			return *this;
		}

		/// Comparison operators

		typedef Helper::PartialOrder OrderType;

		constexpr bool operator==(Vector4 const& other) const {
			return (
				x == other.x
			&&	y == other.y
			&&	z == other.z
			&&	w == other.w
			);
		}

		constexpr OrderType operator<=>(Vector4 const& other) const {
			OrderType result = OrderType::equivalent;
			if ((result = x <=> other.x) != OrderType::equivalent) return result;
			if ((result = y <=> other.y) != OrderType::equivalent) return result;
			if ((result = z <=> other.z) != OrderType::equivalent) return result;
			return w <=> other.w;
		}

		template <typename T>
		constexpr OrderType operator<=>(T const& val) const
		requires Type::Convertible<T, float> {
			return *this <=> Vector4(val);
		}

		/// Unary operators

		constexpr Vector4 operator+() const {
			return *this;
		}

		constexpr Vector4 operator-() const {
			return *this * -1;
		}

		// Other overloads

		constexpr explicit operator float*()				{return data;}
		constexpr explicit operator const float*() const	{return data;}

		constexpr float* operator*() {return data;};

		constexpr float& operator[](size_t const& pos)		{if (pos > 3) return data[0]; return data[pos];}
		constexpr float operator[](size_t const& pos) const	{if (pos > 3) return data[0]; return data[pos];}

		// Extra functions

		/// Gets the sum of the vector's components.
		constexpr float sum() const {
			return x + y + z + w;
		}

		/// Gets the average of the vector's components.
		constexpr float average() const {
			return sum() / 4;
		}

		/// Gets the smallest of the vector's components.
		constexpr float min() const {
			return Math::min(Math::min(Math::min(x, y), z), w);
		}

		/// Gets the biggest of the vector's components.
		constexpr float max() const {
			return Math::max(Math::max(Math::max(x, y), z), w);
		}

		/// Gets the vector's length (distance to origin).
		constexpr float length() const {
			return sqrt((x * x) + (y * y) + (z * z) + (w * w));
		}

		/// Gets the vector's length (distance to origin).
		constexpr float lengthSquared() const {
			return ((x * x) + (y * y) + (z * z) + (w * w));
		}

		/// Gets the normalized vector.
		constexpr Vector4 normalized() const {
			if (*this != 0)
				return *this / length();
			else
				return *this;
		}

		/// Normalizes the vector.
		constexpr Vector4& normalize() {
			if (*this != 0)
				return *this /= length();
			else
				return *this;
		}

		/// Gets the distance to another vector.
		constexpr float distanceTo(Vector4 const& vec) const {
			Vector4 diff = vec - *this;
			return diff.length();
		}

		/// Gets the distance to another vector.
		constexpr float squaredDistanceTo(Vector4 const& vec) const {
			Vector4 diff = vec - *this;
			return diff.lengthSquared();
		}

		/// Gets a normalized vector pointing towards another vector.
		constexpr Vector4 normalTo(Vector4 const& vec) const {
			Vector4 diff = vec - *this;
			return diff.normalized();
		}

		/// Clamps the vector between two values.
		constexpr void clamp(Vector4 const& min, Vector4 const& max) {
			x = Math::clamp(x, min.x, max.x);
			y = Math::clamp(y, min.y, max.y);
			z = Math::clamp(z, min.z, max.z);
			z = Math::clamp(w, min.w, max.w);
		}

		/// Returns the vector clamped between two values.
		constexpr Vector4 clamped(Vector4 const& min, Vector4 const& max) const {
			return Vector4(
				Math::clamp(x, min.x, max.x),
				Math::clamp(y, min.y, max.y),
				Math::clamp(z, min.z, max.z),
				Math::clamp(w, min.w, max.w)
			);
		}

		/// Returns the dot product with another vector.
		constexpr float dotProd(Vector4 const& vec) const {
			Vector4 mult = (*this) * vec;
			return mult.x + mult.y + mult.z + mult.w;
		}

		constexpr Vector4 absolute() const {
			return Vector4(
				abs(x),
				abs(y),
				abs(z),
				abs(w)
			);
		}

		constexpr Vector3 xyz() const {
			return Vector3(x, y, z);
		}

		constexpr Vector4 wzyx() const {
			return Vector4(w, z, y, x);
		}

		constexpr Vector4 wxyz() const {
			return Vector4(w, x, y, z);
		}

		constexpr Vector4 compensated() const {
			return Vector4(xyz() / w, w);
		}
};

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef Vector4 Vec4;

template <
	Type::Vector::ValidVector	TPosition,
	Type::Vector::Vectorable	TRotation	= TPosition,
	Type::Vector::ValidVector	TScale		= TPosition
>
struct Transform {
	typedef TPosition	PositionType;
	typedef TRotation	RotationType;
	typedef TScale		ScaleType;

	constexpr Transform():
		position(0),
		rotation(0),
		scale(1) {}

	constexpr Transform(PositionType const& position, RotationType const& rotation, ScaleType const& scale):
		position(position),
		rotation(rotation),
		scale(scale) {}

	constexpr static Transform identity() {return Transform(0, 0, 1);}

	TPosition	position	= TPosition(0);
	TRotation	rotation	= TRotation(0);
	TScale		scale		= TScale(1);
};

using Transform2D = Transform<Vector2, float, Vector2>;
using Transform3D = Transform<Vector3, Vector3, Vector3>;

enum class Axis: size_t {
	POS_X,
	POS_Y,
	POS_Z,
	NEG_X,
	NEG_Y,
	NEG_Z
};

// Vector rotation

/// Rotates a 2D Vector around the origin by a given angle.
constexpr Vector2 rotateV2(Vector2 vec, float const& angle) {
	// Calculate the sine & cosine of the angle
	float sinA = sin(angle);
	float cosA = cos(angle);
	// Calculate the rotation around the Z axis (i.e. 2D rotation)
	vec.x = vec.x * cosA - vec.y * sinA;
	vec.y = vec.x * sinA + vec.y * cosA;
	// Return rotated vector
	return vec;
}

/// Rotates a given 3D Vector around the origin's axis by given angles.
constexpr Vector3 rotateV3(Vector3 vec, Vector3 const& angle) {
	/*
	* Based off of 3D Rotation Matrices:
	* https://en.wikipedia.org/wiki/Rotation_matrix#General_rotations
	* Most likely follows Tait-Bryan Angles:
	* https://en.wikipedia.org/wiki/Euler_angles#Tait%E2%80%93Bryan_angles
	*/
	// Get a copy of the vector
	Vector3 res = vec;
	#ifndef CTL_EX_SIMPLIFIED_ROTATION_MATH
	// Get sines and cosines
	float sinX = sin(angle.x);
	float cosX = cos(angle.x);
	float sinY = sin(angle.y);
	float cosY = cos(angle.y);
	float sinZ = sin(angle.z);
	float cosZ = cos(angle.z);
	// Calculate Z axis
	res.x = (cosZ * res.x) - (sinZ * res.y);
	res.y = (sinZ * res.x) + (cosZ * res.y);
	// Calculate Y axis
	res.x = (cosY * res.x) + (sinY * sinZ * res.y) + (sinY * cosZ * res.z);
	res.z = (-sinY * cosX * res.x) + (cosX * cosY * res.y) + (sinX * cosY * res.z);
	// Calculate X axis
	res.y = (cosX * res.y) - (sinX * res.z);
	res.z = (sinX * res.y) + (cosX * res.z);
	#else
	// Rotation buffer
	Vector2 buf;
	// Rotate around Z axis
	buf = rotateV2(res.xy(), angle.z);
	res = Vector3(buf.x, buf.y, res.z);
	// Rotate around Y axis
	buf = rotateV2(res.xz(), angle.y);
	res = Vector3(buf.x, res.y, buf.y);
	// Rotate around X axis
	buf = rotateV2(res.yz(), angle.z);
	res = Vector3(res.x, buf.x, buf.y);
	#endif
	// Return result
	return res;
}

/**
* Rotates a given 3D Vector around two of the origin's axis by two respective angles.
* The given axis is the axis to be excluded.
*/
constexpr Vector3 rotateV3(Vector3 const& vec, Vector2 const& angle, Axis const& exclude = Axis::POS_X) {
	switch (exclude) {
	case Axis::POS_X:
	case Axis::NEG_X:
		return rotateV3(vec, Vector3(0, angle.x, angle.y));
	case Axis::POS_Y:
	case Axis::NEG_Y:
	default:
		return rotateV3(vec, Vector3(angle.x, 0, angle.y));
	case Axis::POS_Z:
	case Axis::NEG_Z:
		return rotateV3(vec, Vector3(angle.x, angle.y, 0));
	}
}

// No fourth-dimensional rotation. I'd rather die.

// Angle-to-vector conversion

/// Gets a 2D Vector of size 1 at a given angle relative to the origin.
constexpr Vector2 angleV2(float const& angle) {
	return Vector2(cos(angle), -sin(angle));
}

/// Gets a 3D Vector of size 1 at a given angle around one of the origin's axis.
constexpr Vector3 angleV3(float const& angle, Axis const& axis = Axis::NEG_Z) {
	switch (axis) {
	case Axis::POS_X:
	case Axis::NEG_X:
		return Vector3(0, cos(angle), -sin(angle));
	case Axis::POS_Y:
	case Axis::NEG_Y:
		return Vector3(cos(angle), 0, -sin(angle));
	default:
	case Axis::POS_Z:
	case Axis::NEG_Z:
		return Vector3(cos(angle), -sin(angle), 0);
	}
}

/// Gets a 3D Vector of size 1, pointing towards a given axis, rotated at a given angle.
constexpr Vector3 angleV3(Vector3 const& angle, Axis const& axis = Axis::NEG_Z) {
	switch (axis) {
	case Axis::POS_X:
		return rotateV3(Vector3(+1,0,0), angle);
	case Axis::NEG_X:
		return rotateV3(Vector3(-1,0,0), angle);
	case Axis::POS_Y:
		return rotateV3(Vector3(0,+1,0), angle);
	case Axis::NEG_Y:
		return rotateV3(Vector3(0,-1,0), angle);
	case Axis::POS_Z:
		return rotateV3(Vector3(0,0,+1), angle);
	default:
	case Axis::NEG_Z:
		return rotateV3(Vector3(0,0,-1), angle);
	}
}

// Cross product

/// Gets the cross product of two 3D Vectors.
constexpr Vector3 crossProd(Vector3 const& a, Vector3 const& b) {
	return a.crossProd(b);
}

// Scale-rotation-position transformation

/// Transforms a given 3D Vector by a given position, rotation and scale.
constexpr Vector3 srpTransform(Vector3 vec, Vector3 const& pos, Vector3 const& rot, Vector3 const& scale = Vector3(1.0)) {
	vec *= scale;
	vec = rotateV3(vec, rot);
	vec += pos;
	return vec;
}

/// Transforms a given 2D Vector by a given position, rotation and scale.
constexpr Vector2 srpTransform(Vector2 vec, Vector2 const& pos, float const& rot, Vector2 const& scale = Vector2(1.0)) {
	vec *= scale;
	vec = rotateV2(vec, rot);
	vec += pos;
	return vec;
}

/// Transforms a given set of 3D Vectors by a given position rotation and scale.
constexpr List<Vector3> srpTransform(List<Vector3> vec, Vector3 const& pos, Vector3 const& rot, Vector3 const& scale = Vector3(1.0)) {
	for (size_t i = 0; i < vec.size(); i++) {
		vec[i] = srpTransform(vec[i], pos, rot, scale);
	}
	return vec;
}

/// Transforms a given set of 2D Vectors by a given position, rotation and scale.
constexpr List<Vector2> srpTransform(List<Vector2> vec, Vector2 const& pos, float const& rot, Vector2 const& scale = Vector2(1.0)) {
	for (size_t i = 0; i < vec.size(); i++) {
		vec[i] = srpTransform(vec[i], pos, rot, scale);
	}
	return vec;
}

// Dot product

/// Gets the dot product between two vectors.
constexpr float dotProd(Vector4 a, Vector4 const& b) {
	return a.dotProd(b);
}

/// Gets the dot product between two vectors.
constexpr float dotProd(Vector3 a, Vector3 const& b) {
	return a.dotProd(b);
}

/// Gets the dot product between two vectors.
constexpr float dotProd(Vector2 a, Vector2 const& b) {
	return a.dotProd(b);
}

// Angle between Vectors calculation

/// Gets the angle from the origin to a given Vector.
constexpr float angleTo(Vector2 const& vec) {
	return vec.angle();
}

/// Gets the angle from Vector A to Vector B.
constexpr float angleTo(Vector2 const& a, Vector2 const& b) {
	return a.angleTo(b);
}

/// Gets the angle from the origin to a given Vector.
constexpr Vector3 angleTo(Vector3 const& vec) {
	return vec.angle();
}

/// Gets the angle from Vector A to Vector B.
constexpr Vector3 angleTo(Vector3 const& a, Vector3 const& b) {
	return a.angleTo(b);
}

/// Gets the normal pointing from the origin towards a given Vector.
constexpr Vector2 normalTo(Vector2 const& vec) {
	return vec.normalized();
}

/// Gets the normal pointing from Vector A to Vector B.
constexpr Vector2 normalTo(Vector2 const& a, Vector2 const& b) {
	return a.normalTo(b);
}

/// Gets the normal pointing from the origin towards a given Vector.
constexpr Vector3 normalTo(Vector3 const& vec) {
	return vec.normalized();
}

/// Gets the normal pointing from Vector A to Vector B.
constexpr Vector3 normalTo(Vector3 const& a, Vector3 const& b) {
	return a.normalTo(b);
}

// Normal reflection

/// Reflects a given normal in accordance to a surface normal.
constexpr Vector2 reflect(Vector2 const& normal, Vector2 const& surface) {
	return surface * (normal - 2.0 * dotProd(normal, surface));
}

// Distance between vectors calculation

/// Gets the distance from Vector A to Vector B.
constexpr float distanceTo(Vector2 const& a, Vector2 const& b) {
	return a.distanceTo(b);
}

/// Gets the distance from Vector A to Vector B.
constexpr float distanceTo(Vector3 const& a, Vector3 const& b) {
	return a.distanceTo(b);
}

/// Gets the distance from Vector A to Vector B.
constexpr float distanceTo(Vector4 const& a, Vector4 const& b) {
	return a.distanceTo(b);
}

/// Gets the distance squared from Vector A to Vector B.
constexpr float squaredDistanceTo(Vector2 const& a, Vector2 const& b) {
	return a.squaredDistanceTo(b);
}

/// Gets the distance squared from Vector A to Vector B.
constexpr float squaredDistanceTo(Vector3 const& a, Vector3 const& b) {
	return a.squaredDistanceTo(b);
}

/// Gets the distance squared from Vector A to Vector B.
constexpr float squaredDistanceTo(Vector4 const& a, Vector4 const& b) {
	return a.squaredDistanceTo(b);
}

// Vector linear angle interpolation (by vector)

/// Linearly interpolates two vectors by a certain amount.
constexpr Vector2 angleLerp(Vector2 const& from, Vector2 const& to, Vector2 const& by) {
	Vector2 dist = (to - from) % Math::tau;
	dist = ((dist * 2.0) % Math::tau) - dist;
	return from + dist * by;
}

/// Linearly interpolates two vectors by a certain amount.
constexpr Vector3 angleLerp(Vector3 const& from, Vector3 const& to, Vector3 const& by) {
	Vector3 dist = (to - from) % Math::tau;
	dist = ((dist * 2.0) % Math::tau) - dist;
	return from + dist * by;
}

/// Linearly interpolates two vectors by a certain amount.
constexpr Vector4 angleLerp(Vector4 const& from, Vector4 const& to, Vector4 const& by) {
	Vector4 dist = (to - from) % Math::tau;
	dist = ((dist * 2.0) % Math::tau) - dist;
	return from + dist * by;
}

// Vector linear angle interpolation (by float)

/// Linearly interpolates two vectors by a certain amount.
constexpr Vector2 angleLerp(Vector2 const& from, Vector2 const& to, float const& by) {
	return angleLerp(from, to, Vector2(by));
}

/// Linearly interpolates two vectors by a certain amount.
constexpr Vector3 angleLerp(Vector3 const& from, Vector3 const& to, float const& by) {
	return angleLerp(from, to, Vector3(by));
}

/// Linearly interpolates two vectors by a certain amount.
constexpr Vector4 angleLerp(Vector4 const& from, Vector4 const& to, float const& by) {
	return angleLerp(from, to, Vector4(by));
}

/// Gets the "center" of a given set of points.
constexpr Vector2 center(List<Vector2> const& points) {
	Vector2 result;
	for (Vector2 p: points)
		result += p;
	result /= points.size();
	return result;
}

/// Gets the "center" of a given set of points.
constexpr Vector3 center(List<Vector3> const& points) {
	Vector3 result;
	for (Vector3 p: points)
		result += p;
	result /= points.size();
	return result;
}

/// Gets the "center" of a given set of points.
constexpr Vector4 center(List<Vector4> const& points) {
	Vector4 result;
	for (Vector4 p: points)
		result += p;
	result /= points.size();
	return result;
}

typedef List<Vector2> Points2D;
typedef List<Vector3> Points3D;
typedef List<Vector4> Points4D;

constexpr Vector2 srpTransform(Vector2 const& vec, Transform2D const& trans) {
	return srpTransform(
		vec,
		trans.position,
		trans.rotation,
		trans.scale
	);
}

constexpr Vector3 srpTransform(Vector3 const& vec, Transform3D const& trans) {
	return srpTransform(
		vec,
		trans.position,
		trans.rotation,
		trans.scale
	);
}

static_assert(sizeof(Vector2) == (sizeof(float) * 2), "Vector2 has some size issues...");
static_assert(sizeof(Vector3) == (sizeof(float) * 3), "Vector3 has some size issues...");
static_assert(sizeof(Vector4) == (sizeof(float) * 4), "Vector4 has some size issues...");

#pragma GCC diagnostic pop

}

CTL_EX_NAMESPACE_END

#endif // CTL_EX_LINALG_VECTOR_H
