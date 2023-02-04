typedef std::function<void(RawVertex&)> VertexFunc;

#define VERTEX_SET_POS(VERT, VAL)	Drawer::vertexSetPosition(VERT, VAL);
#define VERTEX_SET_NORM(VERT, VAL)	Drawer::vertexSetNormal(VERT, VAL);

struct Empty {
	virtual void onTransform() {}
	virtual Empty* reset() {return this;}
	virtual Empty* transform() {return this;}
	virtual Triangle** getBoundTriangles() {return nullptr;}
	virtual void forEachVertex(VertexFunc f) {};
	void destroy()	{onDestroy();}
	void unbind()	{onUnbind();}
	bool fixed = true;
	bool visible = true;
	Transform3D local;
	virtual ~Empty() {onDestroy = onUnbind = [](){};};
	friend class RenderData::Renderable;
private:
	$evt Signal onDestroy;
	$evt Signal onUnbind;
};

// [[ PLANES ]]

class Plane: public Empty {
public:
	Plane(
		Triangle* tris[2]
	) {
		this->tris[0]	= tris[0];
		this->tris[1]	= tris[1];
		this->tl	= &(tris[0]->verts[0]);
		this->tr1	= &(tris[0]->verts[1]);
		this->tr2	= &(tris[1]->verts[0]);
		this->bl1	= &(tris[0]->verts[2]);
		this->bl2	= &(tris[1]->verts[1]);
		this->br	= &(tris[1]->verts[2]);
	}

	Plane(
		RawVertex* tl,
		RawVertex* tr1,
		RawVertex* tr2,
		RawVertex* bl1,
		RawVertex* bl2,
		RawVertex* br
	) {
		this->tl	= tl;
		this->tr1	= tr1;
		this->tr2	= tr2;
		this->bl1	= bl1;
		this->bl2	= bl2;
		this->br	= br;
	}

	virtual ~Plane() {
		Empty::~Empty();
		tl = tr1 = tr2 = bl1 = bl2 = br = nullptr;
		tris[0] = tris[1] = nullptr;
	}

	/// Sets the plane's posOrigin.
	Plane* setOrigin(
			Vector3 tlPos = Vector3(-1, +1),
			Vector3 trPos = Vector3(+1, +1),
			Vector3 blPos = Vector3(-1, -1),
			Vector3 brPos = Vector3(+1, -1)
		) {
		VERTEX_SET_POS(origin[0],	tlPos);
		VERTEX_SET_POS(origin[1],	trPos);
		VERTEX_SET_POS(origin[2],	blPos);
		VERTEX_SET_POS(origin[3],	brPos);
		return this;
	}

	/// Transforms the plane's origin and normals by a given transform.
	Plane* setOrigin(Transform3D trans) {
		glm::mat4 glmtrans = asGLMMatrix(trans);
		srpTransform(origin[0],	glmtrans);
		srpTransform(origin[1],	glmtrans);
		srpTransform(origin[2],	glmtrans);
		srpTransform(origin[3],	glmtrans);
		return this;
	}

	Plane* setUV(
			Vector2 tlUV,
			Vector2 trUV,
			Vector2 blUV,
			Vector2 brUV
		) {
		Drawer::vertexSetUV(origin[0],	tlUV);
		Drawer::vertexSetUV(origin[1],	trUV);
		Drawer::vertexSetUV(origin[2],	blUV);
		Drawer::vertexSetUV(origin[3],	brUV);
		return this;
	}

	Plane* setColor(
			Vector4 tlCol,
			Vector4 trCol,
			Vector4 blCol,
			Vector4 brCol
		) {
		Drawer::vertexSetColor(origin[0],	tlCol);
		Drawer::vertexSetColor(origin[1],	trCol);
		Drawer::vertexSetColor(origin[2],	blCol);
		Drawer::vertexSetColor(origin[3],	brCol);
		return this;
	}

	Plane* setColor(
			Vector4 col = Color::WHITE
		) {
		Drawer::vertexSetColor(origin[0],	col);
		Drawer::vertexSetColor(origin[1],	col);
		Drawer::vertexSetColor(origin[2],	col);
		Drawer::vertexSetColor(origin[3],	col);
		return this;
	}

	Plane* setNormal(
			Vector3 tln,
			Vector3 trn,
			Vector3 bln,
			Vector3 brn
		) {
		VERTEX_SET_NORM(origin[0],	tln);
		VERTEX_SET_NORM(origin[1],	trn);
		VERTEX_SET_NORM(origin[2],	bln);
		VERTEX_SET_NORM(origin[3],	brn);
		return this;
	}

	Plane* setNormal(
			Vector3 n
		) {
		VERTEX_SET_NORM(origin[0],	n);
		VERTEX_SET_NORM(origin[1],	n);
		VERTEX_SET_NORM(origin[2],	n);
		VERTEX_SET_NORM(origin[3],	n);
		return this;
	}

	/// Sets the plane to its original state (last state set with setPosition).
	Plane* reset() override {
		// Set origin
		*tl				= origin[0];
		*tr1	= *tr2	= origin[1];
		*bl1	= *bl2	= origin[2];
		*br				= origin[3];
		return this;
	}

	Plane* transform() override {
		onTransform();
		if (!fixed) return this;
		// Get transformation
		Transform3D self = local;
		self.scale *= (float)visible;
		glm::mat4 trans = asGLMMatrix(self);
		// Calculate transformed vertices
		RawVertex plane[4] = {origin[0], origin[1], origin[2], origin[3]};
		srpTransform(plane[0], trans);
		srpTransform(plane[1], trans);
		srpTransform(plane[2], trans);
		srpTransform(plane[3], trans);
		// Apply transformation
		*tl		= plane[0];
		*tr1	= *tr2	= plane[1];
		*bl1	= *bl2	= plane[2];
		*br		= plane[3];
		return this;
	}

