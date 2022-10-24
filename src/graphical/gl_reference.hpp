struct Empty {
	virtual void onTransform() {}
	virtual Empty* reset() {return this;}
	virtual Empty* transform() {return this;}
	virtual Triangle** getTriangles() {return nullptr;}
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
		Vertex* tl,
		Vertex* tr1,
		Vertex* tr2,
		Vertex* bl1,
		Vertex* bl2,
		Vertex* br
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
	}

	/// Sets the plane's origin.
	Plane* setOrigin(
			Vector3 tlPos,
			Vector3 trPos,
			Vector3 blPos,
			Vector3 brPos
		) {
		origin[0] = tl->position	= tlPos;
		origin[1] = tr1->position	= trPos;
		origin[2] = tr2->position	= trPos;
		origin[3] = bl1->position	= blPos;
		origin[4] = bl2->position	= blPos;
		origin[5] = br->position	= brPos;
		return this;
	}

	/// Transforms the plane's origin by a given transform.
	Plane* setOrigin(Transform3D trans) {
		glm::mat4 glmtrans = asGLMMatrix(trans);
		origin[0] = tl->position	= $srpTransform(origin[0], glmtrans);
		origin[1] = tr1->position	= $srpTransform(origin[1], glmtrans);
		origin[2] = tr2->position	= $srpTransform(origin[2], glmtrans);
		origin[3] = bl1->position	= $srpTransform(origin[3], glmtrans);
		origin[4] = bl2->position	= $srpTransform(origin[4], glmtrans);
		origin[5] = br->position	= $srpTransform(origin[5], glmtrans);
		return this;
	}

	Plane* setUV(
			Vector2 tlUV,
			Vector2 trUV,
			Vector2 blUV,
			Vector2 brUV
		) {
		tl->uv	= tlUV;
		tr1->uv	= trUV;
		tr2->uv	= trUV;
		bl1->uv	= blUV;
		bl2->uv	= blUV;
		br->uv	= brUV;
		return this;
	}

	Plane* setColor(
			Vector4 tlCol,
			Vector4 trCol,
			Vector4 blCol,
			Vector4 brCol
		) {
		tl->color	= tlCol;
		tr1->color	= trCol;
		tr2->color	= trCol;
		bl1->color	= blCol;
		bl2->color	= blCol;
		br->color	= brCol;
		return this;
	}

	Plane* setColor(
			Vector4 col = Color::WHITE
		) {
		tl->color	= col;
		tr1->color	= col;
		tr2->color	= col;
		bl1->color	= col;
		bl2->color	= col;
		br->color	= col;
		return this;
	}

	/// Sets the plane to its original state (last state set with setPosition).
	Plane* reset() override {
		tl->position	= origin[0];
		tr1->position	= origin[1];
		tr2->position	= origin[2];
		bl1->position	= origin[3];
		bl2->position	= origin[4];
		br->position	= origin[5];
		return this;
	}

	Plane* transform() override {
		onTransform();
		if (!fixed) return this;
		// Get transformation
		glm::mat4 trans = asGLMMatrix(local);
		// Apply transformation
		tl->position	= $srpTransform(tl->position, trans)	* visible;
		tr1->position	= $srpTransform(tr1->position, trans)	* visible;
		tr2->position	= $srpTransform(tr2->position, trans)	* visible;
		bl1->position	= $srpTransform(bl1->position, trans)	* visible;
		bl2->position	= $srpTransform(bl2->position, trans)	* visible;
		br->position	= $srpTransform(br->position, trans)	* visible;
		return this;
	}

	Triangle** getTriangles() override {
		return tris;
	}

private:

	Triangle* tris[2] = {nullptr, nullptr};

	Vector3 origin[6];

	Vertex* tl	= nullptr;
	Vertex* tr1	= nullptr;
	Vertex* tr2	= nullptr;
	Vertex* bl1	= nullptr;
	Vertex* bl2	= nullptr;
	Vertex* br	= nullptr;
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
