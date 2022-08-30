#ifndef MAKAI_VIEWPERSPECTIVE_CAMERA
#define MAKAI_VIEWPERSPECTIVE_CAMERA

#include "../anchors.hpp"

namespace Camera {
	namespace {
		using
		Vector::Vector2,
		Vector::Vector3,
		Vector::Vector4,
		VecMath::Transform,
		VecMath::Transform2D,
		VecMath::Transform3D,
		VecMath::srpTransform,
		std::function,
		std::vector;
	}

	struct Camera3D {
		Vector3 eye;
		Vector3 at;
		Vector3 up;
		Vector2 aspect = Vector2(4.0, 3.0);
		float fov = 2.0;
		float zNear = 0.1;
		float zFar = 100.0;
		struct {
			bool enabled	= false;
			float left		= 1;
			float right		= 1;
			float bottom	= 1;
			float top		= 1;
		} ortho;

		glm::mat4 matrix() {
			return glm::lookAt(
				glm::vec3(eye.x, eye.y, eye.z),
				glm::vec3(at.x, at.y, at.z),
				glm::vec3(up.x, up.y, up.z)
			);
		}

		glm::mat4 perspective() {
			if (ortho.enabled)
				return glm::ortho(
					ortho.left,
					ortho.right,
					ortho.bottom,
					ortho.top,
					zNear,
					zFar
				);
			return glm::perspective(
				fov,
				aspect.x / aspect.y,
				zNear,
				zFar
			);
		}
	};
}

#endif // MAKAI_VIEWPERSPECTIVE_CAMERA
