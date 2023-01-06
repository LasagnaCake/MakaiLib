namespace {
	using Base::DrawableObject;
}

struct BaseBar {
	RawVertex vertices[5];
};

class LinearBar: public DrawableObject, private BaseBar {
public:
	float value = 0, max = 100;

	LinearBar(size_t layer = 0, bool manual = false): DrawableObject(layer, manual) {
	}

private:
	void draw() override {
		setDefaultShader();
		display(vertices, 4, GL_TRIANGLE_STRIP);
	}
};

class RadialBar: public DrawableObject, private BaseBar {
public:
	float value = 0, max = 100;

	float uvAngle = 0;

	RadialBar(size_t layer = 0, bool manual = false): DrawableObject(layer, manual) {
		vertices[0].u = vertices[0].v = vertices[0].x = vertices[0].y = 0.5;
	}

private:
	void draw() override {
		setDefaultShader();
		display(vertices, 5, GL_TRIANGLE_FAN);
	}
};
