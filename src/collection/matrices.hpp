#ifndef FLOAT_MATRICES_234_H
#define FLOAT_MATRICES_234_H

#include "algebra.hpp"
#include "vectorn.hpp"
#include "helper.hpp"

namespace MatType {
	template <size_t A, size_t B>
	concept EqualSize = (A == B);

	template <typename T1, typename T2>
	concept Compatitble = Math::Operatable<T2> && Type::Convertible<T2, T1>;
}

/**
* [---------------------]
* [                     ]
* [  Base Matrix Class  ]
* [                     ]
* [---------------------]
*/
template<size_t R, size_t C, Math::Operatable T>
class Matrix {
public:
	static_assert(R != 0, "Matrix row size must not be zero!");
	static_assert(C != 0, "Matrix column size must not be zero!");

	/// Constructors.
	constexpr Matrix() {
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				data[i][j] = 0;
	}

	constexpr Matrix(T const& v) {
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				data[i][j] = v;
	}

	constexpr Matrix(const T(&v)[R][C]) {
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				data[i][j] = v[i][j];
	}

	constexpr Matrix(const T(&v)[R*C]) {
		for (size_t i = 0; i < R*C; i++)
			((float*)data)[i] = v[i];
	}

	template<MatType::Compatitble<T> T2>
	constexpr Matrix(const T2(&v)[R][C]) {
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				data[i][j] = T(v[i][j]);
	}

	template<MatType::Compatitble<T> T2>
	constexpr Matrix(const T2(&v)[R*C]) {
		for (size_t i = 0; i < R*C; i++)
			((float*)data)[i] = T(v[i]);
	}

	constexpr Matrix(Vector2 const& vec) requires MatType::EqualSize<R, 2> {
		static_assert(R == 2 && C == 1, "Matrix is not a valid representation of a 2D vector!");
		data[0][0] = vec.x;
		data[0][1] = vec.y;
	}

	constexpr Matrix(Vector3 const& vec) requires MatType::EqualSize<R, 3> {
		static_assert(R == 3 && C == 1, "Matrix is not a valid representation of a 3D vector!");
		data[0][0] = vec.x;
		data[0][1] = vec.y;
		data[0][2] = vec.z;
	}

	constexpr Matrix(Vector4 const& vec) requires MatType::EqualSize<R, 4> {
		static_assert(R == 4 && C == 1, "Matrix is not a valid representation of a 4D vector!");
		data[0][0] = vec.x;
		data[0][1] = vec.y;
		data[0][2] = vec.z;
		data[0][3] = vec.w;
	}

	constexpr Matrix(Matrix<R, C, T> const& other) {
		for (size_t i = 0; i < R; i++) {
			for (size_t j = 0; j < C; j++)
				data[i][j] = other.data[i][j];
		}
	}

	constexpr ~Matrix() {}

	/// Gets the transposed matrix.
	constexpr Matrix<C, R, T> transposed() const {
		Matrix<C, R, T> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				res[j][i] = data[i][j];
		return res;
	}

	constexpr static Matrix<R, C, T> identity() requires MatType::EqualSize<R, C> {
		static_assert(C == R, "Matrix is not a square matrix!");
		Matrix<R, C, T> res(0);
		for(size_t i = 0; i < R; i++)
			res.data[i][j] = 1;
		return res;
	}

	constexpr Matrix<R, C, T> inverted() const requires MatType::EqualSize<R, C> {
		static_assert(C == R, "Matrix is not a square matrix!");
		static_assert(determinant() != T(0), "Determinant cannot be zero!");
		Matrix<R, C, T> res;
		T det = determinant();
		if (det == T(0)) return Matrix::identity();
		res = cofactors().transposed() * (T(1) / det);
		return res;
	}

	constexpr Matrix<R, C, T> cofactors() const requires MatType::EqualSize<R, C> {
		static_assert(C == R, "Matrix is not a square matrix!");
		Matrix<R, C, T> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				res[i][j] = cofactor(i, j);
		return res;
	}

