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
	float	uvAngle		= 0;
	bool	centered	= false;
	bool 	dynamicUV	= true;
	Vector2 offset;

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
		// Progress bar percentage angle (in radians)
		float fraction = TAU * Math::clamp(value/max, 0.0f, 1.0f);
		// Set center to offset
		Drawer::vertexSetPosition(vertices[0], offset * size);
		// For each surrounding vertex...
		for $ssrange(i, 1, RADIAL_BAR_RESOLUTION + 2) {
			// UV fraction, Positional fraction
			float uvfrac, posfrac;
			// Get current vertex's fraction
			uvfrac = posfrac = ((float)(i-1))/(RADIAL_BAR_RESOLUTION);
			// Multiply by progress' percentage angle
			posfrac *= fraction;
			// Center if aplicable
			if (centered)	{
				posfrac	-= fraction/2.0 * PI;
				uvfrac	-= fraction/2.0 * PI;
			}
			// Dynamicise UV if aplicable
			if (dynamicUV) uvfrac = posfrac;
			// Add angle offset to UV fraction
			uvfrac += uvAngle;
			// Set vertex position
			Vector2 pos = VecMath::angleV2(posfrac);
			Drawer::vertexSetPosition(vertices[i], pos * size);
			// Set vertex UV
			Drawer::vertexSetUV(vertices[i], (((VecMath::angleV2(uvfrac + uvAngle) / 2.0) + 0.5)));
		}
	}
};

#define $bar RenderData::Bar::
