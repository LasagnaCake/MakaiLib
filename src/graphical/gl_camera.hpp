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
		Vector3 up = Vector3(0, 1, 0);
		Vector2 aspect = Vector2(4.0, 3.0);
		float fov = 2.0;
		float zNear = 0.1;
		float zFar = 100.0;
		struct {
			bool enabled	= false;
			Vector2 origin	= Vector2(0);
			Vector2 size	= Vector2(1);
		} ortho;
		/// Whether most points given (3D Perspective) are relative to the eye's position;
		bool relativeToEye = false;

		glm::mat4 matrix() {
			glm::vec3
				camEye = glm::vec3(eye.x, eye.y, eye.z),
				camAt = glm::vec3(at.x, at.y, at.z);
			if (relativeToEye)
				camAt += camEye;
			return glm::lookAt(
				camEye,
				camAt,
				glm::vec3(up.x, up.y, up.z)
			);
		}

		glm::mat4 perspective() {
			if (ortho.enabled)
				return glm::ortho(
					ortho.origin.x,
					ortho.size.x,
					ortho.size.y,
					ortho.origin.y,
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
