class PlaneReference {
public:
	PlaneReference(
		Triangle tris[2]
	) {
		this->tl	= &(tris[0].verts[0]);
		this->tr1	= &(tris[0].verts[1]);
		this->tr2	= &(tris[1].verts[0]);
		this->bl1	= &(tris[0].verts[2]);
		this->bl2	= &(tris[1].verts[1]);
		this->br	= &(tris[1].verts[2]);
	}
		PlaneReference(
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

	virtual ~PlaneReference() {}

	/// Sets the plane's origin.
	PlaneReference* setOrigin(
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
	PlaneReference* setOrigin(Transform3D trans) {
		origin[0] = tl->position	= SRP_TRANSFORM(origin[0], trans);
		origin[1] = tr1->position	= SRP_TRANSFORM(origin[1], trans);
		origin[2] = tr2->position	= SRP_TRANSFORM(origin[2], trans);
		origin[3] = bl1->position	= SRP_TRANSFORM(origin[3], trans);
		origin[4] = bl2->position	= SRP_TRANSFORM(origin[4], trans);
		origin[5] = br->position	= SRP_TRANSFORM(origin[5], trans);
		return this;
	}

	PlaneReference* setUV(
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

	PlaneReference* setColor(
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

	PlaneReference* setColor(
			Vector4 col = Vector4(1.0, 1.0, 1.0, 1.0)
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
	PlaneReference* reset() {
		tl->position	= origin[0];
		tr1->position	= origin[1];
		tr2->position	= origin[2];
		bl1->position	= origin[3];
		bl2->position	= origin[4];
		br->position	= origin[5];
		return this;
	}

	virtual void onTransform() { }

	PlaneReference* transform() {
		onTransform();
		if (!fixed) return this;
		// Apply transformation
		tl->position	= SRP_TRANSFORM(tl->position, local);
		tr1->position	= SRP_TRANSFORM(tr1->position, local);
		tr2->position	= SRP_TRANSFORM(tr2->position, local);
		bl1->position	= SRP_TRANSFORM(bl1->position, local);
		bl2->position	= SRP_TRANSFORM(bl2->position, local);
		br->position	= SRP_TRANSFORM(br->position, local);
		return this;
	}

	/// Whether the plane will be transformed when transform() is called
	bool fixed = true;

	Transform3D local;

private:

	Vector3 origin[6];

	Vertex* tl;
	Vertex* tr1;
	Vertex* tr2;
	Vertex* bl1;
	Vertex* bl2;
	Vertex* br;
};

class AnimatedPlaneReference: public PlaneReference {
public:
	using PlaneReference::PlaneReference;
	Vector2 sprite;
	Vector2 size = Vector2(1);

	void onTransform() override {
		if (size.x == 0 || size.y == 0)
			throw runtime_error("Sprite count must be bigger than zero!");
		setUV(
			(sprite) / size,
			(sprite + Vector2(1, 0)) / size,
			(sprite + Vector2(0, 1)) / size,
			(sprite + Vector2(1)) / size
		);
	}
};
