#ifndef FLOAT_VECTOR234_H
#define FLOAT_VECTOR234_H

#include <cmath>
#include <vector>
#include "algebra.hpp"

namespace Vector{
	/**
	* [-----------------]
	* [                 ]
	* [  Vector2 Class  ]
	* [                 ]
	* [-----------------]
	*/
	class Vector2
	{
		public:
			/// The vector's position.
			float x, y;

			/// Consturctors.
			Vector2() {
				x = 0;
				y = 0;
			}

			Vector2(float v) {
				x = v;
				y = v;
			}

			Vector2(float x, float y) {
				this->x = x;
				this->y = y;
			}

			Vector2(const Vector2& vec) {
				x = vec.x;
				y = vec.y;
			}

			/// Destructor.
			virtual ~Vector2() {

			}

			/// Vector operator overloading.
			Vector2 operator+(Vector2 vec) {
				return Vector2(
					x + vec.x,
					y + vec.y
					);
			}

			Vector2 operator-(Vector2 vec) {
				return Vector2(
					x - vec.x,
					y - vec.y
					);
			}

			Vector2 operator*(Vector2 vec) {
				return Vector2(
					x * vec.x,
					y * vec.y
					);
			}

			Vector2 operator/(Vector2 vec) {
				return Vector2(
					x / vec.x,
					y / vec.y
					);
			}

			Vector2 operator%(Vector2 vec) {
				return Vector2(
					fmod(x, vec.x),
					fmod(y, vec.y)
					);
			}

			Vector2 operator^(Vector2 vec) {
				return Vector2(
					pow(x, vec.x),
					pow(y, vec.y)
					);
			}

			Vector2 operator+=(Vector2 vec) {
				x += vec.x;
				y += vec.y;
				return *this;
			}

			Vector2 operator-=(Vector2 vec) {
				x -= vec.x;
				y -= vec.y;
				return *this;
			}

			Vector2 operator*=(Vector2 vec) {
				x *= vec.x;
				y *= vec.y;
				return *this;
			}

			Vector2 operator/=(Vector2 vec) {
				x /= vec.x;
				y /= vec.y;
				return *this;
			}

			Vector2 operator%=(Vector2 vec) {
				x = fmod(x, vec.x);
				y = fmod(y, vec.y);
				return *this;
			}

			Vector2 operator^=(Vector2 vec) {
				x = fmod(x, vec.x);
				y = fmod(y, vec.y);
				return *this;
			}

			bool operator==(Vector2 vec) {
				return (
					x == vec.x &&
					y == vec.y
				);
			}

			bool operator!=(Vector2 vec) {
				return !(*this == vec);
			}

			/// Float operator overloading.
			Vector2 operator+(float val) {
				return *this + Vector2(val);
			}

			Vector2 operator-(float val) {
				return *this - Vector2(val);
			}

			Vector2 operator*(float val) {
				return *this * Vector2(val);
			}

			Vector2 operator/(float val) {
				return *this / Vector2(val);
			}

			Vector2 operator%(float val) {
				return *this % Vector2(val);
			}

			Vector2 operator^(float val) {
				return *this ^ Vector2(val);
			}

			Vector2 operator+=(float val) {
				return *this += Vector2(val);
			}

			Vector2 operator-=(float val) {
				return *this -= Vector2(val);
			}

			Vector2 operator*=(float val) {
				return *this *= Vector2(val);
			}

			Vector2 operator/=(float val) {
				return *this /= Vector2(val);
			}

			Vector2 operator%=(float val) {
				return *this %= Vector2(val);
			}

			Vector2 operator^=(float val) {
				return *this ^= Vector2(val);
			}

			// Other overloads

			float& operator[](char pos) {
				switch (pos) {
				case 0:
				case 'x':
				case 'X':
				default:
					return x;
				case 1:
				case 'y':
				case 'Y':
					return y;
				}
			}

			// Extra functions

			/// Gets the vector's length (distance to origin).
			float length() {
				return sqrt((x * x) + (y * y));
			}

