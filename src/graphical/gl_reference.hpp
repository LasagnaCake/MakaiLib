struct Empty {
	virtual void onTransform() {}
	virtual Empty* reset() {return this;}
	virtual Empty* transform() {return this;}
	virtual Triangle** getBoundTriangles() {return nullptr;}
	bool fixed = true;
	bool visible = true;
	Transform3D local;
};

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
		tl = tr1 = tr2 = bl1 = bl2 = br = nullptr;
		tris[0] = tris[1] = nullptr;
	}

	#define VERTEX_SET_POS(VERT, VAL) Drawer::vertexSetPosition(VERT, VAL);
	/// Sets the plane's origin.
	Plane* setOrigin(
			Vector3 tlPos,
			Vector3 trPos,
			Vector3 blPos,
			Vector3 brPos
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
		VERTEX_SET_POS(*tl,	origin[0]);
		VERTEX_SET_POS(*tr1,	origin[1]);
		VERTEX_SET_POS(*tr2,	origin[2]);
		VERTEX_SET_POS(*bl1,	origin[1]);
		VERTEX_SET_POS(*bl2,	origin[2]);
		VERTEX_SET_POS(*br,	origin[3]);
		return this;
	}

	Plane* transform() override {
		onTransform();
		if (!fixed) return this;
		// Get transformation
		Transform3D self = local;
		//self.scale *= (float)visible;
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

	Triangle** getBoundTriangles() override {
		return tris;
	}

protected:

	Triangle* tris[2] = {nullptr, nullptr};

	Vector3 origin[4];

	RawVertex* tl	= nullptr;
	RawVertex* tr1	= nullptr;
	RawVertex* tr2	= nullptr;
	RawVertex* bl1	= nullptr;
	RawVertex* bl2	= nullptr;
	RawVertex* br	= nullptr;
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

#undef VERTEX_SET_POS
