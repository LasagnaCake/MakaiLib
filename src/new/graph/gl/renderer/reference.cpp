#include "reference.hpp"

using namespace Makai::Graph

void srpTransform(Vertex& vtx, Matrix4x4 const& tmat) {
	// Position
	Vector3 res = (tmat * Vector4(vtx.x, vtx.y, vtx.z, 1.0f)).toVector3();
	Vertex::setPosition(vtx, res);
	// Normal
	res = (tmat * Vector4(vtx.nx, vtx.ny, vtx.nz, 1.0f)).toVector3();
	Vertex::setNormal(vtx, res);
}

inline void srpTransform(Vertex& vtx, Transform3D const& trans) {
	srpTransform(vtx, Matrix4x4(trans));
}

template<usize N>
Shape<N>::Shape(Triangle* const(& tris)[N]) {
	for SRANGE(i, 0, N)
		this->tris[i] = tris[i];
}

template<usize N>
virtual Shape<N>::~Shape() {
	DEBUGLN("Deleting bound triangles (", N, ")...");
	delete[] tris;
};

template<usize N>
typename Shape<N>::Triangles Shape<N>::getBoundTriangles() {
	return typename Shape<N>::Triangles(tris, N);
}

template<usize N>
virtual void Shape<N>::forEachVertex(VertexFunction const& f) {
	for SRANGE(i, 0, N)
		for SRANGE(j, 0, 3)
			f(((Vertex*)tris[i])[j]);
};

PlaneRef::PlaneRef(
	Triangle* const(& tris)[2]
): Shape<2>(tris) {
	// Get vertices
	this->tl	= &(tris[0]->verts[0]);
	this->tr1	= &(tris[0]->verts[1]);
	this->tr2	= &(tris[1]->verts[0]);
	this->bl1	= &(tris[0]->verts[2]);
	this->bl2	= &(tris[1]->verts[1]);
	this->br	= &(tris[1]->verts[2]);
	// Setup plane
	this->setOrigin(
		Vector3(-1.0, +1.0, 0.0),
		Vector3(+1.0, +1.0, 0.0),
		Vector3(-1.0, -1.0, 0.0),
		Vector3(+1.0, -1.0, 0.0)
	);
	this->setUV(
		Vector2(+0.0, +1.0),
		Vector2(+1.0, +1.0),
		Vector2(+0.0, +0.0),
		Vector2(+1.0, +0.0)
	);
	this->setColor();
	this->setNormal(
		Vector3(+0.0, +0.0, -1.0)
	);
}

virtual ~Plane() {}

/// Sets the plane's posOrigin.
PlaneRef* PlaneRef::setOrigin(
		Vector3 const& tlPos = Vector3(-1, +1),
		Vector3 const& trPos = Vector3(+1, +1),
		Vector3 const& blPos = Vector3(-1, -1),
		Vector3 const& brPos = Vector3(+1, -1)
	) {
	Vertex::setPosition(origin[0],	tlPos);
	Vertex::setPosition(origin[1],	trPos);
	Vertex::setPosition(origin[2],	blPos);
	Vertex::setPosition(origin[3],	brPos);
	return this;
}

/// Transforms the plane's origin and normals by a given transform.
PlaneRef* PlaneRef::setOrigin(Transform3D const& trans) {
	Matrix4x4 tmat(trans);
	srpTransform(origin[0],	tmat);
	srpTransform(origin[1],	tmat);
	srpTransform(origin[2],	tmat);
	srpTransform(origin[3],	tmat);
	return this;
}

PlaneRef* PlaneRef::setUV(
		Vector2 const& tlUV,
		Vector2 const& trUV,
		Vector2 const& blUV,
		Vector2 const& brUV
	) {
	Vertex::setUV(origin[0],	tlUV);
	Vertex::setUV(origin[1],	trUV);
	Vertex::setUV(origin[2],	blUV);
	Vertex::setUV(origin[3],	brUV);
	return this;
}

PlaneRef* PlaneRef::setColor(
		Vector4 const& tlCol,
		Vector4 const& trCol,
		Vector4 const& blCol,
		Vector4 const& brCol
	) {
	Vertex::setColor(origin[0],	tlCol);
	Vertex::setColor(origin[1],	trCol);
	Vertex::setColor(origin[2],	blCol);
	Vertex::setColor(origin[3],	brCol);
	return this;
}

PlaneRef* PlaneRef::setColor(
		Vector4 const& col = Color::WHITE
	) {
	setColor(col, col, col, col);
	return this;
}

PlaneRef* PlaneRef::setNormal(
		Vector3 const& tln,
		Vector3 const& trn,
		Vector3 const& bln,
		Vector3 const& brn
	) {
	Vertex::setNormal(origin[0],	tln);
	Vertex::setNormal(origin[1],	trn);
	Vertex::setNormal(origin[2],	bln);
	Vertex::setNormal(origin[3],	brn);
	return this;
}

PlaneRef* PlaneRef::setNormal(
		Vector3 const& n
	) {
	setNormal(n, n, n, n);
	return this;
}

/// Sets the plane to its original state (last state set with setPosition).
PlaneRef* PlaneRef::reset() override final {
	// Set origin
	*tl				= origin[0];
	*tr1	= *tr2	= origin[1];
	*bl1	= *bl2	= origin[2];
	*br				= origin[3];
	return this;
}

PlaneRef* PlaneRef::transform() override final {
	onTransform();
	if (!fixed) return this;
	// Get transformation
	Transform3D self = local;
	self.scale *= (float)visible;
	Matrix4x4 tmat(self);
	// Calculate transformed vertices
	Vertex plane[4] = {origin[0], origin[1], origin[2], origin[3]};
	srpTransform(plane[0], tmat);
	srpTransform(plane[1], tmat);
	srpTransform(plane[2], tmat);
	srpTransform(plane[3], tmat);
	// Apply transformation
	*tl				= plane[0];
	*tr1	= *tr2	= plane[1];
	*bl1	= *bl2	= plane[2];
	*br				= plane[3];
	return this;
}

void PlaneRef::forEachVertex(VertexFunction const& f) override final {
	f(origin[0]);
	f(origin[1]);
	f(origin[2]);
	f(origin[3]);
}

void AnimatedPlaneRef::onTransform() override  {
	if (size.x == 0 || size.y == 0)
		setUV(0, 0, 0, 0);
	else setUV(
		(frame) / size,
		(frame + Vector2(1, 0)) / size,
		(frame + Vector2(0, 1)) / size,
		(frame + Vector2(1)) / size
	);
}
