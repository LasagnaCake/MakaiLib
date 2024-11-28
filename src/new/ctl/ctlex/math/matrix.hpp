#ifndef CTL_EX_MATH_MATRIX_H
#define CTL_EX_MATH_MATRIX_H

#include "vector.hpp"
#include "../../ctl/exnamespace.hpp"
#include "../../ctl/math/core.hpp"

CTL_EX_NAMESPACE_BEGIN

/// @brief Math extensions.
namespace Math {
	template<usize R, usize C, CTL::Type::Math::Operatable T>
	class Matrix;
}

/// @brief Matrix-specific type constraints.
namespace Type::Ex::Math::Matrix {
	/// @brief Type must be a `Matrix`.
	template <class T>
	concept Matrix = requires {
		T::ROWS;
		T::COLUMNS;
		typename T::DataType;
	}
	&&	CTL::Type::Equal<T, CTL::Ex::Math::Matrix<T::ROWS, T::COLUMNS, typename T::DataType>>
	;

	/// @brief Type must be operatable, and must be convertible to `TCompat`.
	template <class T, class TCompat>
	concept Compatitble = CTL::Type::Math::Operatable<TCompat> && CTL::Type::Convertible<T, TCompat>;

	/// @brief Matrix must be a valid `Transform3D`.
	template <usize R, usize C>
	concept ValidTransform3D = (R == 4) && (R == C);

	/// @brief Matrix must be a valid `Transform2D`.
	template <usize R, usize C>
	concept ValidTransform2D = (R == 3) && (R == C);
}

/// @brief Math extensions.
namespace Math {

/// @brief Mathematical matrix. Column-major.
/// @tparam R Row count.
/// @tparam C Column count.
/// @tparam T Matrix element type.
template<usize R, usize C, CTL::Type::Math::Operatable TData>
class Matrix {
public:
	/// @brief Row count.
	constexpr static usize ROWS		= R;
	/// @brief Column count.
	constexpr static usize COLUMNS	= C;

	/// @brief Matrix element type.
	using DataType	= TData;

	template<class T = DataType>
	using SingleElementType	= Decay::AsType<TData[1]>;
	template<class T = DataType>
	using MatrixType		= Decay::AsType<TData[R][C]>;
	template<class T = DataType>
	using ArrayType			= Decay::AsType<TData[R*C]>;
	template<class T = DataType>
	using SingleColumnType	= Decay::AsType<TData[C]>;

	using SelfType	= Matrix<ROWS, COLUMNS, DataType>;

	static_assert(R > 0, "Matrix row size must not be zero!");
	static_assert(C > 0, "Matrix column size must not be zero!");

	/// @brief Empty constructor.
	constexpr Matrix() {}

	constexpr Matrix(DataType const& v) {
		usize const start = ::CTL::Math::min(C, R);
		for (usize i = 0; i < start; i++)
			data[start-1-i][start-1-i] = v;
	}

