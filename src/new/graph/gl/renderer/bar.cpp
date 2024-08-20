#include "bar.hpp"

using namespace Makai::Graph;

void LinearBar::draw() override{
	float percent = Math::clamp((value / max), 0.0f, 1.0f);
	float
		length	= percent * size.x,
		width	= size.y / 2
	;
	vertices[0]	= Vertex::from(Vector2(0, -width),		Vector2(0+uvShift,1) * uvScale);
	vertices[1]	= Vertex::from(Vector2(0, +width),		Vector2(0+uvShift,0) * uvScale);
	vertices[2]	= Vertex::from(Vector2(length, -width),	Vector2((dynamicUV ? percent : 1)+uvShift,1) * uvScale);
	vertices[3]	= Vertex::from(Vector2(length, +width),	Vector2((dynamicUV ? percent : 1)+uvShift,0) * uvScale);
	prepare();
	material.use(material);
	display(vertices, 4, ObjectDisplayMode::ODM_TRIANGLE_STRIP);
}

void RadialBar::draw() override {
	update();
	prepare();
	material.use(material);
	display(vertices, RADIAL_BAR_RESOLUTION + 2, ObjectDisplayMode::ODM_TRIANGLE_FAN);
}

void RadialBar::update() {
	// Progress bar percentage angle (in radians)
	float fraction = TAU * Math::clamp(value/max, 0.0f, 1.0f);
	// Set center to offset
	Vertex::setPosition(vertices[0], offset.yx() * size);
	// For each surrounding vertex...
	for SRANGE(i, 1, RADIAL_BAR_RESOLUTION + 2) {
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
		Vertex::setPosition(vertices[i], VecMath::angleV2(posfrac) * size);
		// Set vertex UV
		Vector2 uv = (((VecMath::angleV2(uvfrac + uvShift) / 2.0) + 0.5)) * uvScale;
		uv.y = 1 - uv.y;
		Vertex::setUV(vertices[i], uv);
	}
}