	Triangle** getBoundTriangles() override {
		return tris;
	}

	void forEachVertex(VertexFunc f) override {
		f(origin[0]);
		f(origin[1]);
		f(origin[2]);
		f(origin[3]);
	}

	RawVertex* tl	= nullptr;
	RawVertex* tr1	= nullptr;
	RawVertex* tr2	= nullptr;
	RawVertex* bl1	= nullptr;
	RawVertex* bl2	= nullptr;
	RawVertex* br	= nullptr;

protected:
	Triangle* tris[2] = {nullptr, nullptr};
	RawVertex	origin[4];
};

class AnimatedPlane: public Plane {
public:
	using Plane::Plane;
	Vector2 frame;
	Vector2 size = Vector2(1);

	void onTransform() override {
		if (size.x == 0 || size.y == 0)
			throw runtime_error("Sprite count (size) must be bigger than zero!");
		setUV(
			(frame) / size,
			(frame + Vector2(1, 0)) / size,
			(frame + Vector2(0, 1)) / size,
			(frame + Vector2(1)) / size
		);
	}
};

// [[ TRIANGLES ]]
class Trigon: public Empty {
public:
	Trigon(
		Triangle* tris[1]
	) {
		this->tris[0]	= tris[0];
		this->a	= &(tris[0]->verts[0]);
		this->b	= &(tris[0]->verts[1]);
		this->c	= &(tris[0]->verts[2]);
	}

	Trigon(
		RawVertex* a,
		RawVertex* b,
		RawVertex* c
	) {
		this->a	= a;
		this->b	= b;
		this->c	= c;
	}

	virtual ~Trigon() {
		Empty::~Empty();
		a = b = c = nullptr;
		tris[0] = nullptr;
	}

	/// Sets the triangle's origin.
	Trigon* setOrigin(
			Vector3 aPos = Vector3(+0, +1),
			Vector3 bPos = Vector3(-1, -1),
			Vector3 cPos = Vector3(+1, -1)
		) {
		VERTEX_SET_POS(origin[0],	aPos);
		VERTEX_SET_POS(origin[1],	bPos);
		VERTEX_SET_POS(origin[2],	cPos);
		return this;
	}

	/// Transforms the triangle's origin and normals by a given transform.
	Trigon* setOrigin(Transform3D trans) {
		glm::mat4 glmtrans = asGLMMatrix(trans);
		srpTransform(origin[0],	glmtrans);
		srpTransform(origin[1],	glmtrans);
		srpTransform(origin[2],	glmtrans);
		return this;
	}

	Trigon* setUV(
			Vector2 aUV,
			Vector2 bUV,
			Vector2 cUV
		) {
		Drawer::vertexSetUV(origin[0],	aUV);
		Drawer::vertexSetUV(origin[1],	bUV);
		Drawer::vertexSetUV(origin[2],	cUV);
		return this;
	}

	Trigon* setColor(
			Vector4 aCol,
			Vector4 bCol,
			Vector4 cCol
		) {
		Drawer::vertexSetColor(origin[0],	aCol);
		Drawer::vertexSetColor(origin[1],	bCol);
		Drawer::vertexSetColor(origin[2],	cCol);
		return this;
	}

	Trigon* setColor(
			Vector4 col = Color::WHITE
		) {
		Drawer::vertexSetColor(origin[0],	col);
		Drawer::vertexSetColor(origin[1],	col);
		Drawer::vertexSetColor(origin[2],	col);
		return this;
	}

	Trigon* setNormal(
			Vector3 an,
			Vector3 bn,
			Vector3 cn
		) {
		VERTEX_SET_NORM(origin[0],	an);
		VERTEX_SET_NORM(origin[1],	bn);
		VERTEX_SET_NORM(origin[2],	cn);
		return this;
	}

	Trigon* setNormal(
			Vector3 n
		) {
		VERTEX_SET_NORM(origin[0],	n);
		VERTEX_SET_NORM(origin[1],	n);
		VERTEX_SET_NORM(origin[2],	n);
		return this;
	}

	/// Sets the triangle to its original state (last state set with setPosition).
	Trigon* reset() override {
		*a = origin[0];
		*b = origin[1];
		*c = origin[2];
		return this;
	}

	Trigon* transform() override {
		onTransform();
		if (!fixed) return this;
		// Get transformation
		Transform3D self = local;
		self.scale *= (float)visible;
		glm::mat4 trans = asGLMMatrix(self);
		// Calculate transformed vertices
		RawVertex tri[3] = {origin[0], origin[1], origin[2]};
		srpTransform(tri[0], trans);
		srpTransform(tri[1], trans);
		srpTransform(tri[2], trans);
		// Apply transformation
		*a	= tri[0];
		*b	= tri[1];
		*c	= tri[2];
		return this;
	}

	Triangle** getBoundTriangles() override {
		return tris;
	}

	void forEachVertex(VertexFunc f) override {
		f(origin[0]);
		f(origin[1]);
		f(origin[2]);
	}

	RawVertex* a	= nullptr;
	RawVertex* b	= nullptr;
	RawVertex* c	= nullptr;

protected:
	Triangle* tris[1] = {nullptr};
	RawVertex origin[3];
};

#undef VERTEX_SET_POS
#undef VERTEX_SET_NORM