	constexpr T determinant() const requires MatType::EqualSize<R, C> {
		static_assert(C == R, "Matrix is not a square matrix!");
		if constexpr(C == 1)		return data[0][0];
		else if constexpr(C == 2)	return data[0][0] * data[1][1] - data[1][0] * data[0][1];
		else if constexpr(C == 3)	return (
			(data[0][0] * data[1][1] * data[2][2])
		+	(data[0][1] * data[1][2] * data[2][0])
		+	(data[0][2] * data[1][0] * data[2][1])
		-	(data[0][2] * data[1][1] * data[2][0])
		-	(data[0][0] * data[1][2] * data[2][1])
		-	(data[0][1] * data[1][0] * data[2][2])
		);
		else {
			T res;
			for (size_t i = 0; i < R; i++) {
				if (data[i][0] == 0) continue;
				res += data[i][0] * cofactor(i, 0);
			}
			return res;
		}
	}

	constexpr Matrix<R-1, C-1, T> truncated(size_t const& row, size_t const& col) const {
		static_assert(R > 1 && C > 1, "Cannot truncate a 1-dimensional matrix!");
		Matrix<R-1, C-1, T> res;
		int ro = 0, co = 0;
		for (size_t i = 0; i < R; i++) {
			co = 0;
			if (i == row) {ro--; continue;}
			for (size_t j = 0; j < C; j++) {
				if (j == col) {co--; continue;}
				res[i+ro][j+co] = data[i][j];
			}
		}
		return res;
	}

	template<size_t RF = 1, size_t CF = 1>
	constexpr Matrix<R-RF, C-CF, T> shrunkBy(size_t const& rowStart = 0, size_t const& colStart = 0) const {
		static_assert(R > 1 && C > 1, "Cannot shrink a 1-dimensional matrix!");
		static_assert(RF > R && CF > C, "Shrinking factor(s) are bigger than the matrix!");
		static_assert(rowStart < (R-RF) && colStart < (C-CF), "Row/Column starts cannot be bigger than the shrunk matrix!");
		if (rowStart < (R-RF) && colStart < (C-CF))
			throw Error::InvalidValue(
				"Invalid row / column start!",
				__FILE__,
				toString(__LINE__),
				"shrunkBy",
				"Values cannot be bigger than the shrunk matrix's dimensions!"
			);
		Matrix<R-RF, C-CF, T> res;
		for (size_t i = 0; i < R-RF; i++)
			for (size_t j = 0; j < C-CF; j++)
				res[i][j] = data[i+rowStart][j+colStart];
		return res;
	}

	template<size_t RF = 1, size_t CF = 1>
	constexpr Matrix<R+RF, C+CF, T> expandedBy(size_t const& rowStart = 0, size_t const& colStart = 0) const {
		static_assert(rowStart < RF && colStart < CF, "Row/Column starts cannot be bigger than the expansion factor!");
		if (rowStart < (RF) && colStart < (CF))
			throw Error::InvalidValue(
				"Invalid row / column start!",
				__FILE__,
				toString(__LINE__),
				"shrunkBy",
				"Values cannot be bigger than the expansion factors!"
			);
		Matrix<R+RF, C+CF, T> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				res[i+rowStart][j+colStart] = data[i][j];
		return res;
	}

	/// Arithmetic operator overloading.

