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
		Vector3 at		= Vector3(0, 0, 1);
		Vector3 up		= Vector3(0, 1, 0);
		Vector2 aspect	= Vector2(4.0, 3.0);
		float fov	= 2.0;
		float zNear	= 0.01;
		float zFar	= 100.0;
		struct {
			bool enabled	= false;
			Vector2 origin	= Vector2(0);
			Vector2 size	= Vector2(1);
		} ortho;
		/// Whether most points given (3D Perspective) are relative to the eye's position.
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
				aspect.tangent(),
				zNear,
				zFar
			);
		}
	};

	Camera3D getCamera2D(Vector2 size) {
		Camera3D camera;
		camera.eye = Vec3(0, 0, -1);
		camera.at = Vec3(0, 0, 0);
		camera.ortho.enabled = true;
		camera.ortho.origin = 0;
		camera.ortho.size = size * -1;
		return camera;
	}

	Camera3D getCamera2D(float scale, Vector2 ratio) {
		Camera3D camera;
		camera.eye = Vec3(0, 0, -1);
		camera.at = Vec3(0, 0, 0);
		camera.ortho.enabled	= true;
		camera.ortho.origin		= 0;
		camera.ortho.size		= ratio * -scale;
		return camera;
	}
}

#define $cam Camera::

#endif // MAKAI_VIEWPERSPECTIVE_CAMERA
