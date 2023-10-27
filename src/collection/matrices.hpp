#ifndef TYPE_MATRICES_H
#define TYPE_MATRICES_H

#include "algebra.hpp"
#include "vectorn.hpp"
#include "helper.hpp"

namespace MatType {
	template <size_t A, size_t B>
	concept EqualSize = (A == B);

	template <typename T1, typename T2>
	concept Compatitble = Math::Operatable<T2> && Type::Convertible<T2, T1>;

	template <size_t R, size_t C>
	concept ValidTransform = EqualSize<R, C> && (R == 4);
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
	constexpr Matrix() {}

	constexpr Matrix(T const& v) {
		size_t const start = Math::min(C, R);
		for (size_t i = 0; i < start; i++)
			data[start-1-i][start-1-i] = v;
	}

	constexpr Matrix(const T(&v)[R][C]) {
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] = v[j][i];
	}

	constexpr Matrix(const T(&v)[R*C]) {
		T hack[R][C];
		for (size_t i = 0; i < R*C; i++)
			((T*)hack)[i] = v[i];
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] = hack[j][i];
	}

	template<MatType::Compatitble<T> T2>
	constexpr Matrix(const T2(&v)[R][C]) {
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] = T(v[j][i]);
	}

	template<MatType::Compatitble<T> T2>
	constexpr Matrix(const T2(&v)[R*C]) {
		T2 hack[R][C];
		for (size_t i = 0; i < R*C; i++)
			((T2*)hack)[i] = v[i];
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] = hack[j][i];
	}

	constexpr Matrix(Vector2 const& vec) {
		static_assert(R >= 2 && C >= 1, "Matrix is not a valid representation of a 2D vector!");
		data[C-1][R-1] = 1;
		data[C-1][0] = vec.x;
		data[C-1][1] = vec.y;
	}

	constexpr Matrix(Vector3 const& vec) {
		static_assert(R >= 3 && C >= 1, "Matrix is not a valid representation of a 3D vector!");
		data[C-1][R-1] = 1;
		data[C-1][0] = vec.x;
		data[C-1][1] = vec.y;
		data[C-1][2] = vec.z;
	}

	constexpr Matrix(Vector4 const& vec) {
		static_assert(R >= 4 && C >= 1, "Matrix is not a valid representation of a 4D vector!");
		data[C-1][R-1] = 1;
		data[C-1][0] = vec.x;
		data[C-1][1] = vec.y;
		data[C-1][2] = vec.z;
		data[C-1][3] = vec.w;
	}

	constexpr Matrix(VecMath::Transform3D const& trans) requires MatType::ValidTransform<R, C> {
		compose(trans);
	}

	constexpr Matrix(
		Vector3 const& pos,
		Vector3 const& rot,
		Vector3 const& scale
	) requires MatType::ValidTransform<R, C> {
		compose(pos, rot, scale);
	}

	constexpr Matrix(
		VecMath::Transform3D const& trans,
		Vector4 const& perspective,
		Vector3 const& skew
	) requires MatType::ValidTransform<R, C> {
		compose(trans, perspective, skew);
	}

	constexpr Matrix(
		Vector3 const& pos,
		Vector3 const& rot,
		Vector3 const& scale,
		Vector4 const& perspective,
		Vector3 const& skew
	) requires MatType::ValidTransform<R, C> {
		compose(pos, rot, scale, perspective, skew);
	}

	constexpr Matrix(Matrix<R, C, T> const& other) {
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] = other.data[i][j];
	}

	template <typename T2>
	constexpr Matrix(Matrix<R, C, T2> const& other) {
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] = T(other.data[i][j]);
	}

	constexpr ~Matrix() {}

	constexpr Matrix<R, C, T>& fill(T const& v) {
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] = v;
		return *this;
	}

	/// Gets the transposed matrix.
	constexpr Matrix<C, R, T> transposed() const {
		Matrix<C, R, T> res;
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				res[j][i] = data[i][j];
		return res;
	}

	constexpr Matrix<C, R, T>& transpose() const requires MatType::EqualSize<R, C> {
		(*this) = transposed();
		return *this;
	}

	constexpr static Matrix<R, C, T> identity() requires MatType::EqualSize<R, C> {
		static_assert(C == R, "Matrix is not a square matrix!");
		return Matrix<R, C, T>(1);
	}

	constexpr static Matrix<R, C, T> prototype() requires MatType::EqualSize<R, C> {
		static_assert(C == R, "Matrix is not a square matrix!");
		Matrix<R, C, T> res(0);
		res.data[R-1][C-1] = 1;
		return res;
	}

	constexpr static Matrix<R, C, T> mirror() requires MatType::EqualSize<R, C> {
		static_assert(C == R, "Matrix is not a square matrix!");
		Matrix<R, C, T> res(0);
		for(size_t i = 0; i < R; i++)
			res.data[(R-1)-i][i] = 1;
		return res;
	}

	// https://github.com/g-truc/glm/blob/master/glm/gtx/euler_angles.inl
	constexpr static Matrix<4, 4, T> fromEulerXYZ(Vector3 const& angle) {
		// Get sines and cosines
		T c1 = cos(-angle.x);
        T c2 = cos(-angle.y);
        T c3 = cos(-angle.z);
        T s1 = sin(-angle.x);
        T s2 = sin(-angle.y);
        T s3 = sin(-angle.z);
		// Formulate matrix
        Matrix<4, 4, T> result;
        result[0][0] = c2 * c3;
        result[0][1] =-c1 * s3 + s1 * s2 * c3;
        result[0][2] = s1 * s3 + c1 * s2 * c3;
        result[0][3] = T(0);
        result[1][0] = c2 * s3;
        result[1][1] = c1 * c3 + s1 * s2 * s3;
        result[1][2] =-s1 * c3 + c1 * s2 * s3;
        result[1][3] = T(0);
        result[2][0] =-s2;
        result[2][1] = s1 * c2;
        result[2][2] = c1 * c2;
        result[2][3] = T(0);
        result[3][0] = T(0);
        result[3][1] = T(0);
        result[3][2] = T(0);
        result[3][3] = T(1);
        // Return result
        return result;
	}

	// https://github.com/g-truc/glm/blob/master/glm/gtx/euler_angles.inl
	constexpr static Matrix<4, 4, T> fromEulerYXZ(Vector3 const& angle) {
		// Get sines and cosines
		T tmp_ch = cos(-angle.y);
		T tmp_sh = sin(-angle.y);
		T tmp_cp = cos(-angle.x);
		T tmp_sp = sin(-angle.x);
		T tmp_cb = cos(-angle.z);
		T tmp_sb = sin(-angle.z);
		// Compute matrix
		Matrix<4, 4, T> result;
		result[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
		result[0][1] = tmp_sb * tmp_cp;
		result[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
		result[0][3] = T(0);
		result[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
		result[1][1] = tmp_cb * tmp_cp;
		result[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
		result[1][3] = T(0);
		result[2][0] = tmp_sh * tmp_cp;
		result[2][1] = -tmp_sp;
		result[2][2] = tmp_ch * tmp_cp;
		result[2][3] = T(0);
		result[3][0] = T(0);
		result[3][1] = T(0);
		result[3][2] = T(0);
		result[3][3] = T(1);
		// Return result
		return result;
	}

	constexpr static Matrix<4, 4, T> fromTranslation(Vector3 const& vec) {
		return Matrix(1).translated(vec);
	}

	constexpr static Matrix<4, 4, T> fromScale(Vector3 const& vec) {
		return Matrix(1).scaled(vec);
	}

	constexpr Matrix<R, C, T> inverted() const requires MatType::EqualSize<R, C> {
		static_assert(C == R, "Matrix is not a square matrix!");
		T det = determinant();
		if (det == T(0)) return Matrix(1);
		Matrix<R, C, T> res = cofactors().transposed() * (T(1) / det);
		return res;
	}

	constexpr Matrix<R, C, T>& invert() const requires MatType::EqualSize<R, C> {
		static_assert(C == R, "Matrix is not a square matrix!");
		static_assert(determinant() != T(0), "Determinant cannot be zero!");
		T det = determinant();
		if (det == T(0)) (*this) = Matrix(1);
		(*this) = cofactors().transposed() * (T(1) / det);
		return *this;
	}

	constexpr Matrix<4, 4, T> translated(Vector3 const& vec) const requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return Matrix<4, 4, T>(data).translate(vec);
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
	constexpr Matrix<4, 4, T>& translate(Vector3 const& vec) requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		Vector4 calc =
			(Vector4(data[0]) * vec[0])
		+	(Vector4(data[1]) * vec[1])
		+	(Vector4(data[2]) * vec[2])
		+	Vector4(data[3])
		;
		data[3][0] = calc[0];
		data[3][1] = calc[1];
		data[3][2] = calc[2];
		data[3][3] = calc[3];
		return (*this);
	}

	template<auto EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<4, 4, T> rotated(Vector3 const& vec) const requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return (*this) * EULER_FUNC(vec);
	}

	template<auto EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<4, 4, T>& rotate(Vector3 const& vec) requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		(*this) *= EULER_FUNC(vec);
		return (*this);
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
	constexpr Matrix<4, 4, T> scaled(Vector3 const& vec) const requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		Vector4 result[4];
		result[0] = Vector4(data[0]) * vec[0];
		result[1] = Vector4(data[1]) * vec[1];
		result[2] = Vector4(data[2]) * vec[2];
		result[3] = Vector4(data[3]);
		return Matrix<4, 4, T>({
			result[0][0], result[1][0], result[2][0], result[3][0],
			result[0][1], result[1][1], result[2][1], result[3][1],
			result[0][2], result[1][2], result[2][2], result[3][2],
			result[0][3], result[1][3], result[2][3], result[3][3]
		});
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
	constexpr Matrix<4, 4, T>& scale(Vector3 const& vec) requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		*this = scaled(vec);
		return (*this);
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
		+	(data[1][0] * data[2][1] * data[0][2])
		+	(data[2][0] * data[0][1] * data[1][2])
		-	(data[2][0] * data[1][1] * data[0][2])
		-	(data[0][0] * data[2][1] * data[1][2])
		-	(data[1][0] * data[0][1] * data[2][2])
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
		for (size_t i = 0; i < C; i++) {
			ro = 0;
			if (i == col) {co--; continue;}
			for (size_t j = 0; j < R; j++) {
				if (j == row) {ro--; continue;}
				res[i+co][j+ro] = data[i][j];
			}
		}
		return res;
	}

	template<size_t RF = 1, size_t CF = 1>
	constexpr Matrix<R-RF, C-CF, T> shrunkBy(size_t const& rowStart = 0, size_t const& colStart = 0) const {
		static_assert(R > 1 && C > 1, "Cannot shrink a 1-dimensional matrix!");
		static_assert(RF > R && CF > C, "Shrinking factor(s) are bigger than the matrix!");
		static_assert(rowStart < (R-RF) && colStart < (C-CF), "Row/Column starts cannot be bigger than the shrunk matrix!");
		if (rowStart < (R-RF) && colStart < (C-CF)) Matrix<R-RF, C-CF, T>(0);
		Matrix<R-RF, C-CF, T> res;
		for (size_t i = 0; i < C-CF; i++)
			for (size_t j = 0; j < R-RF; j++)
				res[i][j] = data[i+colStart][j+rowStart];
		return res;
	}

	template<size_t RF = 1, size_t CF = 1>
	constexpr Matrix<R+RF, C+CF, T> expandedBy(size_t const& rowStart = 0, size_t const& colStart = 0) const {
		static_assert(rowStart < RF && colStart < CF, "Row/Column starts cannot be bigger than the expansion factor!");
		if (rowStart < (RF) && colStart < (CF)) return Matrix<R+RF, C+CF, T>(0);
		Matrix<R+RF, C+CF, T> res;
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				res[i+colStart][j+rowStart] = data[i][j];
		return res;
	}

	/// Arithmetic operator overloading.

	constexpr Matrix<R, C, T> operator+(T const& val) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				res[j][i] = data[i][j] + val;
		return res;
	}

	constexpr Matrix<R, C, T> operator+(Matrix<R, C, T> const& mat) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				res[j][i] = data[i][j] + mat.data[i][j];
		return res;
	}

	constexpr Matrix<R, C, T> operator+(Vector2 const& vec) const {
		static_assert(R == 2 && C == 1, "Matrix size is invalid!");
		return (*this) + Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T> operator+(Vector3 const& vec) const {
		static_assert(R == 3 && C == 1, "Matrix size is invalid!");
		return (*this) + Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T> operator+(Vector4 const& vec) const {
		static_assert(R == 4 && C == 1, "Matrix size is invalid!");
		return (*this) + Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T> operator-(T const& val) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				res[j][i] = data[i][j] - val;
		return res;
	}

	constexpr Matrix<R, C, T> operator-(Matrix<R, C, T> const& mat) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				res[j][i] = data[i][j] - mat.data[i][j];
		return res;
	}

	constexpr Matrix<R, C, T> operator-(Vector2 const& vec) const {
		static_assert(R == 2 && C == 1, "Matrix size is invalid!");
		return (*this) - Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T> operator-(Vector3 const& vec) const {
		static_assert(R == 3 && C == 1, "Matrix size is invalid!");
		return (*this) - Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T> operator-(Vector4 const& vec) const {
		static_assert(R == 4 && C == 1, "Matrix size is invalid!");
		return (*this) - Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T> operator*(T const& val) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				res[j][i] = data[i][j] * val;
		return res;
	}

	template<size_t C2>
	constexpr Matrix<R, C2, T> operator*(Matrix<C, C2, T> const& mat) const {
		Matrix<R, C2, T> res;
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C2; j++) {
				res.data[j][i] = 0;
				for (size_t k = 0; k < C; k++)
					res.data[j][i] += data[k][i] * mat.data[j][k];
			}
		return res;
	}

	constexpr Matrix<R, 1, T> operator*(Vector2 const& vec) const {
		static_assert(R == 2, "Matrix row count is invalid!");
		return (*this) * Matrix<R, 1, float>(vec);
	}

	constexpr Matrix<R, 1, T> operator*(Vector3 const& vec) const {
		static_assert(R == 3, "Matrix row count is invalid!");
		return (*this) * Matrix<R, 1, float>(vec);
	}

	constexpr Matrix<R, 1, T> operator*(Vector4 const& vec) const {
		static_assert(R == 4, "Matrix row count is invalid!");
		return (*this) * Matrix<R, 1, float>(vec);
	}

	constexpr Matrix<R, C, T> operator*(VecMath::Transform3D const& trans) const requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return (*this) * Matrix<R, C, float>(trans);
	}

	constexpr Matrix<R, C, T> operator/(T const& val) const {
		Matrix<R, C, T> res;
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				res[j][i] = data[i][j] / val;
		return res;
	}

	template<size_t C2>
	constexpr Matrix<R, C2, T> operator/(Matrix<C, C2, T> const& mat) const {
		return (*this) * mat.inverted();
	}

	/// Assignment operator overloading.

	constexpr Matrix<R, C, T>& operator=(const T(&v)[R][C]) {
		for (size_t i = 0; i < C; i++) {
			for (size_t j = 0; j < R; j++)
				data[i][j] = T(v[i][j]);
		}
	}

	constexpr Matrix<R, C, T>& operator=(const T(&v)[R*C]) {
		for (size_t i = 0; i < R*C; i++)
			((float*)data)[i] = T(v[i]);
	}

	constexpr Matrix<R, C, T>& operator+=(T const& val) {
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] += val;
		return *this;
	}

	constexpr Matrix<R, C, T>& operator+=(Matrix<R, C, T> const& mat) {
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] += mat.data[i][j];
		return *this;
	}

	constexpr Matrix<R, C, T> operator+=(Vector2 const& vec) {
		static_assert(R == 2 && C == 1, "Matrix size is invalid!");
		return (*this) += Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T> operator+=(Vector3 const& vec) {
		static_assert(R == 3 && C == 1, "Matrix size is invalid!");
		return (*this) += Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T> operator+=(Vector4 const& vec) {
		static_assert(R == 4 && C == 1, "Matrix size is invalid!");
		return (*this) += Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T>& operator-=(T const& val) {
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] -= val;
		return *this;
	}

	constexpr Matrix<R, C, T>& operator-=(Matrix<R, C, T> const& mat) {
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] -= mat.data[i][j];
		return *this;
	}

	constexpr Matrix<R, C, T> operator-=(Vector2 const& vec) {
		static_assert(R == 2 && C == 1, "Matrix size is invalid!");
		return (*this) -= Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T> operator-=(Vector3 const& vec) {
		static_assert(R == 3 && C == 1, "Matrix size is invalid!");
		return (*this) -= Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T> operator-=(Vector4 const& vec) {
		static_assert(R == 4 && C == 1, "Matrix size is invalid!");
		return (*this) -= Matrix<R, C, float>(vec);
	}

	constexpr Matrix<R, C, T>& operator*=(T const& val) {
		for (size_t i = 0; i < C; i++)
			for (size_t j = 0; j < R; j++)
				data[i][j] *= val;
		return *this;
	}

	constexpr Matrix<R, C, T>& operator*=(Matrix<R, C, T> const& mat) requires MatType::EqualSize<R, C> {
		(*this) = (*this) * mat;
		return *this;
	}

	constexpr Matrix<R, C, T> operator*=(VecMath::Transform3D const& trans) const requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return (*this) *= Matrix<R, C, float>(trans);
	}

	constexpr Matrix<R, C, T>& operator/=(T const& val) {
		for (size_t i = 0; i < R; i++)
			for (size_t j = 0; j < C; j++)
				data[i][j] /= val;
		return *this;
	}

	template<size_t C2>
	constexpr Matrix<R, C, T>& operator/=(Matrix<R, C, T> const& mat) requires MatType::EqualSize<R, C> {
		(*this) = (*this) / mat;
		return *this;
	}

	/// Other operator overloadings.

	constexpr Span<T, R> operator[](size_t const& idx) {return Span{data[idx]};}

	constexpr Span<const T, R> operator[](size_t const& idx) const {return Span{data[idx]};}

	constexpr float* operator*() {return data;};

	template <MatType::Compatitble<T> T2>
	constexpr operator Matrix<R, C, T2>() const {return Matrix<R, C, T2>(data);}

	constexpr operator Vector2() const {return toVector2();}
	constexpr operator Vector3() const {return toVector3();}
	constexpr operator Vector4() const {return toVector4();}

	constexpr explicit operator const T*() const	{return (const T*)data;}
	constexpr explicit operator T*()				{return (T*)data;}

	/// Size accessors.

	constexpr static size_t rowCount() {return R;}

	constexpr static size_t columnCount() {return C;}

	/// Converters.

	constexpr Vector2 toVector2() const {
		static_assert(R >= 2, "Matrix is not a valid representation of a 2D vector!");
		if constexpr(R == 4) return Vector2(data[C-1][0], data[C-1][1]) / data[C-1][3];
		else return Vector2(data[C-1][0], data[C-1][1]);
	}

	constexpr Vector3 toVector3() const {
		static_assert(R >= 3, "Matrix is not a valid representation of a 3D vector!");
		if constexpr(R == 4) return Vector3(data[C-1][0], data[C-1][1], data[C-1][2]) / data[C-1][3];
		else return Vector3(data[C-1][0], data[C-1][1], data[C-1][2]);
	}

	constexpr Vector4 toVector4() const {
		static_assert(R >= 4, "Matrix is not a valid representation of a 4D vector!");
		return Vector4(data[C-1][0], data[C-1][1], data[C-1][2], data[C-1][3]);
	}

	template<auto EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<R, C, T>& transformed(
		Vector3 const& position,
		Vector3 const& rotation,
		Vector3 const& scale
	) const requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		// Transform
		return translated(position).template rotated<EULER_FUNC>(rotation).scaled(scale);
	}

	template<auto EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<R, C, T>& transformed(VecMath::Transform3D const& trans) const requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return transformed<EULER_FUNC>(trans.position, trans.rotation, trans.scale);
	}

	template<auto EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<R, C, T>& transform(
		Vector3 const& position,
		Vector3 const& rotation,
		Vector3 const& scale
	) requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		// Transform
		return translate(position).template rotate<EULER_FUNC>(rotation).scale(scale);
	}

	template<auto EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<R, C, T>& transform(VecMath::Transform3D const& trans) requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return transform<EULER_FUNC>(trans.position, trans.rotation, trans.scale);
	}

	template<auto EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<R, C, T>& compose(
		Vector3 const& position,
		Vector3 const& rotation,
		Vector3 const& scale
	) requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		// Fill & Transform
		(*this) = Matrix(1);
		return transform<EULER_FUNC>(position, rotation, scale);
	}

	template<auto EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<R, C, T>& compose(VecMath::Transform3D const& trans) requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return compose<EULER_FUNC>(trans.position, trans.rotation, trans.scale);
	}

	// https://github.com/g-truc/glm/blob/master/glm/gtx/matrix_decompose.inl
	template<auto EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<R, C, T>& compose(
		Vector3 const& position,
		Vector3 const& rotation,
		Vector3 const& scale,
		Vector4 const& perspective,
		Vector3 const& skew
	) requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		Matrix<4, 4, T> result(Matrix<4, 4, T>(1));
		// Apply perspective
		result.data[3][0] = perspective.x;
		result.data[3][1] = perspective.y;
		result.data[3][2] = perspective.z;
		result.data[3][3] = perspective.w;
		// Translate & rotate
		result.translate(position).template rotate<EULER_FUNC>(rotation);
		Matrix<4, 4, T> tmp = Matrix<4, 4, T>(1);
		// Skew
		if (skew.x) {
			tmp = 1;
			tmp[2][1] = skew.x;
			result *= tmp;
		}
		if (skew.y) {
			tmp = 1;
			tmp[2][0] = skew.y;
			result *= tmp;
		}
		if (skew.z) {
			tmp = 1;
			tmp[1][0] = skew.z;
			result *= tmp;
		}
		// Scale
		result.scale(scale);
		// Apply result
		(*this) = result;
		return *this;
	}

	// https://github.com/g-truc/glm/blob/master/glm/gtx/matrix_decompose.inl
	template<auto EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<R, C, T>& compose(
		VecMath::Transform3D const&	trans,
		Vector4 const&				perspective,
		Vector3 const&				skew
	) requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return compose<EULER_FUNC>(trans.position, trans.rotation, trans.scale, perspective, skew);
	}

	// https://opensource.apple.com/source/WebCore/WebCore-514/platform/graphics/transforms/TransformationMatrix.cpp
	[[gnu::unavailable("Not working as intended!")]]
	constexpr VecMath::Transform3D decompose(
			Vector4& perspective,
			Vector3& skew
		) const requires MatType::ValidTransform<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		VecMath::Transform3D result;
		// if identity value is 0, return
		if (data[3][3] == 0) return VecMath::Transform3D();
		// Copy & normalize matrix
		Matrix<R, C, T> local(data);
		local /= local.data[3][3];
		// Copy normalized
		Matrix<R, C, T> pMatrix(local);
		// Remove translation
		pMatrix.data[0][3] =
		pMatrix.data[1][3] =
		pMatrix.data[2][3] = T(0);
		pMatrix.data[3][3] = T(1);
		// If perspective's determinant is zero, return
		if (pMatrix.determinant() == T(0))
			return VecMath::Transform3D();
		// Isolate perspective
		if (
			local.data[0][3] != T(0)
		||	local.data[1][3] != T(0)
		||	local.data[2][3] != T(0)
		) {
			// Get right-hand side of the equation
			Vector4 rhs;
			for (size_t i = 0; i < 4; i++)
				rhs.data[i] = local.data[i][3];
			Matrix<R, C, T> tipMatrix = pMatrix.inverted().transposed();
			perspective = tipMatrix * Matrix<4, 1, float>(rhs);
			// Clear perspective
			local.data[0][3] =
			local.data[1][3] =
			local.data[2][3] = T(0);
			local.data[3][3] = T(1);
		} else {
			perspective = Vector4(0, 0, 0, 1);
		}
		// Isolate translation
		result.position = Vector3(
			local.data[3][0],
			local.data[3][1],
			local.data[3][2]
		);
		local.data[3][0] =
		local.data[3][1] =
		local.data[3][2] = T(0);
		// Isolate scale & shear
		Vector3 row[3];
		for (size_t i = 0; i < 3; i++)
			for (size_t j = 0; i < 3; i++)
				row[i][j] = local.data[i][j];
		// Compute X scale & normalize first row
		result.scale.x = row[0].length();
		row[0].normalize();
		// Compute XY shear factor & orthogonalize 2nd row to 1st
		skew.z = row[0].dotProd(row[1]);
		row[1] = (row[1] * 1.0) + (row[0] * -skew.z);
		// Compute Y scale & normalize second row
		result.scale.y = row[1].length();
		row[1].normalize();
		skew.z /= result.scale.y;
		// Compute remaining shears & orthogonalize 3rd row
		skew.y = row[0].dotProd(row[2]);
		row[2] = (row[2] * 1.0) + (row[0] * -skew.y);
		skew.x = row[1].dotProd(row[2]);
		row[2] = (row[2] * 1.0) + (row[1] * -skew.x);
		// Get Z scale & normalize third row
		result.scale.z = row[2].length();
		row[2].normalize();
		skew.y /= result.scale.z;
		skew.x /= result.scale.z;
		// Check for coordinate flip
		if (row[0].dotProd(row[1].crossProd(row[2])) < 0)
			for (size_t i = 0; i < 3; i++) {
				result.scale[i] *= T(-1);
				row[i] *= T(-1);
			}
		// Get euler angles
		result.rotation.y = asin(-row[0][2]);
		if (cos(result.rotation.x) != 0) {
			result.rotation.x = atan2(row[1][2], row[2][2]);
			result.rotation.z = atan2(row[0][1], row[0][0]);
		} else {
			result.rotation.x = atan2(-row[2][0], row[1][1]);
			result.rotation.z = 0;
		}
		// Return result
		return result;
	}

	[[gnu::unavailable("Not working as intended!")]]
	constexpr VecMath::Transform3D decompose() const requires MatType::ValidTransform<R, C> {
		Vector4 _p;
		Vector3 _s;
		return decompose(_p, _s);
	}

	constexpr T cofactor(size_t const& row, size_t const& col) const {
		return ((((row + col) % 2) == 0) ? T(+1) : T(-1)) * truncated(row, col).determinant();
	}