			/// Gets the vector's length (distance to origin).
			float lengthSquared() {
				return ((x * x) + (y * y));
			}

			/// Gets the normalized vector.
			Vector2 normalized() {
				if (*this != 0)
					return *this / length();
				else
					return *this;
			}

			/// Gets the distance to another vector.
			float distanceTo(Vector2 vec) {
				Vector2 diff = vec - *this;
				return diff.length();
			}

			/// Gets the distance to another vector.
			float squaredDistanceTo(Vector2 vec) {
				Vector2 diff = vec - *this;
				return diff.lengthSquared();
			}

			/// Clamps the vector between two values.
			void clamp(Vector2 min, Vector2 max) {
				x = Math::clamp(x, min.x, max.x);
				y = Math::clamp(y, min.y, max.y);
			}

			/// Returns the vector clamped between two values.
			Vector2 clamped(Vector2 min, Vector2 max) {
				return Vector2(
					Math::clamp(x, min.x, max.x),
					Math::clamp(y, min.y, max.y)
				);
			}

			/// Gets the tangent of the vector.
			float tangent() {
				return x / y;
			}

			Vector2 absolute() {
				return Vector2(
					abs(x),
					abs(y)
				);
			}

			Vector2 yx() {
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

	class Vector3
	{
		public:
			/// The vector's position.
			float x, y, z;

			/// Constructors.

			Vector3() {
				x = 0;
				y = 0;
				z = 0;
			}

			Vector3(float v) {
				x = v;
				y = v;
				z = v;
			}

			Vector3(float x, float y, float z = 0.0) {
				this->x = x;
				this->y = y;
				this->z = z;
			}

			Vector3(const Vector3& vec) {
				x = vec.x;
				y = vec.y;
				z = vec.z;
			}

			Vector3(Vector2 vec, float z = 0.0) {
				x = vec.x;
				y = vec.y;
				this->z = z;
			}

			/// Destructor.

			virtual ~Vector3() {

			}

			/// Vector operator overloading.

			Vector3 operator+(Vector3 vec) {
				return Vector3(
					x + vec.x,
					y + vec.y,
					z + vec.z
					);
			}

			Vector3 operator-(Vector3 vec) {
				return Vector3(
					x - vec.x,
					y - vec.y,
					z - vec.z
					);
			}

			Vector3 operator*(Vector3 vec) {
				return Vector3(
					x * vec.x,
					y * vec.y,
					z * vec.z
					);
			}

			Vector3 operator/(Vector3 vec) {
				return Vector3(
					x / vec.x,
					y / vec.y,
					z / vec.z
					);
			}

			Vector3 operator%(Vector3 vec) {
				return Vector3(
					fmod(x, vec.x),
					fmod(y, vec.y),
					fmod(z, vec.z)
					);
			}

			Vector3 operator^(Vector3 vec) {
				return Vector3(
					pow(x, vec.x),
					pow(y, vec.y),
					pow(z, vec.z)
					);
			}

			Vector3 operator+=(Vector3 vec) {
				x += vec.x;
				y += vec.y;
				z += vec.z;
				return *this;
			}

			Vector3 operator-=(Vector3 vec) {
				x -= vec.x;
				y -= vec.y;
				z -= vec.z;
				return *this;
			}

			Vector3 operator*=(Vector3 vec) {
				x *= vec.x;
				y *= vec.y;
				z *= vec.z;
				return *this;
			}

			Vector3 operator/=(Vector3 vec) {
				x /= vec.x;
				y /= vec.y;
				z /= vec.z;
				return *this;
			}

			Vector3 operator%=(Vector3 vec) {
				x = fmod(x, vec.x);
				y = fmod(y, vec.y);
				z = fmod(z, vec.z);
				return *this;
			}

			Vector3 operator^=(Vector3 vec) {
				x = pow(x, vec.x);
				y = pow(y, vec.y);
				z = pow(z, vec.z);
				return *this;
			}

			bool operator==(Vector3 vec) {
				return (
					x == vec.x &&
					y == vec.y &&
					y == vec.z
				);
			}

			bool operator!=(Vector3 vec) {
				return !(*this == vec);
			}

			/// Float operator overloading.

			Vector3 operator+(float val) {
				return *this + Vector3(val);
			}

			Vector3 operator-(float val) {
				return *this - Vector3(val);
			}

			Vector3 operator*(float val) {
				return *this * Vector3(val);
			}

			Vector3 operator/(float val) {
				return *this / Vector3(val);
			}

			Vector3 operator%(float val) {
				return *this % Vector3(val);
			}

			Vector3 operator^(float val) {
				return *this ^ Vector3(val);
			}

			Vector3 operator+=(float val) {
				return *this += Vector3(val);
			}

			Vector3 operator-=(float val) {
				return *this -= Vector3(val);
			}

			Vector3 operator*=(float val) {
				return *this *= Vector3(val);
			}

			Vector3 operator/=(float val) {
				return *this /= Vector3(val);
			}


			Vector3 operator%=(float val) {
				return *this %= Vector3(val);
			}

			Vector3 operator^=(float val) {
				return *this ^= Vector3(val);
			}

			// Other overloads

			float& operator[](char pos) {
				switch (pos) {
				case 0:
				case 'x':
				case 'X':
				default:
					return x;
				case 1:
				case 'y':
				case 'Y':
					return y;
				case 2:
				case 'z':
				case 'Z':
					return z;
				}
			}

			// Extra functions

			/// Gets the vector's length (distance to origin).
			float length() {
				return sqrt((x * x) + (y * y) + (z * z));
			}

			/// Gets the vector's length (distance to origin).
			float lengthSquared() {
				return ((x * x) + (y * y) + (z * z));
			}

			/// Gets the normalized vector.
			Vector3 normalized() {
				if (*this != 0)
					return *this / length();
				else
					return *this;
			}

			/// Gets the distance to another vector.
			float distanceTo(Vector3 vec) {
				Vector3 diff = vec - *this;
				return diff.length();
			}

			/// Gets the distance to another vector.
			float squaredDistanceTo(Vector3 vec) {
				Vector3 diff = vec - *this;
				return diff.lengthSquared();
			}

			/// Clamps the vector between two values.
			void clamp(Vector3 min, Vector3 max) {
				x = Math::clamp(x, min.x, max.x);
				y = Math::clamp(y, min.y, max.y);
				z = Math::clamp(z, min.z, max.z);
			}

			/// Returns the vector clamped between two values.
			Vector3 clamped(Vector3 min, Vector3 max) {
				return Vector3(
					Math::clamp(x, min.x, max.x),
					Math::clamp(y, min.y, max.y),
					Math::clamp(z, min.z, max.z)
				);
			}

			Vector2 xy() {
				return Vector2(x, y);
			}

			Vector2 yz() {
				return Vector2(x, y);
			}

			Vector2 xz() {
				return Vector2(x, y);
			}

			Vector3 zyx() {
				return Vector3(z, y, x);
			}

			Vector3 absolute() {
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
	class Vector4
	{
		public:
			/// The vector's position.
			float  x, y, z, w;

			/// Constructors.

			Vector4() {
				x = 0;
				y = 0;
				z = 0;
				w = 0;
			}

			Vector4(float v) {
				x = v;
				y = v;
				z = v;
				w = v;
			}

			Vector4(float x, float y, float z, float w = 0.0) {
				this->x = x;
				this->y = y;
				this->z = z;
				this->w = w;
			}

			Vector4(const Vector4& vec) {
				x = vec.x;
				y = vec.y;
				z = vec.z;
				w = vec.w;
			}

			Vector4(Vector3 vec, float w = 0) {
				x = vec.x;
				y = vec.y;
				z = vec.z;
				this->w = w;
			}

			Vector4(Vector2 vec, float z = 0, float w = 0) {
				x = vec.x;
				y = vec.y;
				this->z = z;
				this->w = w;
			}

			/// Destructor.

			virtual ~Vector4() {

			}

			/// Vector operator overloading.

			Vector4 operator+(Vector4 vec) {
				return Vector4(
					x + vec.x,
					y + vec.y,
					z + vec.z,
					w + vec.w
					);
			}

			Vector4 operator-(Vector4 vec) {
				return Vector4(
					x - vec.x,
					y - vec.y,
					z - vec.z,
					w - vec.w
					);
			}

			Vector4 operator*(Vector4 vec) {
				return Vector4(
					x * vec.x,
					y * vec.y,
					z * vec.z,
					w * vec.w
					);
			}

			Vector4 operator/(Vector4 vec) {
				return Vector4(
					x / vec.x,
					y / vec.y,
					z / vec.z,
					w / vec.w
					);
			}

			Vector4 operator%(Vector4 vec) {
				return Vector4(
					fmod(x, vec.x),
					fmod(y, vec.y),
					fmod(z, vec.z),
					fmod(w, vec.w)
					);
			}

			Vector4 operator^(Vector4 vec) {
				return Vector4(
					pow(x, vec.x),
					pow(y, vec.y),
					pow(z, vec.z),
					pow(w, vec.w)
					);
			}

			Vector4 operator+=(Vector4 vec) {
				x += vec.x;
				y += vec.y;
				z += vec.z;
				w += vec.w;
				return *this;
			}

			Vector4 operator-=(Vector4 vec) {
				x -= vec.x;
				y -= vec.y;
				z -= vec.z;
				w -= vec.w;
				return *this;
			}

			Vector4 operator*=(Vector4 vec) {
				x *= vec.x;
				y *= vec.y;
				z *= vec.z;
				w *= vec.w;
				return *this;
			}

			Vector4 operator/=(Vector4 vec) {
				x /= vec.x;
				y /= vec.y;
				z /= vec.z;
				w /= vec.w;
				return *this;
			}

			Vector4 operator%=(Vector4 vec) {
				x = fmod(x, vec.x);
				y = fmod(y, vec.y);
				z = fmod(z, vec.z);
				w = fmod(w, vec.w);
				return *this;
			}

			Vector4 operator^=(Vector4 vec) {
				x = pow(x, vec.x);
				y = pow(y, vec.y);
				z = pow(z, vec.z);
				w = pow(w, vec.w);
				return *this;
			}

			bool operator==(Vector4 vec) {
				return (
					x == vec.x &&
					y == vec.y &&
					y == vec.z &&
					y == vec.w
				);
			}

			bool operator!=(Vector4 vec) {
				return !(*this == vec);
			}

			/// Float operator overloading.

			Vector4 operator+(float val) {
				return *this + Vector4(val);
			}

			Vector4 operator-(float val) {
				return *this - Vector4(val);
			}

			Vector4 operator*(float val) {
				return *this * Vector4(val);
			}

			Vector4 operator/(float val) {
				return *this / Vector4(val);
			}

			Vector4 operator%(float val) {
				return *this % Vector4(val);
			}

			Vector4 operator^(float val) {
				return *this ^ Vector4(val);
			}

			Vector4 operator+=(float val) {
				return *this += Vector4(val);
			}

			Vector4 operator-=(float val) {
				return *this -= Vector4(val);
			}

			Vector4 operator*=(float val) {
				return *this *= Vector4(val);
			}

			Vector4 operator/=(float val) {
				return *this /= Vector4(val);
			}

			Vector4 operator^=(float val) {
				return *this ^= Vector4(val);
			}

			// Other overloads

			float& operator[](char pos) {
				switch (pos) {
				case 0:
				case 'x':
				case 'X':
				default:
					return x;
				case 1:
				case 'y':
				case 'Y':
					return y;
				case 2:
				case 'z':
				case 'Z':
					return z;
				case 3:
				case 'w':
				case 'W':
					return w;
				}
			}

			// Extra functions

			/// Gets the vector's length (distance to origin).
			float length() {
				return sqrt((x * x) + (y * y) + (z * z) + (w * w));
			}

			/// Gets the vector's length (distance to origin).
			float lengthSquared() {
				return ((x * x) + (y * y) + (z * z) + (w * w));
			}

			/// Gets the normalized vector.
			Vector4 normalized() {
				if (*this != 0)
					return *this / length();
				else
					return *this;
			}

			/// Gets the distance to another vector.
			float distanceTo(Vector4 vec) {
				Vector4 diff = vec - *this;
				return diff.length();
			}

			/// Gets the distance to another vector.
			float squaredDistanceTo(Vector4 vec) {
				Vector4 diff = vec - *this;
				return diff.lengthSquared();
			}

			/// Clamps the vector between two values.
			void clamp(Vector4 min, Vector4 max) {
				x = Math::clamp(x, min.x, max.x);
				y = Math::clamp(y, min.y, max.y);
				z = Math::clamp(z, min.z, max.z);
				z = Math::clamp(w, min.w, max.w);
			}

			/// Returns the vector clamped between two values.
			Vector4 clamped(Vector4 min, Vector4 max) {
				return Vector4(
					Math::clamp(x, min.x, max.x),
					Math::clamp(y, min.y, max.y),
					Math::clamp(z, min.z, max.z),
					Math::clamp(w, min.w, max.w)
				);
			}

			Vector4 absolute() {
				return Vector4(
					abs(x),
					abs(y),
					abs(z),
					abs(w)
				);
			}

			Vector3 xyz() {
				return Vector3(x, y, z);
			}

			Vector4 wzyx() {
				return Vector4(w, z, y, x);
			}

			Vector4 wxyz() {
				return Vector4(w, x, y, z);
			}

			Vector4 compensated() {
				return Vector4(xyz() / w, w);
			}
	};
}

#define $vec	Vector::
#define $vec2	Vector::Vector2
#define $vec3	Vector::Vector3
#define $vec4	Vector::Vector4

#define Vec2	Vector::Vector2
#define Vec3	Vector::Vector3
#define Vec4	Vector::Vector4

namespace VecMath
{
	using namespace Vector;

	enum class Axis: size_t
	{
		POS_X,
		POS_Y,
		POS_Z,
		NEG_X,
		NEG_Y,
		NEG_Z
	};

	namespace {
		using std::vector;
	}

	// Vector rotation

	/// Rotates a 2D Vector around the origin by a given angle.
	Vector2 rotateV2(Vector2 vec, float angle) {
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
	Vector3 rotateV3(Vector3 vec, Vector3 angle) {
		/*
		* Based off of 3D Rotation Matrices:
		* https://en.wikipedia.org/wiki/Rotation_matrix#General_rotations
		* Most likely follows Tait-Bryan Angles:
		* https://en.wikipedia.org/wiki/Euler_angles#Tait%E2%80%93Bryan_angles
		*/
		// Get a copy of the vector
		Vector3 res = vec;
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
		// Return result
		return res;
	}

	/**
	* Rotates a given 3D Vector around two of the origin's axis by two respective angles.
	* The given axis is the axis to be excluded.
	*/
	Vector3 rotateV3(Vector3 vec, Vector2 angle, Axis exclude = Axis::POS_X) {
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
	Vector2 angleV2(float angle) {
		return Vector2(cos(angle), -sin(angle));
	}

	/// Gets a 3D Vector of size 1 at a given angle around one of the origin's axis.
	Vector3 angleV3(float angle, Axis axis = Axis::NEG_Z) {
		switch (axis) {
		case Axis::POS_X:
		case Axis::NEG_X:
			return Vector3(0, cos(angle), sin(angle));
		case Axis::POS_Y:
		case Axis::NEG_Y:
			return Vector3(cos(angle), 0, sin(angle));
		default:
		case Axis::POS_Z:
		case Axis::NEG_Z:
			return Vector3(cos(angle), sin(angle), 0);
		}
	}

	/// Gets a 3D Vector of size 1 at a given angle.
	Vector3 angleV3(Vec3 angle, Axis axis = Axis::NEG_Z) {
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

	/* TODO This, but properly

	/// Gets the cross product of two 3D Vectors.
	Vector3 crossProd(Vector3 a, Vector3 b) {
		return Vector3(
			(a.y * b.z) + (a.z * b.y),
			(a.z * b.x) + (a.x * b.z),
			(a.x * b.y) + (a.y * b.x)
		);
	}

	*/

	// Scale-rotation-position transformation

	/// Transforms a given 3D Vector by a given position, rotation and scale.
	Vector3 srpTransform(Vector3 vec, Vector3 pos, Vector3 rot, Vector3 scale = Vector3(1.0)) {
		vec *= scale;
		vec = rotateV3(vec, rot);
		vec += pos;
		return vec;
	}

	/// Transforms a given 2D Vector by a given position, rotation and scale.
	Vector2 srpTransform(Vector2 vec, Vector2 pos, float rot, Vector2 scale = Vector2(1.0)) {
		vec *= scale;
		vec = rotateV2(vec, rot);
		vec += pos;
		return vec;
	}

	/// Transforms a given set of 3D Vectors by a given position rotation and scale.
	vector<Vector3> srpTransform(vector<Vector3> vec, Vector3 pos, Vector3 rot, Vector3 scale = Vector3(1.0)) {
		for (size_t i = 0; i < vec.size(); i++) {
			vec[i] = srpTransform(vec[i], pos, rot, scale);
		}
		return vec;
	}

	/// Transforms a given set of 2D Vectors by a given position, rotation and scale.
	vector<Vector2> srpTransform(vector<Vector2> vec, Vector2 pos, float rot, Vector2 scale = Vector2(1.0)) {
		for (size_t i = 0; i < vec.size(); i++) {
			vec[i] = srpTransform(vec[i], pos, rot, scale);
		}
		return vec;
	}

	// Dot product

	/// Gets the dot product between two vectors.
	float dotProd(Vector4 a, Vector4 b) {
		a *= b;
		return a.x + a.y + a.z + a.w;
	}

	/// Gets the dot product between two vectors.
	float dotProd(Vector3 a, Vector3 b) {
		a *= b;
		return a.x + a.y + a.z;
	}

	/// Gets the dot product between two vectors.
	float dotProd(Vector2 a, Vector2 b) {
		a *= b;
		return a.x + a.y;
	}

	// Angle between Vectors calculation

	/// Gets the angle from the origin to a given Vector.
	inline float angleTo(Vector2 vec) {
		return - atan2(vec.x, vec.y) + Math::hpi;
	}

	/// Gets the angle from Vector A to Vector B.
	inline float angleTo(Vector2 a, Vector2 b) {
		return angleTo(a - b);
	}

	/// Gets the angle from the origin to a given Vector.
	Vector3 angleTo(Vector3 vec) {
		// TODO: Test this
		Vector3 res;
		float mag = vec.length();
		res.x = acos(vec.x/mag);
		res.y = acos(vec.y/mag);
		res.z = acos(vec.z/mag);
		return res;
	}

	/// Gets the angle from Vector A to Vector B.
	inline Vector3 angleTo(Vector3 a, Vector3 b) {
		return angleTo(a - b);
	}

	/// Gets the normal pointing from the origin towards a given Vector.
	inline Vector2 normalTo(Vector2 vec) {
		return angleV2(angleTo(vec));
	}

	/// Gets the normal pointing from Vector A to Vector B.
	inline Vector2 normalTo(Vector2 a, Vector2 b) {
		return normalTo(a - b);
	}

	// Distance between vectors calculation

	/// Gets the distance from Vector A to Vector B.
	float distanceTo(Vector2 a, Vector2 b) {
		return a.distanceTo(b);
	}

	/// Gets the distance from Vector A to Vector B.
	float distanceTo(Vector3 a, Vector3 b) {
		return a.distanceTo(b);
	}

	/// Gets the distance from Vector A to Vector B.
	float distanceTo(Vector4 a, Vector4 b) {
		return a.distanceTo(b);
	}

	/// Gets the distance squared from Vector A to Vector B.
	float squaredDistanceTo(Vector2 a, Vector2 b) {
		return a.squaredDistanceTo(b);
	}

	/// Gets the distance squared from Vector A to Vector B.
	float squaredDistanceTo(Vector3 a, Vector3 b) {
		return a.squaredDistanceTo(b);
	}

	/// Gets the distance squared from Vector A to Vector B.
	float squaredDistanceTo(Vector4 a, Vector4 b) {
		return a.squaredDistanceTo(b);
	}

	// Vector linear angle interpolation (by vector)

	/// Linearly interpolates two vectors by a certain amount.
	Vector2 angleLerp(Vector2 from, Vector2 to, Vector2 by) {
		Vector2 dist = (to - from) % Math::tau;
		dist = ((dist * 2.0) % Math::tau) - dist;
		return from + dist * by;
	}

	/// Linearly interpolates two vectors by a certain amount.
	Vector3 angleLerp(Vector3 from, Vector3 to, Vector3 by) {
		Vector3 dist = (to - from) % Math::tau;
		dist = ((dist * 2.0) % Math::tau) - dist;
		return from + dist * by;
	}

	/// Linearly interpolates two vectors by a certain amount.
	Vector4 angleLerp(Vector4 from, Vector4 to, Vector4 by) {
		Vector4 dist = (to - from) % Math::tau;
		dist = ((dist * 2.0) % Math::tau) - dist;
		return from + dist * by;
	}

	// Vector linear angle interpolation (by float)

	/// Linearly interpolates two vectors by a certain amount.
	Vector2 angleLerp(Vector2 from, Vector2 to, float by) {
		return angleLerp(from, to, Vector2(by));
	}

	/// Linearly interpolates two vectors by a certain amount.
	Vector3 angleLerp(Vector3 from, Vector3 to, float by) {
		return angleLerp(from, to, Vector3(by));
	}

	/// Linearly interpolates two vectors by a certain amount.
	Vector4 angleLerp(Vector4 from, Vector4 to, float by) {
		return angleLerp(from, to, Vector4(by));
	}

	/// Gets the "center" of a given set of points.
	Vector2 center(vector<Vector2> points) {
		Vector2 result;
		for (Vector2 p: points)
			result += p;
		result /= points.size();
		return result;
	}

	/// Gets the "center" of a given set of points.
	Vector3 center(vector<Vector3> points) {
		Vector3 result;
		for (Vector3 p: points)
			result += p;
		result /= points.size();
		return result;
	}

	/// Gets the "center" of a given set of points.
	Vector4 center(vector<Vector4> points) {
		Vector4 result;
		for (Vector4 p: points)
			result += p;
		result /= points.size();
		return result;
	}

	typedef vector<Vector2> Points2D;
	typedef vector<Vector3> Points3D;
	typedef vector<Vector4> Points4D;

	/// Base transformation data structure.
	template <class T, typename ROT_T>
	struct Transform {
		Transform() {}
		Transform(T position, ROT_T rotation, T scale) {
			this->position	= position;
			this->rotation	= rotation;
			this->scale		= scale;
		}
		T		position = T(0.0);
		ROT_T	rotation = ROT_T(0.0);
		T		scale = T(1.0);
	};

	typedef Transform<Vector2, float>	Transform2D;
	typedef Transform<Vector3, Vector3>	Transform3D;

	inline Vector2 srpTransform(Vector2 vec, Transform2D trans) {
		return srpTransform(
			vec,
			trans.position,
			trans.rotation,
			trans.scale
		);
	}

	inline Vector3 srpTransform(Vector3 vec, Transform3D trans) {
		return srpTransform(
			vec,
			trans.position,
			trans.rotation,
			trans.scale
		);
	}
}

#define $vmt VecMath::

#endif // VECTOR234_H
