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
			Vector3 tlPos = Vector3(-1, 1),
			Vector3 trPos = Vector3(1, 1),
			Vector3 blPos = Vector3(-1, -1),
			Vector3 brPos = Vector3(1, -1)
		) {
		VERTEX_SET_POS(*tl,		tlPos);
		VERTEX_SET_POS(*tr1,	trPos);
		VERTEX_SET_POS(*tr2,	trPos);
		VERTEX_SET_POS(*bl1,	blPos);
		VERTEX_SET_POS(*bl2,	blPos);
		VERTEX_SET_POS(*br,		brPos);
		posOrigin[0] = tlPos;
		posOrigin[1] = trPos;
		posOrigin[2] = blPos;
		posOrigin[3] = brPos;
		return this;
	}

	/// Transforms the plane's origin and normals by a given transform.
	Plane* setOrigin(Transform3D trans) {
		glm::mat4 glmtrans = asGLMMatrix(trans);
		posOrigin[0]	= srpTransform(posOrigin[0],	glmtrans);
		posOrigin[1]	= srpTransform(posOrigin[1],	glmtrans);
		posOrigin[2]	= srpTransform(posOrigin[2],	glmtrans);
		posOrigin[3]	= srpTransform(posOrigin[3],	glmtrans);
		normOrigin[0]	= srpTransform(normOrigin[0],	glmtrans);
		normOrigin[1]	= srpTransform(normOrigin[1],	glmtrans);
		normOrigin[2]	= srpTransform(normOrigin[2],	glmtrans);
		normOrigin[3]	= srpTransform(normOrigin[3],	glmtrans);
		return reset();
	}

	Plane* setUV(
			Vector2 tlUV,
			Vector2 trUV,
			Vector2 blUV,
			Vector2 brUV
		) {
		Drawer::vertexSetUV(*tl,	tlUV);
		Drawer::vertexSetUV(*tr1,	trUV);
		Drawer::vertexSetUV(*tr2,	trUV);
		Drawer::vertexSetUV(*bl1,	blUV);
		Drawer::vertexSetUV(*bl2,	blUV);
		Drawer::vertexSetUV(*br,	brUV);
		return this;
	}

	Plane* setColor(
			Vector4 tlCol,
			Vector4 trCol,
			Vector4 blCol,
			Vector4 brCol
		) {
		Drawer::vertexSetColor(*tl,		tlCol);
		Drawer::vertexSetColor(*tr1,	trCol);
		Drawer::vertexSetColor(*tr2,	trCol);
		Drawer::vertexSetColor(*bl1,	blCol);
		Drawer::vertexSetColor(*bl2,	blCol);
		Drawer::vertexSetColor(*br,		brCol);
		return this;
	}

	Plane* setColor(
			Vector4 col = Color::WHITE
		) {
		Drawer::vertexSetColor(*tl,		col);
		Drawer::vertexSetColor(*tr1,	col);
		Drawer::vertexSetColor(*tr2,	col);
		Drawer::vertexSetColor(*bl1,	col);
		Drawer::vertexSetColor(*bl2,	col);
		Drawer::vertexSetColor(*br,		col);
		return this;
	}

	Plane* setNormal(
			Vector3 tln,
			Vector3 trn,
			Vector3 bln,
			Vector3 brn
		) {
		VERTEX_SET_NORM(*tl,	tln);
		VERTEX_SET_NORM(*tr1,	trn);
		VERTEX_SET_NORM(*tr2,	trn);
		VERTEX_SET_NORM(*bl1,	bln);
		VERTEX_SET_NORM(*bl2,	bln);
		VERTEX_SET_NORM(*br,	brn);
		normOrigin[0] = tln;
		normOrigin[1] = trn;
		normOrigin[2] = bln;
		normOrigin[3] = brn;
		return this;
	}

	Plane* setNormal(
			Vector3 n
		) {
		VERTEX_SET_NORM(*tl,	n);
		VERTEX_SET_NORM(*tr1,	n);
		VERTEX_SET_NORM(*tr2,	n);
		VERTEX_SET_NORM(*bl1,	n);
		VERTEX_SET_NORM(*bl2,	n);
		VERTEX_SET_NORM(*br,	n);
		normOrigin[0] = n;
		normOrigin[1] = n;
		normOrigin[2] = n;
		normOrigin[3] = n;
		return this;
	}

	/// Sets the plane to its original state (last state set with setPosition).
	Plane* reset() override {
		// Set origin
		VERTEX_SET_POS(*tl,		posOrigin[0]);
		VERTEX_SET_POS(*tr1,	posOrigin[1]);
		VERTEX_SET_POS(*tr2,	posOrigin[1]);
		VERTEX_SET_POS(*bl1,	posOrigin[2]);
		VERTEX_SET_POS(*bl2,	posOrigin[2]);
		VERTEX_SET_POS(*br,		posOrigin[3]);
		// Set normals
		VERTEX_SET_NORM(*tl,	normOrigin[0]);
		VERTEX_SET_NORM(*tr1,	normOrigin[1]);
		VERTEX_SET_NORM(*tr2,	normOrigin[1]);
		VERTEX_SET_NORM(*bl1,	normOrigin[2]);
		VERTEX_SET_NORM(*bl2,	normOrigin[2]);
		VERTEX_SET_NORM(*br,	normOrigin[3]);
		return this;
	}

	Plane* transform() override {
		onTransform();
		if (!fixed) return this;
		// Get transformation
		Transform3D self = local;
		self.scale *= (float)visible;
		glm::mat4 trans = asGLMMatrix(self);
		// Apply transformation
		srpTransform(*tl, trans);
		srpTransform(*tr1, trans);
		srpTransform(*tr2, trans);
		srpTransform(*bl1, trans);
		srpTransform(*bl2, trans);
		srpTransform(*br, trans);
		return this;
	}

	Triangle** getBoundTriangles() override {
		return tris;
	}

	void forEachVertex(VertexFunc f) override {
		f(*tl);
		f(*tr1);
		f(*tr2);
		f(*bl1);
		f(*bl2);
		f(*br);
	}

	RawVertex* tl	= nullptr;
	RawVertex* tr1	= nullptr;
	RawVertex* tr2	= nullptr;
	RawVertex* bl1	= nullptr;
	RawVertex* bl2	= nullptr;
	RawVertex* br	= nullptr;

