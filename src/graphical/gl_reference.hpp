typedef Function<void(RawVertex&)> VertexFunc;

struct Empty {
	void destroy()	{onDestroy();}
	void unbind()	{onUnbind();}
	virtual ~Empty() {onDestroy = onUnbind = []{};};
	friend class RenderData::Renderable;
private:
	Event::Signal onDestroy;
	Event::Signal onUnbind;
};

template<size_t COUNT>
struct Shape: public Empty {
	constexpr static size_t triangleCount = COUNT;

	Shape(Triangle* const(& tris)[triangleCount]) {
		for SRANGE(i, 0, triangleCount)
			this->tris[i] = tris[i];
	}

	virtual ~Shape() {
		DEBUGLN("Deleting bound triangles (", triangleCount, ")...");
		delete[] tris;
	};

	virtual Shape* reset()		= 0;
	virtual Shape* transform()	= 0;

	typedef Span<Triangle*, triangleCount> Triangles;

	Triangles getBoundTriangles() {
		return Triangles(tris, triangleCount);
	}

	virtual void forEachVertex(VertexFunc const& f) {
		for SRANGE(i, 0, triangleCount)
			for SRANGE(j, 0, 3)
				f(((RawVertex*)tris[i])[j]);
	};

	bool fixed		= true;
	bool visible	= true;

	Transform3D local;

	friend class RenderData::Renderable;
protected:
	Triangle** tris = new Triangle*[triangleCount](nullptr);
};

// [[ PLANES ]]

