#ifndef MAKAILIB_GRAPH_CAMERA_H
#define MAKAILIB_GRAPH_CAMERA_H

#include "../../ctl/ctl.hpp"

namespace Makai::Graph {
	struct OrthographicData {
		float strength	= 0;
		Vector2 origin	= Vector2(0);
		Vector2 size	= Vector2(1);
	};

	constexpr Matrix4x4 asMatrix(OrthographicData const& data, float const& zNear, float const& zFar);

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

		constexpr Matrix4x4 matrix() const;

		constexpr Matrix4x4 projection() const;

		constexpr static Camera3D from2D(Vector2 const& size);

		constexpr static Camera3D from2D(float const& scale, Vector2 const& ratio);
	};

	struct GimbalCamera3D {
		Vector3 position;
		Vector3 rotation;
		Vector2 aspect	= Vector2(4.0, 3.0);
		float fov	= 2.0;
		float zNear	= 0.01;
		float zFar	= 100.0;
		OrthographicData ortho;

		constexpr Camera3D toCamera3D() const;

		constexpr operator Camera3D() const;

		constexpr GimbalCamera3D& fromCamera3D(Camera3D const& cam);

		constexpr GimbalCamera3D& operator=(Camera3D const& cam);
	};
}

#endif // MAKAILIB_GRAPH_CAMERA_H