protected:

	Triangle* tris[2] = {nullptr, nullptr};

	Vector3 posOrigin[4];
	Vector3 normOrigin[4];
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
			Vector3 aPos = Vector3(+0, 1),
			Vector3 bPos = Vector3(-1, -1),
			Vector3 cPos = Vector3(+1, -1)
		) {
		VERTEX_SET_POS(*a,	aPos);
		VERTEX_SET_POS(*b,	bPos);
		VERTEX_SET_POS(*c,	cPos);
		posOrigin[0] = aPos;
		posOrigin[1] = bPos;
		posOrigin[2] = cPos;
		return this;
	}

	/// Transforms the triangle's origin and normals by a given transform.
	Trigon* setOrigin(Transform3D trans) {
		glm::mat4 glmtrans = asGLMMatrix(trans);
		posOrigin[0]	= srpTransform(posOrigin[0],	glmtrans);
		posOrigin[1]	= srpTransform(posOrigin[1],	glmtrans);
		posOrigin[2]	= srpTransform(posOrigin[2],	glmtrans);
		normOrigin[0]	= srpTransform(normOrigin[0],	glmtrans);
		normOrigin[1]	= srpTransform(normOrigin[1],	glmtrans);
		normOrigin[2]	= srpTransform(normOrigin[2],	glmtrans);
		return reset();
	}

	Trigon* setUV(
			Vector2 aUV,
			Vector2 bUV,
			Vector2 cUV
		) {
		Drawer::vertexSetUV(*a,	aUV);
		Drawer::vertexSetUV(*b,	bUV);
		Drawer::vertexSetUV(*c,	cUV);
		return this;
	}

	Trigon* setColor(
			Vector4 aCol,
			Vector4 bCol,
			Vector4 cCol
		) {
		Drawer::vertexSetColor(*a,	aCol);
		Drawer::vertexSetColor(*b,	bCol);
		Drawer::vertexSetColor(*c,	cCol);
		return this;
	}

	Trigon* setColor(
			Vector4 col = Color::WHITE
		) {
		Drawer::vertexSetColor(*a,	col);
		Drawer::vertexSetColor(*b,	col);
		Drawer::vertexSetColor(*c,	col);
		return this;
	}

	Trigon* setNormal(
			Vector3 an,
			Vector3 bn,
			Vector3 cn
		) {
		VERTEX_SET_NORM(*a,	an);
		VERTEX_SET_NORM(*b,	bn);
		VERTEX_SET_NORM(*c,	cn);
		return this;
	}

	Trigon* setNormal(
			Vector3 n
		) {
		VERTEX_SET_NORM(*a,	n);
		VERTEX_SET_NORM(*b,	n);
		VERTEX_SET_NORM(*c,	n);
		return this;
	}

	/// Sets the triangle to its original state (last state set with setPosition).
	Trigon* reset() override {
		VERTEX_SET_POS(*a,	posOrigin[0]);
		VERTEX_SET_POS(*b,	posOrigin[1]);
		VERTEX_SET_POS(*c,	posOrigin[2]);
		return this;
	}

	Trigon* transform() override {
		onTransform();
		if (!fixed) return this;
		// Get transformation
		Transform3D self = local;
		self.scale *= (float)visible;
		glm::mat4 trans = asGLMMatrix(self);
		// Apply transformation
		srpTransform(*a,	trans);
		srpTransform(*b,	trans);
		srpTransform(*c,	trans);
		return this;
	}

	Triangle** getBoundTriangles() override {
		return tris;
	}

	void forEachVertex(VertexFunc f) override {
		f(*a);
		f(*b);
		f(*c);
	}

	RawVertex* a	= nullptr;
	RawVertex* b	= nullptr;
	RawVertex* c	= nullptr;

protected:

	Triangle* tris[1] = {nullptr};

	Vector3 posOrigin[3];
	Vector3 normOrigin[3];
};

#undef VERTEX_SET_POS
#undef VERTEX_SET_NORM