private:
	template<size_t R2, size_t C2, Math::Operatable T2> friend class Matrix;
	template<typename T2> friend Matrix<4, 4, T2> translate(Matrix<4, 4, T2>, Vector3 const&);
	/// The matrix's columns;
	T data[C][R] = {};
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
	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
	constexpr Matrix4x4 lookAt(Vector3 const& eye, Vector3 const& at, Vector3 const& up) {
		Vector3 const f((at - eye).normalized());
		Vector3 const s(f.crossProd(up).normalized());
		Vector3 const u(s.crossProd(f));
		Matrix4x4 result(Matrix4x4::identity());
		result[0][0] = +s.x;
		result[1][0] = +s.y;
		result[2][0] = +s.z;
		result[0][1] = +u.x;
		result[1][1] = +u.y;
		result[2][1] = +u.z;
		result[0][2] = -f.x;
		result[1][2] = -f.y;
		result[2][2] = -f.z;
		result[3][0] = -s.dotProd(eye);
		result[3][1] = -u.dotProd(eye);
		result[3][2] = +f.dotProd(eye);
		return result;
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
	template<Math::Operatable T>
	constexpr Matrix<4, 4, T> ortho(
		T const& left,
		T const& right,
		T const& bottom,
		T const& top,
		T const& zNear,
		T const& zFar
	) {
		Matrix<4, 4, T> result(Matrix<4, 4, T>::identity());
		result[0][0] = T(2) / (right - left);
		result[1][1] = T(2) / (top - bottom);
		result[2][2] = - T(2) / (zFar - zNear);
		result[3][0] = - (right + left) / (right - left);
		result[3][1] = - (top + bottom) / (top - bottom);
		result[3][2] = - (zFar + zNear) / (zFar - zNear);
		return result;
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
	template<Math::Operatable T>
	constexpr Matrix<4, 4, T> infiniteOrtho(
		T const& left,
		T const& right,
		T const& bottom,
		T const& top
	) {
		Matrix<4, 4, T> result(Matrix<4, 4, T>::identity());
		result[0][0] = T(2) / (right - left);
		result[1][1] = T(2) / (top - bottom);
		result[2][2] = - T(1);
		result[3][0] = - (right + left) / (right - left);
		result[3][1] = - (top + bottom) / (top - bottom);
		return result;
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
	template<Math::Operatable T>
	constexpr Matrix<4, 4, T> frustum(
		T const& left,
		T const& right,
		T const& bottom,
		T const& top,
		T const& zNear,
		T const& zFar
	) {
		Matrix<4, 4, T> result(T(0));
		result[0][0] = (T(2) * zNear) / (right - left);
		result[1][1] = (T(2) * zNear) / (top - bottom);
		result[2][0] = (right + left) / (right - left);
		result[2][1] = (top + bottom) / (top - bottom);
		result[2][2] = - (zFar + zNear) / (zFar - zNear);
		result[2][3] = T(-1);
		result[3][2] = - (T(2) * zFar * zNear) / (zFar - zNear);
		return result;
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
	template<Math::Operatable T>
	constexpr Matrix<4, 4, T> perspective(
		T const& fovy,
		T const& aspect,
		T const& zNear,
		T const& zFar
	) {
		Matrix<4, 4, T> result(T(0));
		T const tanHalfFovy = tan(fovy / T(2));
		result[0][0] = T(1) / (aspect * tanHalfFovy);
		result[1][1] = T(1) / (tanHalfFovy);
		result[2][2] = - (zFar + zNear) / (zFar - zNear);
		result[2][3] = - T(1);
		result[3][2] = - (T(2) * zFar * zNear) / (zFar - zNear);
		return result;
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
	template<Math::Operatable T>
	constexpr Matrix<4, 4, T> perspectiveFOV(
		T const& fov,
		T const& width,
		T const& height,
		T const& zNear,
		T const& zFar
	) {
		Matrix<4, 4, T> result(T(0));
		T const rad	= fov;
		T const h	= cos(T(0.5) * rad) / sin(T(0.5) * rad);
		T const w	= h * height / width;
		result[0][0] = w;
		result[1][1] = h;
		result[2][2] = - (zFar + zNear) / (zFar - zNear);
		result[2][3] = - T(1);
		result[3][2] = - (T(2) * zFar * zNear) / (zFar - zNear);
		return result;
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
	template<Math::Operatable T>
	constexpr Matrix<4, 4, T> infinitePerspective(
		T const& fovy,
		T const& aspect,
		T const& zNear
	) {
		T const range	= tan(fovy / T(2)) * zNear;
		T const left	= -range * aspect;
		T const right	= range * aspect;
		T const bottom	= -range;
		T const top		= range;
		Matrix<4, 4, T> result(T(0));
		result[0][0] = (T(2) * zNear) / (right - left);
		result[1][1] = (T(2) * zNear) / (top - bottom);
		result[2][2] = - T(1);
		result[2][3] = - T(1);
		result[3][2] = - T(2) * zNear;
		return result;
	}

	template<Math::Operatable T>
	constexpr Vector3 getEulerAnglesYXZ(Matrix<4, 4, T> const& mat) {
		float T1 = atan2(mat[2][0], mat[2][2]);
		float C2 = sqrt(mat[0][1]*mat[0][1] + mat[1][1]*mat[1][1]);
		float T2 = atan2(-mat[2][1], C2);
		float S1 = sin(T1);
		float C1 = cos(T1);
		float T3 = atan2(S1*mat[1][2] - C1*mat[1][0], C1*mat[0][0] - S1*mat[0][2]);
		return Vector3(T1, T2, T3);
	}

	template<Math::Operatable T>
	constexpr Vector3 getEulerAnglesXYZ(Matrix<4, 4, T> const& mat) {
		float T1 = atan2(mat[2][1], mat[2][2]);
		float C2 = sqrt(mat[0][0]*mat[0][0] + mat[1][0]*mat[1][0]);
		float T2 = atan2(-mat[2][0], C2);
		float S1 = sin(T1);
		float C1 = cos(T1);
		float T3 = atan2(S1*mat[0][2] - C1*mat[0][1], C1*mat[1][1] - S1*mat[1][2]);
		return Vector3(T1, T2, T3);
	}
}

#endif // TYPE_MATRICES_H
