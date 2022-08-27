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
					this->x + vec.x,
					this->y + vec.y
					);
			}

			Vector2 operator-(Vector2 vec) {
				return Vector2(
					this->x - vec.x,
					this->y - vec.y
					);
			}

			Vector2 operator*(Vector2 vec) {
				return Vector2(
					this->x * vec.x,
					this->y * vec.y
					);
			}

			Vector2 operator/(Vector2 vec) {
				return Vector2(
					this->x / vec.x,
					this->y / vec.y
					);
			}

			Vector2 operator%(Vector2 vec) {
				return Vector2(
					fmod(this->x, vec.x),
					fmod(this->y, vec.y)
					);
			}

			Vector2 operator+=(Vector2 vec) {
				this->x += vec.x;
				this->y += vec.y;
				return *this;
			}

			Vector2 operator-=(Vector2 vec) {
				this->x -= vec.x;
				this->y -= vec.y;
				return *this;
			}

			Vector2 operator*=(Vector2 vec) {
				this->x *= vec.x;
				this->y *= vec.y;
				return *this;
			}

			Vector2 operator/=(Vector2 vec) {
				this->x /= vec.x;
				this->y /= vec.y;
				return *this;
			}

			Vector2 operator%=(Vector2 vec) {
				this->x = fmod(this->x, vec.x);
				this->y = fmod(this->y, vec.y);
				return *this;
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
				return *this / length();
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
				Vector2 v = normalized();
				return v.x / v.y;
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

			Vector3(float x, float y, float z) {
				this->x = x;
				this->y = y;
				this->z = z;
			}

			Vector3(const Vector3& vec) {
				x = vec.x;
				y = vec.y;
				z = vec.z;
			}

			Vector3(Vector2 vec, float z) {
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
					this->x + vec.x,
					this->y + vec.y,
					this->z + vec.z
					);
			}

			Vector3 operator-(Vector3 vec) {
				return Vector3(
					this->x - vec.x,
					this->y - vec.y,
					this->z - vec.z
					);
			}

			Vector3 operator*(Vector3 vec) {
				return Vector3(
					this->x * vec.x,
					this->y * vec.y,
					this->z * vec.z
					);
			}

			Vector3 operator/(Vector3 vec) {
				return Vector3(
					this->x / vec.x,
					this->y / vec.y,
					this->z / vec.z
					);
			}

			Vector3 operator%(Vector3 vec) {
				return Vector3(
					fmod(this->x, vec.x),
					fmod(this->y, vec.y),
					fmod(this->z, vec.z)
					);
			}

			Vector3 operator+=(Vector3 vec) {
				this->x += vec.x;
				this->y += vec.y;
				this->z += vec.z;
				return *this;
			}

			Vector3 operator-=(Vector3 vec) {
				this->x -= vec.x;
				this->y -= vec.y;
				this->z -= vec.z;
				return *this;
			}

			Vector3 operator*=(Vector3 vec) {
				this->x *= vec.x;
				this->y *= vec.y;
				this->z *= vec.z;
				return *this;
			}

			Vector3 operator/=(Vector3 vec) {
				this->x /= vec.x;
				this->y /= vec.y;
				this->z /= vec.z;
				return *this;
			}

			Vector3 operator%=(Vector3 vec) {
				this->x = fmod(this->x, vec.x);
				this->y = fmod(this->y, vec.y);
				this->z = fmod(this->z, vec.z);
				return *this;
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
				return *this / length();
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

			Vector4(float x, float y, float z, float w) {
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
					this->x - vec.x,
					this->y - vec.y,
					this->z - vec.z,
					this->w - vec.w
					);
			}

			Vector4 operator*(Vector4 vec) {
				return Vector4(
					this->x * vec.x,
					this->y * vec.y,
					this->z * vec.z,
					this->w * vec.w
					);
			}

			Vector4 operator/(Vector4 vec) {
				return Vector4(
					this->x / vec.x,
					this->y / vec.y,
					this->z / vec.z,
					this->w / vec.w
					);
			}

			Vector4 operator%(Vector4 vec) {
				return Vector4(
					fmod(this->x, vec.x),
					fmod(this->y, vec.y),
					fmod(this->z, vec.z),
					fmod(this->w, vec.w)
					);
			}

			Vector4 operator+=(Vector4 vec) {
				this->x += vec.x;
				this->y += vec.y;
				this->z += vec.z;
				this->w += vec.w;
				return *this;
			}

			Vector4 operator-=(Vector4 vec) {
				this->x -= vec.x;
				this->y -= vec.y;
				this->z -= vec.z;
				this->w -= vec.w;
				return *this;
			}

			Vector4 operator*=(Vector4 vec) {
				this->x *= vec.x;
				this->y *= vec.y;
				this->z *= vec.z;
				this->w *= vec.w;
				return *this;
			}

			Vector4 operator/=(Vector4 vec) {
				this->x /= vec.x;
				this->y /= vec.y;
				this->z /= vec.z;
				this->w /= vec.w;
				return *this;
			}

			Vector4 operator%=(Vector4 vec) {
				this->x = fmod(this->x, vec.x);
				this->y = fmod(this->y, vec.y);
				this->z = fmod(this->z, vec.z);
				this->w = fmod(this->w, vec.w);
				return *this;
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

			Vector4 operator%=(float val) {
				return *this %= Vector4(val);
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
				return *this / length();
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
	};
}

