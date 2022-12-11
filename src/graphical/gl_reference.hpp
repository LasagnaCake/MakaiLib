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

#define GET_VERTEX(IDX) (*source)[offset+(IDX)]

class Plane: public Empty {
public:
	Plane(vector<RawVertex>& source, size_t offset) {
		this->source	= &source;
		this->offset	= offset;
	}

	virtual ~Plane() {
		Empty::~Empty();
	}

	#define VERTEX_SET_POS(VERT, VAL) Drawer::vertexSetPosition(VERT, VAL);
	/// Sets the plane's origin.
	Plane* setOrigin(
			Vector3 tlPos = Vector3(-1, 1),
			Vector3 trPos = Vector3(1, 1),
			Vector3 blPos = Vector3(-1, -1),
			Vector3 brPos = Vector3(1, -1)
		) {
		VERTEX_SET_POS(GET_VERTEX(0),	tlPos);
		VERTEX_SET_POS(GET_VERTEX(1),	trPos);
		VERTEX_SET_POS(GET_VERTEX(3),	trPos);
		VERTEX_SET_POS(GET_VERTEX(2),	blPos);
		VERTEX_SET_POS(GET_VERTEX(4),	blPos);
		VERTEX_SET_POS(GET_VERTEX(5),	brPos);
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
		Drawer::vertexSetUV(GET_VERTEX(0),	tlUV);
		Drawer::vertexSetUV(GET_VERTEX(1),	trUV);
		Drawer::vertexSetUV(GET_VERTEX(3),	trUV);
		Drawer::vertexSetUV(GET_VERTEX(2),	blUV);
		Drawer::vertexSetUV(GET_VERTEX(4),	blUV);
		Drawer::vertexSetUV(GET_VERTEX(5),	brUV);
		return this;
	}

	Plane* setColor(
			Vector4 tlCol,
			Vector4 trCol,
			Vector4 blCol,
			Vector4 brCol
		) {
		Drawer::vertexSetColor(GET_VERTEX(0),	tlCol);
		Drawer::vertexSetColor(GET_VERTEX(1),	trCol);
		Drawer::vertexSetColor(GET_VERTEX(3),	trCol);
		Drawer::vertexSetColor(GET_VERTEX(2),	blCol);
		Drawer::vertexSetColor(GET_VERTEX(4),	blCol);
		Drawer::vertexSetColor(GET_VERTEX(5),	brCol);
		return this;
	}

	Plane* setColor(
			Vector4 col = Color::WHITE
		) {
		Drawer::vertexSetColor(GET_VERTEX(0),	col);
		Drawer::vertexSetColor(GET_VERTEX(1),	col);
		Drawer::vertexSetColor(GET_VERTEX(3),	col);
		Drawer::vertexSetColor(GET_VERTEX(2),	col);
		Drawer::vertexSetColor(GET_VERTEX(4),	col);
		Drawer::vertexSetColor(GET_VERTEX(5),	col);
		return this;
	}

	/// Sets the plane to its original state (last state set with setPosition).
	Plane* reset() override {
		VERTEX_SET_POS(GET_VERTEX(0),	origin[0]);
		VERTEX_SET_POS(GET_VERTEX(1),	origin[1]);
		VERTEX_SET_POS(GET_VERTEX(3),	origin[1]);
		VERTEX_SET_POS(GET_VERTEX(2),	origin[2]);
		VERTEX_SET_POS(GET_VERTEX(4),	origin[2]);
		VERTEX_SET_POS(GET_VERTEX(5),	origin[3]);
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
		$srpTransform(GET_VERTEX(0),	trans);
		$srpTransform(GET_VERTEX(1),	trans);
		$srpTransform(GET_VERTEX(2),	trans);
		$srpTransform(GET_VERTEX(3),	trans);
		$srpTransform(GET_VERTEX(4),	trans);
		$srpTransform(GET_VERTEX(5),	trans);
		//$debug(GET_VERTEX(0).x);
		return this;
	}

	RawVertex** getBoundVertices() override {
		for $ssrange(i, 0, 6) verts[i] = &GET_VERTEX(i);
		return verts;
	}

	void forEachVertex(VertexFunc f) override {
		for $ssrange(i, 0, 6) f(GET_VERTEX(i));
	}

protected:
	vector<RawVertex>*	source;
	size_t				offset;

	RawVertex*	verts[6]	= {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

	Vector3	origin[4];

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
	Triangle(vector<RawVertex>& source, size_t offset) {
		this->source = &source;
		this->offset = offset;
	}

	virtual ~Triangle() {
		Empty::~Empty();
	}

	#define VERTEX_SET_POS(VERT, VAL) Drawer::vertexSetPosition(VERT, VAL);
	/// Sets the triangle's origin.
	Triangle* setOrigin(
			Vector3 aPos = Vector3(+0, 1),
			Vector3 bPos = Vector3(-1, -1),
			Vector3 cPos = Vector3(+1, -1)
		) {
		VERTEX_SET_POS(GET_VERTEX(0),	aPos);
		VERTEX_SET_POS(GET_VERTEX(1),	bPos);
		VERTEX_SET_POS(GET_VERTEX(2),	cPos);
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
		Drawer::vertexSetUV(GET_VERTEX(0),	aUV);
		Drawer::vertexSetUV(GET_VERTEX(1),	bUV);
		Drawer::vertexSetUV(GET_VERTEX(2),	cUV);
		return this;
	}

	Triangle* setColor(
			Vector4 aCol,
			Vector4 bCol,
			Vector4 cCol
		) {
		Drawer::vertexSetColor(GET_VERTEX(0),	aCol);
		Drawer::vertexSetColor(GET_VERTEX(1),	bCol);
		Drawer::vertexSetColor(GET_VERTEX(2),	cCol);
		return this;
	}

	Triangle* setColor(
			Vector4 col = Color::WHITE
		) {
		Drawer::vertexSetColor(GET_VERTEX(0),	col);
		Drawer::vertexSetColor(GET_VERTEX(1),	col);
		Drawer::vertexSetColor(GET_VERTEX(2),	col);
		return this;
	}

	/// Sets the triangle to its original state (last state set with setPosition).
	Triangle* reset() override {
		VERTEX_SET_POS(GET_VERTEX(0),	origin[0]);
		VERTEX_SET_POS(GET_VERTEX(1),	origin[1]);
		VERTEX_SET_POS(GET_VERTEX(2),	origin[2]);
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
		$srpTransform(GET_VERTEX(0),	trans);
		$srpTransform(GET_VERTEX(1),	trans);
		$srpTransform(GET_VERTEX(2),	trans);
		return this;
	}

	RawVertex** getBoundVertices() override {
		for $ssrange(i, 0, 3) verts[i] = &GET_VERTEX(i);
		return verts;
	}

	void forEachVertex(VertexFunc f) override {
		for $ssrange(i, 0, 3) f(GET_VERTEX(i));
	}

protected:
	vector<RawVertex>*	source;
	size_t				offset;

	RawVertex*	verts[3]	= {nullptr, nullptr, nullptr};

	Vector3	origin[3];

};

#undef GET_VERTEX

#undef VERTEX_SET_POS
