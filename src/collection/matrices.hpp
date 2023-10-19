#ifndef FLOAT_MATRICES_234_H
#define FLOAT_MATRICES_234_H

#include "algebra.hpp"
#include "vectorn.hpp"
#include "helper.hpp"
#include <vector>

namespace Matrix {
	namespace {
		using
			Vector::Vector2,
			Vector::Vector3,
			Vector::Vector4
		;
	}

	/**
	* [---------------------]
	* [                     ]
	* [  Base Matrix Class  ]
	* [                     ]
	* [---------------------]
	*/
	template<size_t R, size_t C, Math::Operatable T>
	class Mat {
	public:
		/// Constructors.
		constexpr Mat() {
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					data[i][j] = 0;
		}

		constexpr Mat(T const& v) {
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					data[i][j] = v;
		}

		constexpr Mat(T v[R][C]) {
			for (size_t i = 0; i < R; i++) {
				for (size_t j = 0; j < C; j++)
					data[i][j] = v[i][j];
			}
		}

		constexpr Mat(Arguments<T[C]> const& v) {
			static_assert(v.size() == R, "Initializer list is ill-formed!");
			for (size_t i = 0; i < R; i++) {
				for (size_t j = 0; j < C; j++)
					data[i][j] = v[i][j];
			}
		}


		constexpr Mat(Vector2 const& vec) {
			static_assert(R == 2 && C == 1, "Matrix is not a valid representation of a 2D vector!");
			data[0][0] = vec.x;
			data[0][1] = vec.y;
		}

		constexpr Mat(Vector3 const& vec) {
			static_assert(R == 3 && C == 1, "Matrix is not a valid representation of a 3D vector!");
			data[0][0] = vec.x;
			data[0][1] = vec.y;
			data[0][2] = vec.z;
		}

		constexpr Mat(Vector4 const& vec) {
			static_assert(R == 4 && C == 1, "Matrix is not a valid representation of a 4D vector!");
			data[0][0] = vec.x;
			data[0][1] = vec.y;
			data[0][2] = vec.z;
			data[0][3] = vec.w;
		}

		constexpr Mat(Mat<R, C, T> const& other) {
			for (size_t i = 0; i < R; i++) {
				for (size_t j = 0; j < C; j++)
					data[i][j] = other.data[i][j];
			}
		}

		/// Gets the transposed matrix.
		constexpr Mat<C, R, T> transposed() const {
			Mat<C, R, T> res;
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					res[j][i] = data[i][j];
			return res;
		}

		constexpr Mat<R, C, T> inverted() const {
			static_assert(C == R, "Matrix is not a square matrix!");
			Mat<R, C, T> res;
			T det = determinant();
			if (det == T(0)) return Mat<R, C, T>(Math::infinity);
			res = cofactors().transposed() * (T(1) / det);
			return res;
		}

		constexpr Mat<R, C, T> cofactors() const {
			static_assert(C == R, "Matrix is not a square matrix!");
			Mat<R, C, T> res;
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					res[i][j] = cofactor(i, j);
			return res;
		}