	constexpr Matrix(SingleElementType<> const& v) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] = v[0];
	}

	constexpr Matrix(MatrixType<> const& v) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] = v[j][i];
	}

	constexpr Matrix(ArrayType<> const& v) {
		DataType hack[R][C];
		for (usize i = 0; i < R*C; i++)
			((DataType*)hack)[i] = v[i];
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] = hack[j][i];
	}

	constexpr Matrix(SingleColumnType<> const& v)
	requires (C > 1) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] = v[i];
	}

	template<Type::Ex::Math::Matrix::Compatitble<DataType> T2>
	constexpr Matrix(SingleElementType<T2> const& v) {
		DataType rv = DataType(v[0]);
		usize const start = ::CTL::Math::min(C, R);
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] = rv;
	}

	template<Type::Ex::Math::Matrix::Compatitble<DataType> T2>
	constexpr Matrix(MatrixType<T2> const& v) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] = DataType(v[j][i]);
	}

	template<Type::Ex::Math::Matrix::Compatitble<DataType> T2>
	constexpr Matrix(ArrayType<T2> const& v) {
		T2 hack[R][C];
		for (usize i = 0; i < R*C; i++)
			((T2*)hack)[i] = v[i];
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] = DataType(hack[j][i]);
	}

	template<Type::Ex::Math::Matrix::Compatitble<DataType> T2>
	constexpr Matrix(SingleColumnType<T2> const& v)
	requires (C > 1) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] = DataType(v[i]);
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

	constexpr Matrix(Transform3D const& trans)
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		compose(trans);
	}

	constexpr Matrix(Transform2D const& trans)
	requires Type::Ex::Math::Matrix::ValidTransform2D<R, C> {
		*this = identity().transform(trans);
	}

	constexpr Matrix(
		Vector3 const& pos,
		Vector3 const& rot,
		Vector3 const& scale
	) requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		compose(pos, rot, scale);
	}

	constexpr Matrix(
		Transform3D const& trans,
		Vector4 const& perspective,
		Vector3 const& skew
	) requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		compose(trans, perspective, skew);
	}

	constexpr Matrix(
		Vector3 const& pos,
		Vector3 const& rot,
		Vector3 const& scale,
		Vector4 const& perspective,
		Vector3 const& skew
	) requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		compose(pos, rot, scale, perspective, skew);
	}

	constexpr Matrix(SelfType const& other) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] = other.data[i][j];
	}

	template <typename T2>
	constexpr Matrix(Matrix<R, C, T2> const& other) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] = DataType(other.data[i][j]);
	}

	constexpr ~Matrix() {}

	constexpr SelfType& fill(DataType const& v) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] = v;
		return *this;
	}

	/// Gets the transposed matrix.
	constexpr Matrix<C, R, DataType> transposed() const {
		Matrix<C, R, DataType> res;
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				res[j][i] = data[i][j];
		return res;
	}

	constexpr SelfType& transpose() requires (R == C) {
		(*this) = transposed();
		return *this;
	}

	constexpr static SelfType identity() requires (R == C) {
		static_assert(C == R, "Matrix is not a square matrix!");
		return SelfType(1);
	}

	constexpr static SelfType prototype() requires (R == C) {
		static_assert(C == R, "Matrix is not a square matrix!");
		SelfType res(0);
		res.data[R-1][C-1] = 1;
		return res;
	}

	constexpr static SelfType mirror() requires (R == C) {
		static_assert(C == R, "Matrix is not a square matrix!");
		SelfType res(0);
		for(usize i = 0; i < R; i++)
			res.data[(R-1)-i][i] = 1;
		return res;
	}

	// https://github.com/g-truc/glm/blob/master/glm/gtx/euler_angles.inl
	constexpr static Matrix<4, 4, DataType> fromEulerXYZ(Vector3 const& angle) {
		// Get sines and cosines
		DataType c1 = cos(-angle.x);
		DataType c2 = cos(-angle.y);
		DataType c3 = cos(-angle.z);
		DataType s1 = sin(-angle.x);
		DataType s2 = sin(-angle.y);
		DataType s3 = sin(-angle.z);
		// Formulate matrix
		Matrix<4, 4, DataType> result;
		result[0][0] = c2 * c3;
		result[0][1] =-c1 * s3 + s1 * s2 * c3;
		result[0][2] = s1 * s3 + c1 * s2 * c3;
		result[0][3] = DataType(0);
		result[1][0] = c2 * s3;
		result[1][1] = c1 * c3 + s1 * s2 * s3;
		result[1][2] =-s1 * c3 + c1 * s2 * s3;
		result[1][3] = DataType(0);
		result[2][0] =-s2;
		result[2][1] = s1 * c2;
		result[2][2] = c1 * c2;
		result[2][3] = DataType(0);
		result[3][0] = DataType(0);
		result[3][1] = DataType(0);
		result[3][2] = DataType(0);
		result[3][3] = DataType(1);
		// Return result
		return result;
	}

	// https://github.com/g-truc/glm/blob/master/glm/gtx/euler_angles.inl
	constexpr static Matrix<4, 4, DataType> fromEulerYXZ(Vector3 const& angle) {
		// Get sines and cosines
		DataType tmp_ch = cos(-angle.y);
		DataType tmp_sh = sin(-angle.y);
		DataType tmp_cp = cos(-angle.x);
		DataType tmp_sp = sin(-angle.x);
		DataType tmp_cb = cos(-angle.z);
		DataType tmp_sb = sin(-angle.z);
		// Compute matrix
		Matrix<4, 4, DataType> result;
		result[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
		result[0][1] = tmp_sb * tmp_cp;
		result[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
		result[0][3] = DataType(0);
		result[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
		result[1][1] = tmp_cb * tmp_cp;
		result[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
		result[1][3] = DataType(0);
		result[2][0] = tmp_sh * tmp_cp;
		result[2][1] = -tmp_sp;
		result[2][2] = tmp_ch * tmp_cp;
		result[2][3] = DataType(0);
		result[3][0] = DataType(0);
		result[3][1] = DataType(0);
		result[3][2] = DataType(0);
		result[3][3] = DataType(1);
		// Return result
		return result;
	}

	// Euler function type
	typedef decltype(fromEulerXYZ)	EulerFunction;

	constexpr static Matrix<4, 4, DataType> fromTranslation(Vector3 const& vec) {
		return Matrix(1).translated(vec);
	}

	constexpr static Matrix<4, 4, DataType> fromScale(Vector3 const& vec) {
		return Matrix(1).scaled(vec);
	}

	constexpr SelfType inverted() const requires (R == C) {
		static_assert(C == R, "Matrix is not a square matrix!");
		DataType det = determinant();
		if (det == DataType(0)) return Matrix(1);
		SelfType res = cofactors().transposed() * (DataType(1) / det);
		return res;
	}

	constexpr SelfType& invert() requires (R == C) {
		static_assert(C == R, "Matrix is not a square matrix!");
		(*this) = inverted();
		return *this;
	}

	constexpr Matrix<4, 4, DataType> translated(Vector3 const& vec) const
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return Matrix<4, 4, DataType>(data).translate(vec);
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
	constexpr Matrix<4, 4, DataType>& translate(Vector3 const& vec)
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
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

	template<EulerFunction EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<4, 4, DataType> rotated(Vector3 const& vec) const
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return (*this) * EULER_FUNC(vec);
	}

	template<EulerFunction EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr Matrix<4, 4, DataType>& rotate(Vector3 const& vec)
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		(*this) *= EULER_FUNC(vec);
		return (*this);
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
	constexpr Matrix<4, 4, DataType> scaled(Vector3 const& vec) const
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		Vector4 result[4];
		result[0] = Vector4(data[0]) * vec[0];
		result[1] = Vector4(data[1]) * vec[1];
		result[2] = Vector4(data[2]) * vec[2];
		result[3] = Vector4(data[3]);
		return Matrix<4, 4, DataType>({
			result[0][0], result[1][0], result[2][0], result[3][0],
			result[0][1], result[1][1], result[2][1], result[3][1],
			result[0][2], result[1][2], result[2][2], result[3][2],
			result[0][3], result[1][3], result[2][3], result[3][3]
		});
	}

	// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
	constexpr Matrix<4, 4, DataType>& scale(Vector3 const& vec)
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		*this = scaled(vec);
		return (*this);
	}

	constexpr SelfType cofactors() const requires (R == C) {
		static_assert(C == R, "Matrix is not a square matrix!");
		SelfType res;
		for (usize i = 0; i < R; i++)
			for (usize j = 0; j < C; j++)
				res[i][j] = cofactor(i, j);
		return res;
	}

	constexpr DataType determinant() const requires (R == C) {
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
			DataType res = 0;
			for (usize i = 0; i < R; i++) {
				if (data[i][0] == 0) continue;
				res += data[i][0] * cofactor(i, 0);
			}
			return res;
		}
	}

	constexpr Matrix<R-1, C-1, DataType> truncated(usize const row, usize const col) const {
		static_assert(R > 1 && C > 1, "Cannot truncate a 1-dimensional matrix!");
		Matrix<R-1, C-1, DataType> res;
		int ro = 0, co = 0;
		for (usize i = 0; i < C; i++) {
			ro = 0;
			if (i == col) {co--; continue;}
			for (usize j = 0; j < R; j++) {
				if (j == row) {ro--; continue;}
				res[i+co][j+ro] = data[i][j];
			}
		}
		return res;
	}

	template<usize RF = 1, usize CF = 1>
	constexpr Matrix<R-RF, C-CF, DataType> shrunkBy(usize const rowStart = 0, usize const colStart = 0) const {
		static_assert(R > 1 && C > 1, "Cannot shrink a 1-dimensional matrix!");
		static_assert(RF > R && CF > C, "Shrinking factor(s) are bigger than the matrix!");
		static_assert(rowStart < (R-RF) && colStart < (C-CF), "Row/Column starts cannot be bigger than the shrunk matrix!");
		if (rowStart < (R-RF) && colStart < (C-CF)) Matrix<R-RF, C-CF, DataType>(0);
		Matrix<R-RF, C-CF, DataType> res;
		for (usize i = 0; i < C-CF; i++)
			for (usize j = 0; j < R-RF; j++)
				res[i][j] = data[i+colStart][j+rowStart];
		return res;
	}

	template<usize RF = 1, usize CF = 1>
	constexpr Matrix<R+RF, C+CF, DataType> expandedBy(usize const rowStart = 0, usize const colStart = 0) const {
		static_assert(rowStart < RF && colStart < CF, "Row/Column starts cannot be bigger than the expansion factor!");
		if (rowStart < (RF) && colStart < (CF)) return Matrix<R+RF, C+CF, DataType>(0);
		Matrix<R+RF, C+CF, DataType> res;
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				res[i+colStart][j+rowStart] = data[i][j];
		return res;
	}

	/// Arithmetic operator overloading.

	constexpr SelfType operator+() const {return *this;}

	constexpr SelfType operator+(DataType const& val) const {
		SelfType res;
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				res[j][i] = data[i][j] + val;
		return res;
	}

	constexpr SelfType operator+(SelfType const& mat) const {
		SelfType res;
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				res[j][i] = data[i][j] + mat.data[i][j];
		return res;
	}

	constexpr SelfType operator+(Vector2 const& vec) const {
		static_assert(R == 2 && C == 1, "Matrix size is invalid!");
		return (*this) + SelfType(vec);
	}

	constexpr SelfType operator+(Vector3 const& vec) const {
		static_assert(R == 3 && C == 1, "Matrix size is invalid!");
		return (*this) + SelfType(vec);
	}

	constexpr SelfType operator+(Vector4 const& vec) const {
		static_assert(R == 4 && C == 1, "Matrix size is invalid!");
		return (*this) + SelfType(vec);
	}

	constexpr SelfType operator-() const {
		SelfType res;
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				res[j][i] = -data[i][j];
		return res;
	}

	constexpr SelfType operator-(DataType const& val) const {
		SelfType res;
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				res[j][i] = data[i][j] - val;
		return res;
	}

	constexpr SelfType operator-(SelfType const& mat) const {
		SelfType res;
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				res[j][i] = data[i][j] - mat.data[i][j];
		return res;
	}

	constexpr SelfType operator-(Vector2 const& vec) const {
		static_assert(R == 2 && C == 1, "Matrix size is invalid!");
		return (*this) - SelfType(vec);
	}

	constexpr SelfType operator-(Vector3 const& vec) const {
		static_assert(R == 3 && C == 1, "Matrix size is invalid!");
		return (*this) - SelfType(vec);
	}

	constexpr SelfType operator-(Vector4 const& vec) const {
		static_assert(R == 4 && C == 1, "Matrix size is invalid!");
		return (*this) - SelfType(vec);
	}

	constexpr SelfType operator*(DataType const& val) const {
		SelfType res;
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				res[j][i] = data[i][j] * val;
		return res;
	}

	template<usize C2>
	constexpr Matrix<R, C2, DataType> operator*(Matrix<C, C2, DataType> const& mat) const {
		Matrix<R, C2, DataType> res;
		for (usize i = 0; i < R; i++)
			for (usize j = 0; j < C2; j++) {
				res.data[j][i] = 0;
				for (usize k = 0; k < C; k++)
					res.data[j][i] += data[k][i] * mat.data[j][k];
			}
		return res;
	}

	constexpr Matrix<R, 1, DataType> operator*(Vector2 const& vec) const {
		static_assert(R == 2, "Matrix row count is invalid!");
		return (*this) * Matrix<R, 1, DataType>(vec);
	}

	constexpr Matrix<R, 1, DataType> operator*(Vector3 const& vec) const {
		static_assert(R == 3, "Matrix row count is invalid!");
		return (*this) * Matrix<R, 1, DataType>(vec);
	}

	constexpr Matrix<R, 1, DataType> operator*(Vector4 const& vec) const {
		static_assert(R == 4, "Matrix row count is invalid!");
		return (*this) * Matrix<R, 1, DataType>(vec);
	}

	constexpr SelfType operator*(Transform3D const& trans) const
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C>  {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return (*this) * SelfType(trans);
	}

	constexpr SelfType operator/(DataType const& val) const {
		SelfType res;
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				res[j][i] = data[i][j] / val;
		return res;
	}

	template<usize C2>
	constexpr Matrix<R, C2, DataType> operator/(Matrix<C, C2, DataType> const& mat) const {
		return (*this) * mat.inverted();
	}

	/// Assignment operator overloading.

	constexpr SelfType& operator=(DataType const(& v)[R][C]) {
		for (usize i = 0; i < C; i++) {
			for (usize j = 0; j < R; j++)
				data[i][j] = DataType(v[i][j]);
		}
	}

	constexpr SelfType& operator=(DataType const(& v)[R*C]) {
		for (usize i = 0; i < R*C; i++)
			((DataType*)data)[i] = DataType(v[i]);
	}

	constexpr SelfType& operator+=(DataType const& val) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] += val;
		return *this;
	}

	constexpr SelfType& operator+=(SelfType const& mat) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] += mat.data[i][j];
		return *this;
	}

	constexpr SelfType operator+=(Vector2 const& vec) {
		static_assert(R == 2 && C == 1, "Matrix size is invalid!");
		return (*this) += SelfType(vec);
	}

	constexpr SelfType operator+=(Vector3 const& vec) {
		static_assert(R == 3 && C == 1, "Matrix size is invalid!");
		return (*this) += SelfType(vec);
	}

	constexpr SelfType operator+=(Vector4 const& vec) {
		static_assert(R == 4 && C == 1, "Matrix size is invalid!");
		return (*this) += SelfType(vec);
	}

	constexpr SelfType& operator-=(DataType const& val) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] -= val;
		return *this;
	}

	constexpr SelfType& operator-=(SelfType const& mat) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] -= mat.data[i][j];
		return *this;
	}

	constexpr SelfType operator-=(Vector2 const& vec) {
		static_assert(R == 2 && C == 1, "Matrix size is invalid!");
		return (*this) -= SelfType(vec);
	}

	constexpr SelfType operator-=(Vector3 const& vec) {
		static_assert(R == 3 && C == 1, "Matrix size is invalid!");
		return (*this) -= SelfType(vec);
	}

	constexpr SelfType operator-=(Vector4 const& vec) {
		static_assert(R == 4 && C == 1, "Matrix size is invalid!");
		return (*this) -= SelfType(vec);
	}

	constexpr SelfType& operator*=(DataType const& val) {
		for (usize i = 0; i < C; i++)
			for (usize j = 0; j < R; j++)
				data[i][j] *= val;
		return *this;
	}

	constexpr SelfType& operator*=(SelfType const& mat)
	requires (R == C) {
		(*this) = (*this) * mat;
		return *this;
	}

	constexpr SelfType operator*=(Transform3D const& trans)
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return (*this) *= SelfType(trans);
	}

	constexpr SelfType& operator/=(DataType const& val) {
		for (usize i = 0; i < R; i++)
			for (usize j = 0; j < C; j++)
				data[i][j] /= val;
		return *this;
	}

	template<usize C2>
	constexpr SelfType& operator/=(Matrix<R, C2, DataType> const& mat)
	requires (R == C) {
		(*this) = (*this) / mat;
		return *this;
	}

	/// Other operator overloadings.

	constexpr Span<DataType, R> operator[](usize const idx)				{return Span<DataType, R>(data[idx]);		}
	constexpr Span<const DataType, R> operator[](usize const idx) const	{return Span<const DataType, R>(data[idx]);	}

	constexpr DataType* begin()	{return &data[0][0];		}
	constexpr DataType* end()	{return &data[C-1][R-1];	}

	constexpr const DataType* begin() const	{return &data[0][0];		}
	constexpr const DataType* end() const	{return &data[C-1][R-1];	}

	template <Type::Ex::Math::Matrix::Compatitble<DataType> T2>
	constexpr operator Matrix<R, C, T2>() const {return Matrix<R, C, T2>(data);}

	constexpr operator Vector2() const {return toVector2();}
	constexpr operator Vector3() const {return toVector3();}
	constexpr operator Vector4() const {return toVector4();}

	constexpr explicit operator const DataType*() const	{return begin();	}
	constexpr explicit operator DataType*()				{return begin();	}

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

	template<EulerFunction EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr SelfType& transformed(
		Vector3 const& position,
		Vector3 const& rotation,
		Vector3 const& scale
	) const requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		// Transform
		return translated(position).template rotated<EULER_FUNC>(rotation).scaled(scale);
	}

	template<EulerFunction EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr SelfType& transformed(Transform3D const& trans)
	const requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return transformed<EULER_FUNC>(trans.position, trans.rotation, trans.scale);
	}

	template<EulerFunction EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr SelfType& transformed(Transform2D const& trans)
	const requires Type::Ex::Math::Matrix::ValidTransform2D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return Matrix(*this).transform(trans);
	}

	template<EulerFunction EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr SelfType& transform(
		Vector3 const& position,
		Vector3 const& rotation,
		Vector3 const& scale
	) requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		// Transform
		return translate(position).template rotate<EULER_FUNC>(rotation).scale(scale);
	}

	template<EulerFunction EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr SelfType& transform(Transform3D const& trans)
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return transform<EULER_FUNC>(trans.position, trans.rotation, trans.scale);
	}

	constexpr SelfType& transform(Transform2D const& trans)
	requires Type::Ex::Math::Matrix::ValidTransform2D<R, C> {
		static_assert(R == 3, "Matrix is not a valid representation of a 3D transform!");
		SelfType
			pos		= Matrix::identity(),
			rot		= Matrix::identity(),
			scale	= Matrix::identity()
		;
		// Scale
		scale[0][0] = trans.scale.x;
		scale[1][1] = trans.scale.y;
		// Position
		pos[2][0] = trans.position.x;
		pos[2][1] = trans.position.y;
		// Rotation
		rot[0][0] = cos(trans.rotation);
		rot[1][0] = -sin(trans.rotation);
		rot[0][1] = sin(trans.rotation);
		rot[1][1] = cos(trans.rotation);
		// Result
		(*this) = pos * rot * scale * (*this);
		return *this;
	}

	template<EulerFunction EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr SelfType& compose(
		Vector3 const& position,
		Vector3 const& rotation,
		Vector3 const& scale
	) requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		// Fill & Transform
		(*this) = Matrix(1);
		return transform<EULER_FUNC>(position, rotation, scale);
	}

	template<EulerFunction EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr SelfType& compose(Transform3D const& trans)
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return compose<EULER_FUNC>(trans.position, trans.rotation, trans.scale);
	}

	// https://github.com/g-truc/glm/blob/master/glm/gtx/matrix_decompose.inl
	template<EulerFunction EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr SelfType& compose(
		Vector3 const& position,
		Vector3 const& rotation,
		Vector3 const& scale,
		Vector4 const& perspective,
		Vector3 const& skew
	) requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		Matrix<4, 4, DataType> result(Matrix<4, 4, DataType>(1));
		// Apply perspective
		result.data[3][0] = perspective.x;
		result.data[3][1] = perspective.y;
		result.data[3][2] = perspective.z;
		result.data[3][3] = perspective.w;
		// Translate & rotate
		result.translate(position).template rotate<EULER_FUNC>(rotation);
		Matrix<4, 4, DataType> tmp = Matrix<4, 4, DataType>(1);
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

	template<EulerFunction EULER_FUNC = Matrix::fromEulerYXZ>
	constexpr SelfType& compose(
		Transform3D const&	trans,
		Vector4 const&				perspective,
		Vector3 const&				skew
	) requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		return compose<EULER_FUNC>(trans.position, trans.rotation, trans.scale, perspective, skew);
	}

	// https://opensource.apple.com/source/WebCore/WebCore-514/platform/graphics/transforms/TransformationMatrix.cpp
	[[gnu::unavailable("Not working as intended!")]]
	constexpr Transform3D decompose(
			Vector4& perspective,
			Vector3& skew
		) const requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		static_assert(R == 4, "Matrix is not a valid representation of a 3D transform!");
		Transform3D result;
		// if identity value is 0, return
		if (data[3][3] == 0) return Transform3D();
		// Copy & normalize matrix
		SelfType local(data);
		local /= local.data[3][3];
		// Copy normalized
		SelfType pMatrix(local);
		// Remove translation
		pMatrix.data[0][3] =
		pMatrix.data[1][3] =
		pMatrix.data[2][3] = DataType(0);
		pMatrix.data[3][3] = DataType(1);
		// If perspective's determinant is zero, return
		if (pMatrix.determinant() == DataType(0))
			return Transform3D();
		// Isolate perspective
		if (
			local.data[0][3] != DataType(0)
		||	local.data[1][3] != DataType(0)
		||	local.data[2][3] != DataType(0)
		) {
			// Get right-hand side of the equation
			Vector4 rhs;
			for (usize i = 0; i < 4; i++)
				rhs.data[i] = local.data[i][3];
			SelfType tipMatrix = pMatrix.inverted().transposed();
			perspective = tipMatrix * Matrix<4, 1, DataType>(rhs);
			// Clear perspective
			local.data[0][3] =
			local.data[1][3] =
			local.data[2][3] = DataType(0);
			local.data[3][3] = DataType(1);
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
		local.data[3][2] = DataType(0);
		// Isolate scale & shear
		Vector3 row[3];
		for (usize i = 0; i < 3; i++)
			for (usize j = 0; i < 3; i++)
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
		if (row[0].mixProd(row[1], row[2]) < 0)
			for (usize i = 0; i < 3; i++) {
				result.scale[i] *= DataType(-1);
				row[i] *= DataType(-1);
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
	constexpr Transform3D decompose() const
	requires Type::Ex::Math::Matrix::ValidTransform3D<R, C> {
		Vector4 _p;
		Vector3 _s;
		return decompose(_p, _s);
	}

	constexpr DataType cofactor(usize const row, usize const col) const {
		return ((((row + col) % 2) == 0) ? DataType(+1) : DataType(-1)) * truncated(row, col).determinant();
	}

private:
	template<usize R2, usize C2, CTL::Type::Math::Operatable T2> friend class Matrix;
	template<typename T2> friend Matrix<4, 4, T2> translate(Matrix<4, 4, T2>, Vector3 const&);
	/// The matrix's data.
	MatrixType<> data = {};
};

// Template matrices
template <usize R, usize C, CTL::Type::Math::Operatable T> using Mat = Matrix<R, C, T>;

template <CTL::Type::Math::Operatable T> using TMat2x1 = Matrix<2, 1, T>;
template <CTL::Type::Math::Operatable T> using TMat3x1 = Matrix<3, 1, T>;
template <CTL::Type::Math::Operatable T> using TMat4x1 = Matrix<4, 1, T>;

template <CTL::Type::Math::Operatable T> using TMat2x2 = Matrix<2, 2, T>;
template <CTL::Type::Math::Operatable T> using TMat3x2 = Matrix<3, 2, T>;
template <CTL::Type::Math::Operatable T> using TMat4x2 = Matrix<4, 2, T>;

template <CTL::Type::Math::Operatable T> using TMat2x3 = Matrix<2, 3, T>;
template <CTL::Type::Math::Operatable T> using TMat3x3 = Matrix<3, 3, T>;
template <CTL::Type::Math::Operatable T> using TMat4x3 = Matrix<4, 3, T>;

template <CTL::Type::Math::Operatable T> using TMat2x4 = Matrix<2, 4, T>;
template <CTL::Type::Math::Operatable T> using TMat3x4 = Matrix<3, 4, T>;
template <CTL::Type::Math::Operatable T> using TMat4x4 = Matrix<4, 4, T>;

template <CTL::Type::Math::Operatable T> using TMat2 = TMat2x2<T>;
template <CTL::Type::Math::Operatable T> using TMat3 = TMat3x3<T>;
template <CTL::Type::Math::Operatable T> using TMat4 = TMat4x4<T>;

// Float matrices
typedef TMat2x1<float> Matrix2x1;
typedef TMat3x1<float> Matrix3x1;
typedef TMat4x1<float> Matrix4x1;

typedef TMat2x2<float> Matrix2x2;
typedef TMat3x2<float> Matrix3x2;
typedef TMat4x2<float> Matrix4x2;

typedef TMat2x3<float> Matrix2x3;
typedef TMat3x3<float> Matrix3x3;
typedef TMat4x3<float> Matrix4x3;

typedef TMat2x4<float> Matrix2x4;
typedef TMat3x4<float> Matrix3x4;
typedef TMat4x4<float> Matrix4x4;

typedef TMat2<float> Matrix2;
typedef TMat3<float> Matrix3;
typedef TMat4<float> Matrix4;

// Double matrices
typedef TMat2x1<double> Matrix2x1d;
typedef TMat3x1<double> Matrix3x1d;
typedef TMat4x1<double> Matrix4x1d;

typedef TMat2x2<double> Matrix2x2d;
typedef TMat3x2<double> Matrix3x2d;
typedef TMat4x2<double> Matrix4x2d;

typedef TMat2x3<double> Matrix2x3d;
typedef TMat3x3<double> Matrix3x3d;
typedef TMat4x3<double> Matrix4x3d;

typedef TMat2x4<double> Matrix2x4d;
typedef TMat3x4<double> Matrix3x4d;
typedef TMat4x4<double> Matrix4x4d;

typedef TMat2<double> Matrix2d;
typedef TMat3<double> Matrix3d;
typedef TMat4<double> Matrix4d;

// Long double matrices
typedef TMat2x1<long double> Matrix2x1ld;
typedef TMat3x1<long double> Matrix3x1ld;
typedef TMat4x1<long double> Matrix4x1ld;

typedef TMat2x2<long double> Matrix2x2ld;
typedef TMat3x2<long double> Matrix3x2ld;
typedef TMat4x2<long double> Matrix4x2ld;

typedef TMat2x3<long double> Matrix2x3ld;
typedef TMat3x3<long double> Matrix3x3ld;
typedef TMat4x3<long double> Matrix4x3ld;

typedef TMat2x4<long double> Matrix2x4ld;
typedef TMat3x4<long double> Matrix3x4ld;
typedef TMat4x4<long double> Matrix4x4ld;

typedef TMat2<long double> Matrix2ld;
typedef TMat3<long double> Matrix3ld;
typedef TMat4<long double> Matrix4ld;

// Integer matrices
typedef TMat2x1<int> Matrix2x1i;
typedef TMat3x1<int> Matrix3x1i;
typedef TMat4x1<int> Matrix4x1i;

typedef TMat2x2<int> Matrix2x2i;
typedef TMat3x2<int> Matrix3x2i;
typedef TMat4x2<int> Matrix4x2i;

typedef TMat2x3<int> Matrix2x3i;
typedef TMat3x3<int> Matrix3x3i;
typedef TMat4x3<int> Matrix4x3i;

typedef TMat2x4<int> Matrix2x4i;
typedef TMat3x4<int> Matrix3x4i;
typedef TMat4x4<int> Matrix4x4i;

typedef TMat2<int> Matrix2i;
typedef TMat3<int> Matrix3i;
typedef TMat4<int> Matrix4i;

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

typedef Matrix2 Mat2;
typedef Matrix3 Mat3;
typedef Matrix4 Mat4;

// Double shorthands
typedef Matrix2x1d Mat2x1d;
typedef Matrix3x1d Mat3x1d;
typedef Matrix4x1d Mat4x1d;

typedef Matrix2x2d Mat2x2d;
typedef Matrix3x2d Mat3x2d;
typedef Matrix4x2d Mat4x2d;

typedef Matrix2x3d Mat2x3d;
typedef Matrix3x3d Mat3x3d;
typedef Matrix4x3d Mat4x3d;

typedef Matrix2x4d Mat2x4d;
typedef Matrix3x4d Mat3x4d;
typedef Matrix4x4d Mat4x4d;

typedef Matrix2d Mat2d;
typedef Matrix3d Mat3d;
typedef Matrix4d Mat4d;

//Long double shorthands
typedef Matrix2x1ld Mat2x1ld;
typedef Matrix3x1ld Mat3x1ld;
typedef Matrix4x1ld Mat4x1ld;

typedef Matrix2x2ld Mat2x2ld;
typedef Matrix3x2ld Mat3x2ld;
typedef Matrix4x2ld Mat4x2ld;

typedef Matrix2x3ld Mat2x3ld;
typedef Matrix3x3ld Mat3x3ld;
typedef Matrix4x3ld Mat4x3ld;

typedef Matrix2x4ld Mat2x4ld;
typedef Matrix3x4ld Mat3x4ld;
typedef Matrix4x4ld Mat4x4ld;

typedef Matrix2ld Mat2ld;
typedef Matrix3ld Mat3ld;
typedef Matrix4ld Mat4ld;

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

typedef Matrix2i Mat2i;
typedef Matrix3i Mat3i;
typedef Matrix4i Mat4i;

// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
constexpr Matrix4x4 lookAt(Vector3 const& eye, Vector3 const& at, Vector3 const& up) {
	Vector3 const f((at - eye).normalized());
	Vector3 const s(f.crossProd(up).normalized());
	Vector3 const u(s.crossProd(f));
	Matrix4x4 result(1);
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
template<CTL::Type::Math::Operatable T>
constexpr Matrix<4, 4, T> ortho(
	T const& left,
	T const& right,
	T const& bottom,
	T const& top,
	T const& zNear,
	T const& zFar
) {
	Matrix<4, 4, T> result(1);
	result[0][0] = T(2) / (right - left);
	result[1][1] = T(2) / (top - bottom);
	result[2][2] = - T(2) / (zFar - zNear);
	result[3][0] = - (right + left) / (right - left);
	result[3][1] = - (top + bottom) / (top - bottom);
	result[3][2] = - (zFar + zNear) / (zFar - zNear);
	return result;
}

// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
template<CTL::Type::Math::Operatable T>
constexpr Matrix<4, 4, T> infiniteOrtho(
	T const& left,
	T const& right,
	T const& bottom,
	T const& top
) {
	Matrix<4, 4, T> result(1);
	result[0][0] = T(2) / (right - left);
	result[1][1] = T(2) / (top - bottom);
	result[2][2] = - T(1);
	result[3][0] = - (right + left) / (right - left);
	result[3][1] = - (top + bottom) / (top - bottom);
	return result;
}

// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
template<CTL::Type::Math::Operatable T>
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
template<CTL::Type::Math::Operatable T>
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
template<CTL::Type::Math::Operatable T>
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
template<CTL::Type::Math::Operatable T>
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

// https://github.com/g-truc/glm/blob/master/glm/gtx/euler_angles.inl
template<CTL::Type::Math::Operatable T>
constexpr Vector3 getEulerAnglesYXZ(Matrix<4, 4, T> const& mat) {
	float T1 = atan2(mat[2][0], mat[2][2]);
	float C2 = sqrt(mat[0][1]*mat[0][1] + mat[1][1]*mat[1][1]);
	float T2 = atan2(-mat[2][1], C2);
	float S1 = sin(T1);
	float C1 = cos(T1);
	float T3 = atan2(S1*mat[1][2] - C1*mat[1][0], C1*mat[0][0] - S1*mat[0][2]);
	return Vector3(T1, T2, T3);
}

// https://github.com/g-truc/glm/blob/master/glm/gtx/euler_angles.inl
template<CTL::Type::Math::Operatable T>
constexpr Vector3 getEulerAnglesXYZ(Matrix<4, 4, T> const& mat) {
	float T1 = atan2(mat[2][1], mat[2][2]);
	float C2 = sqrt(mat[0][0]*mat[0][0] + mat[1][0]*mat[1][0]);
	float T2 = atan2(-mat[2][0], C2);
	float S1 = sin(T1);
	float C1 = cos(T1);
	float T3 = atan2(S1*mat[0][2] - C1*mat[0][1], C1*mat[1][1] - S1*mat[1][2]);
	return Vector3(T1, T2, T3);
}

template <Type::Ex::Math::Matrix::Matrix T>
constexpr T lerp(T const& from, T const& to, typename T::DataType const& by) {
	T result(0);
	for (usize i = 0; i < T::COLUMNS; i++)
		for (usize j = 0; j < T::ROWS; j++)
			result[i][j] =CTL::Math::lerp<typename T::DataType>(from[i][j], to[i][j], by);
	return result;
}

template <Type::Ex::Math::Matrix::Matrix T>
constexpr T lerp(T const& from, T const& to, T const& by) {
	T result(0);
	for (usize i = 0; i < T::COLUMNS; i++)
		for (usize j = 0; j < T::ROWS; j++)
			result[i][j] = CTL::Math::lerp<typename T::DataType>(from[i][j], to[i][j], by[i][j]);
	return result;
}

}

CTL_EX_NAMESPACE_END

#endif // CTL_EX_MATH_MATRIX_H