	constexpr Matrix<R, C, T> operator+(T const& val) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				res[j][i] = data[i][j] + val;
		return res;
	}

	constexpr Matrix<R, C, T> operator+(Matrix<C, R, T> const& mat) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				res[j][i] = data[i][j] + mat.data[i][j];
		return res;
	}

	constexpr Matrix<R, C, T> operator-(T const& val) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				res[j][i] = data[i][j] - val;
		return res;
	}

	constexpr Matrix<R, C, T> operator-(Matrix<R, C, T> const& mat) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				res[j][i] = data[i][j] - mat.data[i][j];
		return res;
	}

	constexpr Matrix<R, C, T> operator*(T const& val) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				res[j][i] = data[i][j] * val;
		return res;
	}

	template<size_t C2>
	constexpr Matrix<R, C2, T> operator*(Matrix<C, C2, T> const& mat) const {
		Matrix<R, C2, T> res;
		auto nmat = mat.transposed();
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C2; j++)
				res[i][j] = rdp(i, nmat.data[j]);
		return res;
	}

	constexpr Matrix<R, C, T> operator/(T const& val) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				res[j][i] = data[i][j] / val;
		return res;
	}

	template<size_t C2>
	constexpr Matrix<R, C2, T> operator/(Matrix<C, C2, T> const& mat) const {
		return (*this) * mat.inverted();
	}

	/// Assignment operator overloading.

	constexpr Matrix<R, C, T>& operator=(const T(&v)[R][C]) {
		for (size_t i = 0; i < R; i++) {
			for (size_t j = 0; j < C; j++)
				data[i][j] = T(v[i][j]);
		}
	}

	constexpr Matrix<R, C, T>& operator=(const T(&v)[R*C]) {
		for (size_t i = 0; i < R*C; i++)
			((float*)data)[i] = T(v[i]);
	}

	constexpr Matrix<R, C, T>& operator+=(T const& val) {
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				data[i][j] += val;
		return *this;
	}

	constexpr Matrix<R, C, T>& operator+=(Matrix<C, R, T> const& mat) {
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				data[i][j] += mat.data[i][j];
		return *this;
	}

	constexpr Matrix<R, C, T>& operator-=(T const& val) {
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				data[i][j] -= val;
		return *this;
	}

	constexpr Matrix<R, C, T>& operator-=(Matrix<R, C, T> const& mat) {
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				data[i][j] -= mat.data[i][j];
		return *this;
	}

	/// Other operator overloadings.

	constexpr Span<T, C> operator[](size_t const& idx) {
		return Span{data[idx]};
	}

	template <MatType::Compatitble<T> T2>
	constexpr operator Matrix<R, C, T2>() const {return Matrix<R, C, T2>(data);}

	constexpr operator Vector2() const requires MatType::EqualSize<R, 2> {return toVector2();}
	constexpr operator Vector3() const requires MatType::EqualSize<R, 3> {return toVector3();}
	constexpr operator Vector4() const requires MatType::EqualSize<R, 4> {return toVector4();}

	/// Size accessors.

	constexpr static size_t rowCount() {return R;}

	constexpr static size_t columnCount() {return C;}

	/// Converters.

	constexpr Vector2 toVector2() const requires MatType::EqualSize<R, 2> {
		static_assert(R == 2, "Matrix is not a valid representation of a 2D vector!");
		return Vector2(data[0][C-1], data[1][C-1]);
	}

	constexpr Vector3 toVector3() const requires MatType::EqualSize<R, 3> {
		static_assert(R == 3, "Matrix is not a valid representation of a 3D vector!");
		return Vector2(data[0][C-1], data[1][C-1], data[2][C-1]);
	}

	constexpr Vector4 toVector4() const requires MatType::EqualSize<R, 4> {
		static_assert(R == 4, "Matrix is not a valid representation of a 4D vector!");
		return Vector2(data[0][C-1], data[1][C-1], data[2][C-1], data[3][C-1]);
	}

	[[unavailable("Unimplemented!")]]
	constexpr void compose(VecMath::Transform3D trans) requires MatType::EqualSize<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
	}

	[[unavailable("Unimplemented!")]]
	constexpr VecMath::Transform3D decompose() const requires MatType::EqualSize<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		VecMath::Transform3D res;
		res.position = Vector3(data[0][3], data[1][3], data[2][3]);
		Vector3
			sx = Vector3(data[0][0], data[1][0], data[2][0]),
			sy = Vector3(data[0][1], data[1][1], data[2][1]),
			sz = Vector3(data[0][2], data[1][2], data[2][2])
		;
		if (sx.crossProd(sy).dotProd(sz) < 0) sx *= -1;
		res.scale = Vector3(sx.length(), sy.length(), sz.length());

	}

	/// Gets the dot product of a given row by another given row.
	constexpr T rdp(size_t const& row, const T (&other)[R]) const {
		T res = 0;
		for (size_t i = 0; i < R; i++)
			res += other[i] * data[row][i];
		return res;
	}

	/// Gets the dot product of a given row by another given row.
	constexpr T rdp(size_t const& row, Span<T, R> const& other) const {
		T res = 0;
		for (size_t i = 0; i < R; i++)
			res += other[i] * data[row][i];
		return res;
	}

	constexpr T cofactor(size_t const& row, size_t const& col) const {
		return ((((row + col) % 2) == 0) ? T(+1) : T(-1)) * truncated(row, col).determinant();
	}

