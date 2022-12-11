typedef std::function<void(RawVertex&)> VertexFunc;

struct Empty {
	virtual void onTransform() {}
	virtual Empty* reset() {return this;}
	virtual Empty* transform() {return this;}
	virtual RawVertex** getBoundVertices() {return nullptr;}
	virtual void forEachVertex(VertexFunc f) {};
	void destroy()	{onDestroy();}
	void unbind()	{onUnbind();}
	bool fixed = true;
	bool visible = true;
	Transform3D local;
	virtual ~Empty() {onDestroy = onUnbind = [](){};};
	// Since it's not working, everything's public now.
	// friend class Renderable;
	// DO NOT TOUCH THESE.
	$evt Signal onDestroy;
	$evt Signal onUnbind;
};

// [[ PLANES ]]

class Plane: public Empty {
public:
	Plane(
		RawVertex* tl,
		RawVertex* tr1,
		RawVertex* tr2,
		RawVertex* bl1,
		RawVertex* bl2,
		RawVertex* br
	) {
		verts[0]	= this->tl	= tl;
		verts[1]	= this->tr1	= tr1;
		verts[2]	= this->tr2	= tr2;
		verts[3]	= this->bl1	= bl1;
		verts[4]	= this->bl2	= bl2;
		verts[5]	= this->br	= br;
	}

	virtual ~Plane() {
		Empty::~Empty();
		tl = tr1 = tr2 = bl1 = bl2 = br = nullptr;
		verts[0] = verts[1] = verts[2] = verts[3] = verts[4] = verts[5] = nullptr;
	}

	#define VERTEX_SET_POS(VERT, VAL) Drawer::vertexSetPosition(VERT, VAL);
	/// Sets the plane's origin.
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
		origin[0] = tlPos;
		origin[1] = trPos;
		origin[2] = blPos;
		origin[3] = brPos;
		return this;
	}

	/// Transforms the plane's origin by a given transform.
	Plane* setOrigin(Transform3D trans) {
		glm::mat4 glmtrans = asGLMMatrix(trans);
		origin[0] = $srpTransform(origin[0], glmtrans);
		origin[1] = $srpTransform(origin[1], glmtrans);
		origin[2] = $srpTransform(origin[2], glmtrans);
		origin[3] = $srpTransform(origin[3], glmtrans);
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

	/// Sets the plane to its original state (last state set with setPosition).
	Plane* reset() override {
		VERTEX_SET_POS(*tl,		origin[0]);
		VERTEX_SET_POS(*tr1,	origin[1]);
		VERTEX_SET_POS(*tr2,	origin[1]);
		VERTEX_SET_POS(*bl1,	origin[2]);
		VERTEX_SET_POS(*bl2,	origin[2]);
		VERTEX_SET_POS(*br,		origin[3]);
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
		$srpTransform(*tl, trans);
		$srpTransform(*tr1, trans);
		$srpTransform(*tr2, trans);
		$srpTransform(*bl1, trans);
		$srpTransform(*bl2, trans);
		$srpTransform(*br, trans);
		return this;
	}

	RawVertex** getBoundVertices() override {
		return verts;
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

	RawVertex* verts[6]	= {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

	Vector3 origin[4];

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

class Triangle: public Empty {
public:
	Triangle(
		RawVertex* a,
		RawVertex* b,
		RawVertex* c
	) {
		verts[0]	= this->a	= a;
		verts[1]	= this->b	= b;
		verts[2]	= this->c	= c;
	}

	virtual ~Triangle() {
		Empty::~Empty();
		a = b = c = nullptr;
		verts[0] = verts[1] = verts[2] = nullptr;
	}

	#define VERTEX_SET_POS(VERT, VAL) Drawer::vertexSetPosition(VERT, VAL);
	/// Sets the triangle's origin.
	Triangle* setOrigin(
			Vector3 aPos = Vector3(+0, 1),
			Vector3 bPos = Vector3(-1, -1),
			Vector3 cPos = Vector3(+1, -1)
		) {
		VERTEX_SET_POS(*a,	aPos);
		VERTEX_SET_POS(*b,	bPos);
		VERTEX_SET_POS(*c,	cPos);
		origin[0] = aPos;
		origin[1] = bPos;
		origin[2] = cPos;
		return this;
	}

	/// Transforms the plane's origin by a given transform.
	Triangle* setOrigin(Transform3D trans) {
		glm::mat4 glmtrans = asGLMMatrix(trans);
		origin[0] = $srpTransform(origin[0], glmtrans);
		origin[1] = $srpTransform(origin[1], glmtrans);
		origin[2] = $srpTransform(origin[2], glmtrans);
		return reset();
	}

	Triangle* setUV(
			Vector2 aUV,
			Vector2 bUV,
			Vector2 cUV
		) {
		Drawer::vertexSetUV(*a,	aUV);
		Drawer::vertexSetUV(*b,	bUV);
		Drawer::vertexSetUV(*c,	cUV);
		return this;
	}

	Triangle* setColor(
			Vector4 aCol,
			Vector4 bCol,
			Vector4 cCol
		) {
		Drawer::vertexSetColor(*a,	aCol);
		Drawer::vertexSetColor(*b,	bCol);
		Drawer::vertexSetColor(*c,	cCol);
		return this;
	}

	Triangle* setColor(
			Vector4 col = Color::WHITE
		) {
		Drawer::vertexSetColor(*a,	col);
		Drawer::vertexSetColor(*b,	col);
		Drawer::vertexSetColor(*c,	col);
		return this;
	}

	/// Sets the triangle to its original state (last state set with setPosition).
	Triangle* reset() override {
		VERTEX_SET_POS(*a,	origin[0]);
		VERTEX_SET_POS(*b,	origin[1]);
		VERTEX_SET_POS(*c,	origin[2]);
		return this;
	}

	Triangle* transform() override {
		onTransform();
		if (!fixed) return this;
		// Get transformation
		Transform3D self = local;
		self.scale *= (float)visible;
		glm::mat4 trans = asGLMMatrix(self);
		// Apply transformation
		$srpTransform(*a,	trans);
		$srpTransform(*b,	trans);
		$srpTransform(*c,	trans);
		return this;
	}

	RawVertex** getBoundVertices() override {
		return verts;
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

	RawVertex* verts[3]	= {nullptr, nullptr, nullptr};

	Vector3 origin[3];

};

#undef VERTEX_SET_POS