class Plane: public Shape<2> {
public:
	Plane(
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
	Plane* setOrigin(
			Vector3 const& tlPos = Vector3(-1, +1),
			Vector3 const& trPos = Vector3(+1, +1),
			Vector3 const& blPos = Vector3(-1, -1),
			Vector3 const& brPos = Vector3(+1, -1)
		) {
		Drawer::vertexSetPosition(origin[0],	tlPos);
		Drawer::vertexSetPosition(origin[1],	trPos);
		Drawer::vertexSetPosition(origin[2],	blPos);
		Drawer::vertexSetPosition(origin[3],	brPos);
		return this;
	}

	/// Transforms the plane's origin and normals by a given transform.
	Plane* setOrigin(Transform3D const& trans) {
		Matrix4x4 tmat(trans);
		srpTransform(origin[0],	tmat);
		srpTransform(origin[1],	tmat);
		srpTransform(origin[2],	tmat);
		srpTransform(origin[3],	tmat);
		return this;
	}

	Plane* setUV(
			Vector2 const& tlUV,
			Vector2 const& trUV,
			Vector2 const& blUV,
			Vector2 const& brUV
		) {
		Drawer::vertexSetUV(origin[0],	tlUV);
		Drawer::vertexSetUV(origin[1],	trUV);
		Drawer::vertexSetUV(origin[2],	blUV);
		Drawer::vertexSetUV(origin[3],	brUV);
		return this;
	}

	Plane* setColor(
			Vector4 const& tlCol,
			Vector4 const& trCol,
			Vector4 const& blCol,
			Vector4 const& brCol
		) {
		Drawer::vertexSetColor(origin[0],	tlCol);
		Drawer::vertexSetColor(origin[1],	trCol);
		Drawer::vertexSetColor(origin[2],	blCol);
		Drawer::vertexSetColor(origin[3],	brCol);
		return this;
	}

	Plane* setColor(
			Vector4 const& col = Color::WHITE
		) {
		Drawer::vertexSetColor(origin[0],	col);
		Drawer::vertexSetColor(origin[1],	col);
		Drawer::vertexSetColor(origin[2],	col);
		Drawer::vertexSetColor(origin[3],	col);
		return this;
	}

	Plane* setNormal(
			Vector3 const& tln,
			Vector3 const& trn,
			Vector3 const& bln,
			Vector3 const& brn
		) {
		Drawer::vertexSetNormal(origin[0],	tln);
		Drawer::vertexSetNormal(origin[1],	trn);
		Drawer::vertexSetNormal(origin[2],	bln);
		Drawer::vertexSetNormal(origin[3],	brn);
		return this;
	}

	Plane* setNormal(
			Vector3 const& n
		) {
		Drawer::vertexSetNormal(origin[0],	n);
		Drawer::vertexSetNormal(origin[1],	n);
		Drawer::vertexSetNormal(origin[2],	n);
		Drawer::vertexSetNormal(origin[3],	n);
		return this;
	}

	/// Sets the plane to its original state (last state set with setPosition).
	Plane* reset() override final {
		// Set origin
		*tl				= origin[0];
		*tr1	= *tr2	= origin[1];
		*bl1	= *bl2	= origin[2];
		*br				= origin[3];
		return this;
	}

	Plane* transform() override final {
		onTransform();
		if (!fixed) return this;
		// Get transformation
		Transform3D self = local;
		self.scale *= (float)visible;
		Matrix4x4 tmat(self);
		// Calculate transformed vertices
		RawVertex plane[4] = {origin[0], origin[1], origin[2], origin[3]};
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

	void forEachVertex(VertexFunc const& f) override final {
		f(origin[0]);
		f(origin[1]);
		f(origin[2]);
		f(origin[3]);
	}

	RawVertex	origin[4];

protected:
	virtual void onTransform() {};

private:
	RawVertex
		*tl		= nullptr,
		*tr1	= nullptr,
		*tr2	= nullptr,
		*bl1	= nullptr,
		*bl2	= nullptr,
		*br		= nullptr;
};

class AnimatedPlane: public Plane {
public:
	using Plane::Plane;
	Vector2 frame;
	Vector2 size = Vector2(1);

	void onTransform() override {
		if (size.x == 0 || size.y == 0)
			throw Error::InvalidValue("Sprite count (size) must be bigger than zero!");
		setUV(
			(frame) / size,
			(frame + Vector2(1, 0)) / size,
			(frame + Vector2(0, 1)) / size,
			(frame + Vector2(1)) / size
		);
	}
};

// [[ TRIANGLES ]]
class Trigon: public Shape<1> {
public:
	Trigon(
		Triangle* const(& tris)[1]
	): Shape<1>(tris) {
		// Get vertices
		this->a = &(tris[0]->verts[0]);
		this->b = &(tris[0]->verts[1]);
		this->c = &(tris[0]->verts[2]);
		// Setup trigon
		this->setOrigin(
			Vector3(-0.0, +1.0, 0.0),
			Vector3(-1.0, -1.0, 0.0),
			Vector3(+1.0, -1.0, 0.0)
		);
		this->setUV(
			Vector2(+0.5, +1.0),
			Vector2(+0.0, +0.0),
			Vector2(+1.0, +0.0)
		);
		this->setColor();
		this->setNormal(
			Vector3(+0.0, +0.0, -1.0)
		);
	}

	virtual ~Trigon() {}

	/// Sets the triangle's origin.
	Trigon* setOrigin(
			Vector3 const& aPos = Vector3(+0, +1),
			Vector3 const& bPos = Vector3(-1, -1),
			Vector3 const& cPos = Vector3(+1, -1)
		) {
		Drawer::vertexSetPosition(origin[0],	aPos);
		Drawer::vertexSetPosition(origin[1],	bPos);
		Drawer::vertexSetPosition(origin[2],	cPos);
		return this;
	}

	/// Transforms the triangle's origin and normals by a given transform.
	Trigon* setOrigin(Transform3D const& trans) {
		Matrix4x4 tmat(trans);
		srpTransform(origin[0],	tmat);
		srpTransform(origin[1],	tmat);
		srpTransform(origin[2],	tmat);
		return this;
	}

	Trigon* setUV(
			Vector2 const& aUV,
			Vector2 const& bUV,
			Vector2 const& cUV
		) {
		Drawer::vertexSetUV(origin[0],	aUV);
		Drawer::vertexSetUV(origin[1],	bUV);
		Drawer::vertexSetUV(origin[2],	cUV);
		return this;
	}

	Trigon* setColor(
			Vector4 const& aCol,
			Vector4 const& bCol,
			Vector4 const& cCol
		) {
		Drawer::vertexSetColor(origin[0],	aCol);
		Drawer::vertexSetColor(origin[1],	bCol);
		Drawer::vertexSetColor(origin[2],	cCol);
		return this;
	}

	Trigon* setColor(
			Vector4 const& col = Color::WHITE
		) {
		Drawer::vertexSetColor(origin[0],	col);
		Drawer::vertexSetColor(origin[1],	col);
		Drawer::vertexSetColor(origin[2],	col);
		return this;
	}

	Trigon* setNormal(
			Vector3 const& an,
			Vector3 const& bn,
			Vector3 const& cn
		) {
		Drawer::vertexSetNormal(origin[0],	an);
		Drawer::vertexSetNormal(origin[1],	bn);
		Drawer::vertexSetNormal(origin[2],	cn);
		return this;
	}

	Trigon* setNormal(
			Vector3 const& n
		) {
		Drawer::vertexSetNormal(origin[0],	n);
		Drawer::vertexSetNormal(origin[1],	n);
		Drawer::vertexSetNormal(origin[2],	n);
		return this;
	}

	/// Sets the triangle to its original state (last state set with setPosition).
	Trigon* reset() override final {
		*a = origin[0];
		*b = origin[1];
		*c = origin[2];
		return this;
	}

	Trigon* transform() override final {
		onTransform();
		if (!fixed) return this;
		// Get transformation
		Transform3D self = local;
		self.scale *= (float)visible;
		Matrix4x4 tmat(self);
		// Calculate transformed vertices
		RawVertex tri[3] = {origin[0], origin[1], origin[2]};
		srpTransform(tri[0], tmat);
		srpTransform(tri[1], tmat);
		srpTransform(tri[2], tmat);
		// Apply transformation
		*a	= tri[0];
		*b	= tri[1];
		*c	= tri[2];
		return this;
	}

	void forEachVertex(VertexFunc const& f) override final {
		f(origin[0]);
		f(origin[1]);
		f(origin[2]);
	}

	RawVertex origin[3];

protected:
	virtual void onTransform() {};

private:
	RawVertex
		*a	= nullptr,
		*b	= nullptr,
		*c	= nullptr;
};

template<class T>
concept NotEmpty	= Type::Different<T, Empty>;

template<class T>
concept ShapeType	= requires {
	T::triangleCount;
} && Type::Derived<T, Shape<T::triangleCount>> && NotEmpty<T>;

template<class T>
concept PlaneType	= Type::Derived<T, Plane> && NotEmpty<T>;

template<class T>
concept TrigonType	= Type::Derived<T, Trigon> && NotEmpty<T>;