		constexpr T determinant() const {
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

		constexpr Mat<R-1, C-1, T> truncated(size_t const& row, size_t const& col) const {
			static_assert(R > 1 && C > 1, "Cannot truncate a 1-dimensional matrix!");
			Mat<R-1, C-1, T> res;
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
		constexpr Mat<R-RF, C-CF, T> shrunkBy(size_t const& rowStart = 0, size_t const& colStart = 0) const {
			static_assert(R > 1 && C > 1, "Cannot shrink a 1-dimensional matrix!");
			static_assert(rowStart < (R-RF) && colStart < (C-CF), "Row/Column starts cannot be bigger than the shrunk matrix!");
			if (rowStart < (R-RF) && colStart < (C-CF))
				throw Error::InvalidValue(
					"Invalid row / column start!",
					__FILE__,
					toString(__LINE__),
					"shrunkBy",
					"Values cannot be bigger than the shrunk matrix's dimensions!"
				);
			Mat<R-RF, C-CF, T> res;
			for (size_t i = 0; i < R-RF; i++)
				for (size_t j = 0; j < C-CF; j++)
					res[i][j] = data[i+rowStart][j+colStart];
			return res;
		}

		template<size_t RF = 1, size_t CF = 1>
		constexpr Mat<R+RF, C+CF, T> expandedBy(size_t const& rowStart = 0, size_t const& colStart = 0) const {
			static_assert(rowStart < RF && colStart < CF, "Row/Column starts cannot be bigger than the expansion factor!");
			if (rowStart < (RF) && colStart < (CF))
				throw Error::InvalidValue(
					"Invalid row / column start!",
					__FILE__,
					toString(__LINE__),
					"shrunkBy",
					"Values cannot be bigger than the expansion factors!"
				);
			Mat<R+RF, C+CF, T> res;
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					res[i+rowStart][j+colStart] = data[i][j];
			return res;
		}

		/// Arithmetic operator overloading.

		constexpr Mat<R, C, T> operator+(T const& val) const {
			Mat<R, C, T> res;
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					res[j][i] = data[i][j] + val;
			return res;
		}

		constexpr Mat<R, C, T> operator+(Mat<C, R, T> const& mat) const {
			Mat<R, C, T> res;
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					res[j][i] = data[i][j] + mat.data[i][j];
			return res;
		}

		constexpr Mat<R, C, T> operator-(T const& val) const {
			Mat<R, C, T> res;
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					res[j][i] = data[i][j] - val;
			return res;
		}

		constexpr Mat<R, C, T> operator-(Mat<R, C, T> const& mat) const {
			Mat<R, C, T> res;
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					res[j][i] = data[i][j] - mat.data[i][j];
			return res;
		}

		constexpr Mat<R, C, T> operator*(T const& val) const {
			Mat<R, C, T> res;
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					res[j][i] = data[i][j] * val;
			return res;
		}

		template<size_t C2>
		constexpr Mat<R, C2, T> operator*(Mat<C, C2, T> mat) const {
			Mat<R, C2, T> res;
			mat = mat.transposed();
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < R; j++)
					res[i][j] = rdp(i, mat[j]);
			return res;
		}

		constexpr Mat<R, C, T> operator/(T const& val) const {
			Mat<R, C, T> res;
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					res[j][i] = data[i][j] / val;
			return res;
		}

		template<size_t C2>
		constexpr Mat<R, C2, T> operator/(Mat<C, C2, T> const& mat) const {
			return (*this) * mat.inverted();
		}

		/// Assignment operator overloading.

		constexpr Mat<R, C, T>& operator+=(T const& val) {
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					data[i][j] += val;
			return *this;
		}

		constexpr Mat<R, C, T>& operator+=(Mat<C, R, T> const& mat) {
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					data[i][j] += mat.data[i][j];
			return *this;
		}

		constexpr Mat<R, C, T>& operator-=(T const& val) {
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					data[i][j] -= val;
			return *this;
		}

		constexpr Mat<R, C, T>& operator-=(Mat<R, C, T> const& mat) {
			for (size_t i = 0; i < R; i++)
				for (size_t j = 0; j < C; j++)
					data[i][j] -= mat.data[i][j];
			return *this;
		}

		/// Other operator overloadings.
		constexpr Span<T, C> operator[](size_t const& idx) {
			return Span{data[idx]};
		}

		constexpr size_t rowCount() const {
			return R;
		}

		constexpr size_t columnCount() const {
			return C;
		}

		constexpr Vector2 toVector2() const {
			static_assert(R == 2, "Matrix is not a valid representation of a 2D vector!");
			return Vector2(data[0][C-1], data[1][C-1]);
		}

		constexpr Vector3 toVector3() const {
			static_assert(R == 3, "Matrix is not a valid representation of a 3D vector!");
			return Vector2(data[0][C-1], data[1][C-1], data[2][C-1]);
		}

		constexpr Vector4 toVector4() const {
			static_assert(R == 4, "Matrix is not a valid representation of a 4D vector!");
			return Vector2(data[0][C-1], data[1][C-1], data[2][C-1], data[3][C-1]);
		}

		/// Gets the dot product of a given row by another given row.
		template<size_t S>
		constexpr T rdp(size_t const& row, T other[S]) const {
			T res = 0;
			for (size_t i = 0; i < S; i++)
				res += other[i] * data[row][i];
			return res;
		}

		constexpr T cofactor(size_t row, size_t col) const {
			return ((((row + col) % 2) == 0) ? T(+1) : T(-1)) * truncated(row, col).determinant();
		}

	private:

		/// The matrix's columns;
		T data[R][C];
	};

	typedef Mat<2, 1, float> Matrix2x1;
	typedef Mat<3, 1, float> Matrix3x1;
	typedef Mat<4, 1, float> Matrix4x1;

	typedef Mat<2, 2, float> Matrix2x2;
	typedef Mat<3, 2, float> Matrix3x2;
	typedef Mat<4, 2, float> Matrix4x2;

	typedef Mat<2, 3, float> Matrix2x3;
	typedef Mat<3, 3, float> Matrix3x3;
	typedef Mat<4, 3, float> Matrix4x3;

	typedef Mat<2, 4, float> Matrix2x4;
	typedef Mat<3, 4, float> Matrix3x4;
	typedef Mat<4, 4, float> Matrix4x4;
}

#endif // FLOAT_MATRICES_234_H
