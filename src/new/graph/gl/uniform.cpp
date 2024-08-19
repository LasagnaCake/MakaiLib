#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

#include "uniform.hpp"

using namespace Makai::Graph;

Uniform::Uniform(String const& _name, GLuint const& _id):
	name(_name),
	id(_id),
	location(glGetUniformLocation(_id, _name.c_str()))
{}

void Uniform::set(bool const& value, usize const& offset = 0) const {
	this->offset = 0;
	glUniform1i(getUniform() + offset, (int)value);
	++this->offset;
}

void Uniform::set(int const& value, usize const& offset = 0) const {
	this->offset = 0;
	glUniform1i(getUniform() + offset, value);
	++this->offset;
}

void Uniform::set(unsigned int const& value, usize const& offset = 0) const {
	this->offset = 0;
	glUniform1ui(getUniform() + offset, value);
	++this->offset;
}

void Uniform::set(float const& value, usize const& offset = 0) const {
	this->offset = 0;
	glUniform1f(getUniform() + offset, value);
}

void Uniform::set(Vector2 const& value, usize const& offset = 0) const {
	this->offset = 0;
	glUniform2f(getUniform() + offset, value.x, value.y);
	++this->offset;
}

void Uniform::set(Vector3 const& value, usize const& offset = 0) const {
	this->offset = 0;
	glUniform3f(getUniform() + offset, value.x, value.y, value.z);
	++this->offset;
}

void Uniform::set(Vector4 const& value, usize const& offset = 0) const {
	this->offset = 0;
	glUniform4f(getUniform() + offset, value.x, value.y, value.z, value.w);
	++this->offset;
}

void Uniform::set(Matrix3x3 const& value, usize const& offset = 0) const {
	this->offset = 0;
	glUniformMatrix3fv(getUniform() + offset, 1, GL_FALSE, value.begin());
	++this->offset;
}

void Uniform::set(Matrix4x4 const& value, usize const& offset = 0) const {
	this->offset = 0;
	glUniformMatrix4fv(getUniform() + offset, 1, GL_FALSE, value.begin());
}

void Uniform::setArray(int* const& values, usize const& count, usize const& offset = 0) const {
	this->offset = 0;
	glUniform1iv(getUniform() + offset, count, values);
	this->offset = count;
}

void Uniform::etArray(unsigned int* const& values, usize const& count, usize const& offset = 0) const {
	this->offset = 0;
	glUniform1uiv(getUniform() + offset, count, values);
	this->offset = count;
}

void Uniform::setArray(float* const& values, usize const& count, usize const& offset = 0) const {
	this->offset = 0;
	glUniform1fv(getUniform() + offset, count, values);
	this->offset = count;
}

void Uniform::setArray(Vector2* const& values, usize const& count, usize const& offset = 0) const {
	this->offset = 0;
	for SRANGE(i, 0, count)
		glUniform2f(getUniformArray(i) + offset, values[i].x, values[i].y);
	this->offset = count;
}

void Uniform::setArray(Vector3* const& values, usize const& count, usize const& offset = 0) const {
	this->offset = 0;
	for SRANGE(i, 0, count)
		glUniform3f(getUniformArray(i)+ offset, values[i].x, values[i].y, values[i].z);
	this->offset = count;
}

void Uniform::setArray(Vector4* const& values, usize const& count, usize const& offset = 0) const {
	this->offset = 0;
	for SRANGE(i, 0, count)
		glUniform4f(getUniformArray(i)+ offset, values[i].x, values[i].y, values[i].z, values[i].w);
	this->offset = count;
}

template <typename T>
void Uniform::set(List<T> const& values, usize const& offset = 0) const {
	this->offset = 0;
	setArray((T*)values.data(), values.size(), offset);
	this->offset = values.size();
}

template <typename T, usize S>
void Uniform::set(Span<T, S> const& values, usize const& offset = 0) const {
	this->offset = 0;
	setArray((T*)values.data(), values.size(), offset);
	this->offset = values.size();
}

template <typename... Args>
void Uniform::set(Args const&... args) const
requires (sizeof...(Args) > 1) {
	this->offset = 0;
	usize off = 0;
	(setSpecial(args, off), ...);
	offset = off;
}

template <Type::Class T>
void Uniform::forEach(List<T> const& values, Functor<void(T&, Uniform const&)> func) const {
	for (T& val: values) {
		func(val, Uniform(name, id, location + offset+1));
	}
}

template <typename T>			void Uniform::operator()(T const& value)			{set(value);	}
template <typename... Args>		void Uniform::operator()(Args const&... args)		{set(args...);	}
template <typename T>			void Uniform::operator()(List<T> const& values)		{set(values);	}
template <typename T, usize S>	void Uniform::operator()(Span<T, S> const& values)	{set(values);	}

template<typename T>
void Uniform::setSpecial(T const& value, usize& offset) const {
	set(value, offset);
	++offset;
}

template <typename T>
void Uniform::setSpecial(List<T> const& values, usize& offset) const {
	set(values, offset);
	offset += this->offset;
}

template <typename T, usize S>
void Uniform::setSpecial(Span<T, S> const& values, usize& offset) const {
	set(values, offset);
	offset += this->offset;
}

uint Uniform::getUniformArray(usize const& offset) const {
	return location + offset + this->offset;
}

inline uint Uniform::getUniform() const {
	return location + offset;
}

inline uint Uniform::getUniform(String const& append) const {
	return glGetUniformLocation(id, (name + append).c_str());
}