namespace VecMath
{
	namespace {
		using std::vector;
	}

	using namespace Vector;

	enum : unsigned char
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z
	};

	// Vector rotation

	/// Rotates a 2D Vector around the origin by a given angle.
	Vector2 rotateV2(Vector2 vec, float angle) {
		// Calculate the sine & cosine of the angle
		float
			sinA = sin(angle),
			cosA = cos(angle);
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
		// Get the vector's positions
		float
			vx = vec.x,
			vy = vec.y,
			vz = vec.z;
		// Calculate the sines
		float
			sinX = sin(angle.x),
			sinY = sin(angle.y),
			sinZ = sin(angle.z);
		// Calculate the cosines
		float
			cosX = cos(angle.x),
			cosY = cos(angle.y),
			cosZ = cos(angle.z);
		// Calculate rotation around Z axis
		vec.z = (
			(((cosX * cosY)) * vz)
		+	(((cosX * sinY * sinZ) - (cosX * sinZ)) * vy)
		+	(((sinX * sinZ) + (cosX * sinY * cosZ)) * vx)
		);
		// Calculate rotation around Y axis
		vec.y = (
			(((sinX * cosY)) * vz)
		+	(((cosX * cosZ) + (sinX * sinY * sinZ)) * vy)
		+	(((sinX * sinY * cosZ) + (cosX * sinZ)) * vx)
		);
		// Calculate rotation around X axis
		vec.x = (
		-	(sinY * vz)
		+	((cosY * sinZ) * vy)
		+	((cosY * cosZ) * vx)
		);
		// Return rotated vector
		return vec;
	}

	/**
	* Rotates a given 3D Vector around two of the origin's axis by two respective angles.
	* The given axis is the axis to be excluded.
	*/
	Vector3 rotateV3(Vector3 vec, Vector2 angle, unsigned char exclude = AXIS_X) {
		switch (exclude) {
		case AXIS_X:
			return rotateV3(vec, Vector3(0, angle.x, angle.y));
		case AXIS_Y:
		default:
			return rotateV3(vec, Vector3(angle.x, 0, angle.y));
		case AXIS_Z:
			return rotateV3(vec, Vector3(angle.x, angle.y, 0));
		}
	}

	// No fourth-dimensional rotation.

	// Angle-to-vector conversion

	/// Gets a 2D Vector of size 1 at a given angle relative to the origin.
	Vector2 angleV2(float angle) {
		return Vector2(sin(angle), -cos(angle));
	}

	/// Gets a 3D Vector of size 1 at a given angle around one of the origin's axis.
	Vector3 angleV3(float angle, unsigned char axis) {
		switch (axis) {
		case AXIS_X:
			return Vector3(0, sin(angle), -cos(angle));
		case AXIS_Y:
		default:
			return Vector3(sin(angle), 0, -cos(angle));
		case AXIS_Z:
			return Vector3(sin(angle), -cos(angle), 0);
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

	/// Gets the angle from Vector A to Vector B.
	float angleTo(Vector2 a, Vector2 b) {
		return atan2(b.y - a.y, b.x - a.x) - (Math::hpi);
	}

	/// Gets the angle from Vector A to Vector B.
	Vector3 angleTo(Vector3 a, Vector3 b) {
		// TODO: Test this
		a.x = atan(b.x - a.x);
		a.y = atan(b.y - a.y);
		a.z = atan(b.z - a.z);
		return a;
	}

	/// Gets the angle from Vector A to Vector B.
	Vector4 angleTo(Vector4 a, Vector4 b) {
		// TODO: Test this
		a.x = atan(b.x - a.x);
		a.y = atan(b.y - a.y);
		a.z = atan(b.z - a.z);
		a.w = atan(b.w - a.w);
		return a;
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
		T		position;
		ROT_T	rotation;
		T		scale;
	};

	typedef Transform<VecV2, float>	Transform2D;
	typedef Transform<VecV3, VecV3>	Transform3D;

	VecV2 srpTransform(VecV2 vec, Transform2D trans) {
		return srpTransform(
			vec,
			trans.position,
			trans.rotation,
			trans.scale
		);
	}

	VecV3 srpTransform(VecV3 vec, Transform3D trans) {
		return srpTransform(
			vec,
			trans.position,
			trans.rotation,
			trans.scale
		);
	}
}

#endif // VECTOR234_H
