namespace {
	using Base::DrawableObject;
}

class BaseBar {
public:
	float value = 0, max = 100;
	Vector2 size = 1;
};

#ifndef RADIAL_BAR_RESOLUTION
#define RADIAL_BAR_RESOLUTION 12
#endif // RADIAL_BAR_RESOLUTION

class LinearBar: public DrawableObject, public BaseBar {
public:

	LinearBar(size_t layer = 0, bool manual = false): DrawableObject(layer, manual) {
	}

private:
	RawVertex vertices[4];
	void draw() override {
		float
			length	= Math::clamp((value / max), 0.0f, 1.0f) * size.x,
			width	= size.y / 2;
		vertices[0]	= toRawVertex(Vector2(0, -width),		Vector2(0,0));
		vertices[1]	= toRawVertex(Vector2(0, +width),		Vector2(0,1));
		vertices[2]	= toRawVertex(Vector2(length, -width),	Vector2(1,0));
		vertices[3]	= toRawVertex(Vector2(length, +width),	Vector2(1,1));
		setDefaultShader();
		display(vertices, 4, GL_TRIANGLE_STRIP);
	}
};

class RadialBar: public DrawableObject, public BaseBar {
public:
	float uvAngle	= 0;
	bool centered	= false;

	RadialBar(size_t layer = 0, bool manual = false): DrawableObject(layer, manual) {
		vertices[0].u = vertices[0].v = 0.5;
	}

private:
	RawVertex vertices[RADIAL_BAR_RESOLUTION + 3];
	void draw() override {
		update();
		setDefaultShader();
		display(vertices, RADIAL_BAR_RESOLUTION + 2, GL_TRIANGLE_FAN);
	}

	void update() {
		float fraction = TAU * Math::clamp(value/max, 0.0f, 1.0f);
		for $ssrange(i, 1, RADIAL_BAR_RESOLUTION + 2) {
			float ifrac = ((float)(i-1))/(RADIAL_BAR_RESOLUTION) * fraction - (centered ? (fraction/2 + PI) : 0);
			Vector2 pos = VecMath::angleV2(ifrac);
			Drawer::vertexSetPosition(vertices[i], pos * size);
			Drawer::vertexSetUV(vertices[i], (((VecMath::angleV2(ifrac + uvAngle) / 2.0) + 0.5)));
		}
	}
};

#define $bar RenderData::Bar::
