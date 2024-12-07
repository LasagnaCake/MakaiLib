#ifndef MAKAILIB_GRAPH_CAMERA_H
#define MAKAILIB_GRAPH_CAMERA_H

#include "../../compat/ctl.hpp"

namespace Makai::Graph {
	/// @brief Orthographic projection settings.
	struct OrthographicData {
		/// @brief Projection strength.
		float strength	= 0;
		/// @brief Orthographic origin.
		Vector2 origin	= Vector2(0);
		/// @brief Orthographic size.
		Vector2 size	= Vector2(1);
	};

	/// @brief Converts an orthographic projection into a perspective matrix.
	/// @param data Orthographic projection to convert.
	/// @param zNear Near clip plane.
	/// @param zFar Far clip plane.
	/// @return Resulting matrix.
	constexpr Matrix4x4 asMatrix(OrthographicData const& data, float const zNear, float const zFar) {
		return Math::ortho(
			data.origin.x,
			data.size.x,
			data.size.y,
			data.origin.y,
			zNear,
			zFar
		);
	}
	
	/// @brief 3D camera.
	struct Camera3D {
		Vector3 eye;
		Vector3 at		= Vector3(0, 0, 1);
		Vector3 up		= Vector3(0, 1, 0);
		Vector2 aspect	= Vector2(4.0, 3.0);
		float fov	= 2.0;
		float zNear	= 0.01;
		float zFar	= 100.0;
		OrthographicData ortho;
		/// Whether most points given (3D Perspective) are relative to the eye's position.
		bool relativeToEye = false;

		constexpr Matrix4x4 matrix() const {
			Vector3 camAt = at;
			if (relativeToEye)
				camAt += eye;
			return Math::lookAt(
				eye,
				camAt,
				up
			);
		}

		constexpr Matrix4x4 projection() const {
			if (ortho.strength == 0.0)
				return Math::perspective(
					fov,
					aspect.tangent(),
					zNear,
					zFar
				);
			if (ortho.strength == 1.0)
				return Math::ortho(
					ortho.origin.x,
					ortho.size.x,
					ortho.size.y,
					ortho.origin.y,
					zNear,
					zFar
				);
			Matrix4x4 om = Math::ortho(
				ortho.origin.x,
				ortho.size.x,
				ortho.size.y,
				ortho.origin.y,
				zNear,
				zFar
			);
			Matrix4x4 pm = Math::perspective(
				fov,
				aspect.tangent(),
				zNear,
				zFar
			);
			return Math::lerp<Matrix4x4>(pm, om, ortho.strength);
		}

		constexpr static Camera3D from2D(Vector2 const& size) {
			Camera3D camera;
			camera.eye				= Vec3(0, 0, -100);
			camera.at				= Vec3(0, 0, 1);
			camera.ortho.strength	= 1.0;
			camera.ortho.origin		= 0;
			camera.ortho.size		= size * -1;
			camera.zFar				= 1000.0;
			camera.relativeToEye	= true;
			return camera;
		}

		constexpr static Camera3D from2D(float const scale, Vector2 const& ratio) {
			return Camera3D::from2D(ratio * scale);
		}
	};

	struct GimbalCamera3D {
		Vector3 position;
		Vector3 rotation;
		Vector2 aspect	= Vector2(4.0, 3.0);
		float fov	= 2.0;
		float zNear	= 0.01;
		float zFar	= 100.0;
		OrthographicData ortho;

		constexpr Camera3D toCamera3D() const {
			Vector3 eye	= position;
			Vector3 at	= Vector3(0, -1, 0);
			Vector3 up	= Vector3(0, 0, 1);
			Matrix4x4 const rmat = Matrix4x4::fromEulerYXZ(rotation);
			return Camera3D{
				eye,
				rmat * Vector4(at, 1),
				rmat * Vector4(up, 1),
				aspect,
				fov,
				zNear,
				zFar,
				ortho,
				true
			};
		}

		constexpr operator Camera3D() const {
			return toCamera3D();
		}

		constexpr GimbalCamera3D& fromCamera3D(Camera3D const& cam) {
			auto crot	= Math::getEulerAnglesYXZ(cam.matrix());
			position	= cam.eye;
			rotation	= Vector3(crot.x, crot.y, crot.z);
			aspect		= cam.aspect;
			fov			= cam.fov;
			zNear		= cam.zNear;
			zFar		= cam.zFar;
			ortho		= cam.ortho;
			return (*this);
		}

		constexpr GimbalCamera3D& operator=(Camera3D const& cam) {
			fromCamera3D(cam);
			return (*this);
		}
	};
}

#endif // MAKAILIB_GRAPH_CAMERA_H
