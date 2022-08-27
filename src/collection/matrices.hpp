#ifndef FLOAT_MATRICES_234_H
#define FLOAT_MATRICES_234_H

#include "algebra.hpp"
#include <vector>
#include <stdexcept>

namespace Matrix {
	namespace {
		using
			std::vector,
			std::invalid_argument;

		/**
		* [---------------------]
		* [                     ]
		* [  Base Matrix Class  ]
		* [                     ]
		* [---------------------]
		*/
		template<size_t R, size_t C, typename T>
		class Mat {
		public:
			/// Constructors.
			Mat() {
				// Reserve rows
				rows.resize(R);
				// For every row:
				for (size_t i = 0; i < R; i++) {
					// Reserve cells
					rows[i].resize(C);
					// For every cell, set to 0
					for (size_t j = 0; j < C; j++)
						rows[i][j] = 0;
				}
			}

			Mat(T v) {
				// Reserve rows
				rows.resize(R);
				// For every row:
				for (size_t i = 0; i < R; i++) {
					// Reserve cells
					rows[i].resize(C);
					// For every cell, set to V
					for (size_t j = 0; j < C; j++)
						rows[i][j] = v;
				}
			}

			Mat(vector<vector<T>> v) {
				// If sizes do not match, throw error
				if (C != v.size() || R != v[0].size())
					throw invalid_argument(
						"Matrices' dimensions do not match."
					);
				// Reserve rows
				rows.resize(R);
				// For every row:
				for (size_t i = 0; i < R; i++) {
					// Reserve cells
					rows[i].resize(C);
					// For every cell, set to equivalent cell in V
					for (size_t j = 0; j < C; j++)
						rows[i][j] = v[i][j];
				}
			}

			/// Gets the transposed matrix.
			Mat transposed() {
				// Resulting matrix
				Mat<C, R, T> res;
				// Transpose matrix
				for (size_t i = 0; i < R; i++)
					for (size_t j = 0; j < C; i++)
						res[j][i] = rows[i][j];
				// Return transposed matrix
				return res;
			}

			/// Arithmetic operator overloading.
			Mat operator+(Mat mat) {
				Mat<R, C, T> res;
				// If sizes do not match, throw error
				if (C != mat.size() || R != mat[0].size())
					throw invalid_argument(
						"Matrices' dimensions do not match."
					);
				// Loop through cells and add them up
				for (size_t i = 0; i < R; i++)
					for (size_t j = 0; j < C; i++)
						res[j][i] = rows[i][j] + mat[i][j];
				// Return resulting matrix
				return res;
			}

			Mat operator+(T val) {
				// Result
				Mat<R, C, T> res;
				// Loop through cells and add them up
				for (size_t i = 0; i < R; i++)
					for (size_t j = 0; j < C; i++)
						res[j][i] = rows[i][j] + val;
				// Return resulting matrix
				return res;
			}

			Mat operator-(T val) {
				// Result
				Mat<R, C, T> res;
				// Loop through cells and subtract them up
				for (size_t i = 0; i < R; i++)
					for (size_t j = 0; j < C; i++)
						res[j][i] = rows[i][j] - val;
				// Return resulting matrix
				return res;
			}

			Mat operator-(Mat mat) {
				Mat<R, C, T> res;
				// If sizes do not match, throw error
				if (C != mat.size() || R != mat[0].size())
					throw invalid_argument(
						"Matrices' dimensions do not match."
					);
				// Loop through cells and subtract them up
				for (size_t i = 0; i < R; i++)
					for (size_t j = 0; j < C; i++)
						res[j][i] = rows[i][j] - mat[i][j];
				// Return resulting matrix
				return res;
			}

			Mat operator*(T val) {
				// Result
				Mat<R, C, T> res;
				// Loop through cells and multiply them up
				for (size_t i = 0; i < R; i++)
					for (size_t j = 0; j < C; i++)
						res[j][i] = rows[i][j] * val;
				// Return resulting matrix
				return res;
			}

			Mat operator*(Mat mat) {
				// If given matrix's row count does not match this matrix's column count, throw error
				if (C != mat.size())
					throw invalid_argument(
						"The matrix's column count does not match the given matrix's row count."
					);
				// Reulting matrix
				Mat<R, mat[0].size(), T> res;
				// Transpose given matrix, for easier calculation
				mat = mat.transposed();
				// Loop through rows and get the dot product
				for (size_t i = 0; i < R; i++)
					for (size_t j = 0; j < mat[0].size(); i++)
						res[i][j] = rdp(i, mat[j]);
				// Return resulting matrix
				return res;
			}

			//TODO division

			/// Other operator overloadings.
			vector<T>& operator[](size_t idx) {
				return rows[idx];
			}

			/// Returns the matrix's row count ("size").
			size_t size() {
				return rows.size();
			}

			/// Gets the dot product of a given row by another given row.
			T rdp(size_t row, vector<T> other) {
				T res = 0;
				for (size_t i = 0; i < other.size(); i++)
					res += other[i] * rows[row][i];
				return res;
			}

		private:
			/// The matrix's columns;
			vector<vector<T>> rows;
		};
	}

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
