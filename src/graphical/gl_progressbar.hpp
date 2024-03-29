namespace {
	using Base::DrawableObject;
}

class BaseBar {
public:
	float uvShift = 0;
	float value = 0, max = 100;
	Vector2 size = 1;
	Vector2 uvScale = Vector2(1);
	bool 	dynamicUV	= true;
};

#ifndef RADIAL_BAR_RESOLUTION
#define RADIAL_BAR_RESOLUTION 12
#endif // RADIAL_BAR_RESOLUTION

template<class T>
concept BarType = Type::Subclass<T, BaseBar> && Type::Subclass<T, Base::Drawable>;

class LinearBar: public DrawableObject<Material::ObjectMaterial>, public BaseBar {
public:
	LinearBar(size_t layer = 0, bool manual = false): DrawableObject(layer, manual) {
	}

private:
	RawVertex vertices[4];
	void draw() override {
		float percent = Math::clamp((value / max), 0.0f, 1.0f);
		float
			length	= percent * size.x,
			width	= size.y / 2;
		vertices[0]	= toRawVertex(Vector2(0, -width),		Vector2(0+uvShift,1) * uvScale);
		vertices[1]	= toRawVertex(Vector2(0, +width),		Vector2(0+uvShift,0) * uvScale);
		vertices[2]	= toRawVertex(Vector2(length, -width),	Vector2((dynamicUV ? percent : 1)+uvShift,1) * uvScale);
		vertices[3]	= toRawVertex(Vector2(length, +width),	Vector2((dynamicUV ? percent : 1)+uvShift,0) * uvScale);
		setShaderData();
		Material::setMaterial(shader, material);
		display(vertices, 4, GL_TRIANGLE_STRIP);
	}
};

class RadialBar: public DrawableObject<Material::ObjectMaterial>, public BaseBar {
public:
	bool	centered	= false;
	Vector2 offset		= Vector2(0);

	RadialBar(size_t layer = 0, bool manual = false): DrawableObject(layer, manual) {
		vertices[0].u = vertices[0].v = 0.5;
	}

private:
	RawVertex vertices[RADIAL_BAR_RESOLUTION + 2] = {RawVertex{}};
	void draw() override {
		update();
		setShaderData();
		Material::setMaterial(shader, material);
		display(vertices, RADIAL_BAR_RESOLUTION + 2, GL_TRIANGLE_FAN);
	}

	void update() {
		// Progress bar percentage angle (in radians)
		float fraction = TAU * Math::clamp(value/max, 0.0f, 1.0f);
		// Set center to offset
		Drawer::vertexSetPosition(vertices[0], offset.yx() * size);
		// For each surrounding vertex...
		for SSRANGE(i, 1, RADIAL_BAR_RESOLUTION + 2) {
			// UV fraction, Positional fraction
			float uvfrac, posfrac;
			// Get current vertex's fraction
			uvfrac = posfrac = ((float)(i-1))/(RADIAL_BAR_RESOLUTION);
			uvfrac *= TAU;
			// Multiply by progress' percentage angle
			posfrac *= fraction;
			// Center if aplicable
			if (centered) posfrac -= fraction/2.0 + PI;
			// Dynamicise UV if aplicable
			if (dynamicUV) uvfrac = posfrac;
			// Add angle offset to UV fraction
			uvfrac += uvShift;
			// Set vertex position
			Drawer::vertexSetPosition(vertices[i], VecMath::angleV2(posfrac) * size);
			// Set vertex UV
			Vector2 uv = (((VecMath::angleV2(uvfrac + uvShift) / 2.0) + 0.5)) * uvScale;
			uv.y = 1 - uv.y;
			Drawer::vertexSetUV(vertices[i], uv);
		}
	}
};

template<BarType T = RadialBar>
struct LabeledBar {
	typedef T DataType;
	T			bar;
	Text::Label	label;
};
