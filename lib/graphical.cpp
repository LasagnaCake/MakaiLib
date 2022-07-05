#include "graphical.hpp"

using namespace Vector;

RenderData::GenericShape::GenericShape(Triangle* triangle) {
	this->shape.triangle = triangle;
	type = 'T';
}

RenderData::GenericShape::GenericShape(Plane* plane) {
	this->shape.plane = plane;
	type = 'P';
}
