#include "camera.hpp"

using namespace Makai::Graph;

constexpr Matrix4x4 Makai::Graph::asMatrix(OrthographicData const& data, float const& zNear, float const& zFar) {
	return MatMath::ortho(
		data.origin.x,
		data.size.x,
		data.size.y,
		data.origin.y,
		zNear,
		zFar
	);
}

constexpr Matrix4x4 Camera3D::matrix() const {
	Vector3 camAt = at;
	if (relativeToEye)
		camAt += eye;
	return MatMath::lookAt(
		eye,
		camAt,
		up
	);
}

constexpr Matrix4x4 Camera3D::projection() const {
	if (ortho.strength == 0.0)
		return MatMath::perspective(
			fov,
			aspect.tangent(),
			zNear,
			zFar
		);
	if (ortho.strength == 1.0)
		return MatMath::ortho(
			ortho.origin.x,
			ortho.size.x,
			ortho.size.y,
			ortho.origin.y,
			zNear,
			zFar
		);
	Matrix4x4 om = MatMath::ortho(
		ortho.origin.x,
		ortho.size.x,
		ortho.size.y,
		ortho.origin.y,
		zNear,
		zFar
	);
	Matrix4x4 pm = MatMath::perspective(
		fov,
		aspect.tangent(),
		zNear,
		zFar
	);
	return MatMath::lerp<Matrix4x4>(pm, om, ortho.strength);
}

constexpr static Camera3D Camera3D::from2D(Vector2 const& size) {
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

constexpr static Camera3D Camera3D::from2D(float const& scale, Vector2 const& ratio) {
	return Camera3D::from2D(scale * ratio);
}

constexpr Camera3D GimbalCamera3D::toCamera3D() const {
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

constexpr GimbalCamera3D::operator Camera3D() const {
	return toCamera3D();
}

constexpr GimbalCamera3D& GimbalCamera3D::fromCamera3D(Camera3D const& cam) {
	auto crot	= MatMath::getEulerAnglesYXZ(cam.matrix());
	position	= cam.eye;
	rotation	= Vector3(crot.x, crot.y, crot.z);
	aspect		= cam.aspect;
	fov			= cam.fov;
	zNear		= cam.zNear;
	zFar		= cam.zFar;
	ortho		= cam.ortho;
	return (*this);
}

constexpr GimbalCamera3D& GimbalCamera3D::operator=(Camera3D const& cam) {
	fromCamera3D(cam);
	return (*this);
}