private:
	template<size_t R2, size_t C2, Math::Operatable T2> friend class Matrix;

	/// The matrix's columns;
	T data[R][C];
};
// Float matrices
typedef Matrix<2, 1, float> Matrix2x1;
typedef Matrix<3, 1, float> Matrix3x1;
typedef Matrix<4, 1, float> Matrix4x1;

typedef Matrix<2, 2, float> Matrix2x2;
typedef Matrix<3, 2, float> Matrix3x2;
typedef Matrix<4, 2, float> Matrix4x2;

typedef Matrix<2, 3, float> Matrix2x3;
typedef Matrix<3, 3, float> Matrix3x3;
typedef Matrix<4, 3, float> Matrix4x3;

typedef Matrix<2, 4, float> Matrix2x4;
typedef Matrix<3, 4, float> Matrix3x4;
typedef Matrix<4, 4, float> Matrix4x4;

// Integer matrices
typedef Matrix<2, 1, int> Matrix2x1i;
typedef Matrix<3, 1, int> Matrix3x1i;
typedef Matrix<4, 1, int> Matrix4x1i;

typedef Matrix<2, 2, int> Matrix2x2i;
typedef Matrix<3, 2, int> Matrix3x2i;
typedef Matrix<4, 2, int> Matrix4x2i;

typedef Matrix<2, 3, int> Matrix2x3i;
typedef Matrix<3, 3, int> Matrix3x3i;
typedef Matrix<4, 3, int> Matrix4x3i;

typedef Matrix<2, 4, int> Matrix2x4i;
typedef Matrix<3, 4, int> Matrix3x4i;
typedef Matrix<4, 4, int> Matrix4x4i;

// Float shorthands
typedef Matrix2x1 Mat2x1;
typedef Matrix3x1 Mat3x1;
typedef Matrix4x1 Mat4x1;

typedef Matrix2x2 Mat2x2;
typedef Matrix3x2 Mat3x2;
typedef Matrix4x2 Mat4x2;

typedef Matrix2x3 Mat2x3;
typedef Matrix3x3 Mat3x3;
typedef Matrix4x3 Mat4x3;

typedef Matrix2x4 Mat2x4;
typedef Matrix3x4 Mat3x4;
typedef Matrix4x4 Mat4x4;

// Integer shorthands
typedef Matrix2x1i Mat2x1i;
typedef Matrix3x1i Mat3x1i;
typedef Matrix4x1i Mat4x1i;

typedef Matrix2x2i Mat2x2i;
typedef Matrix3x2i Mat3x2i;
typedef Matrix4x2i Mat4x2i;

typedef Matrix2x3i Mat2x3i;
typedef Matrix3x3i Mat3x3i;
typedef Matrix4x3i Mat4x3i;

typedef Matrix2x4i Mat2x4i;
typedef Matrix3x4i Mat3x4i;
typedef Matrix4x4i Mat4x4i;

namespace MatMath {
}

#endif // FLOAT_MATRICES_234_H
