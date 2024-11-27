#ifndef MAKAILIB_GRAPH_UNIFORM_H
#define MAKAILIB_GRAPH_UNIFORM_H

#include "../../compat/ctl.hpp"
#include "core.hpp"

namespace Makai::Graph {
	struct Uniform {
		String const name;

		Uniform(String const& name, uint const id);

		void set(bool const value, usize const offset = 0) const;

		void set(int const value, usize const offset = 0) const;

		void set(uint const value, usize const offset = 0) const;

		void set(float const value, usize const offset = 0) const;

		void set(Vector2 const& value, usize const offset = 0) const;

		void set(Vector3 const& value, usize const offset = 0) const;

		void set(Vector4 const& value, usize const offset = 0) const;

		void set(Matrix3x3 const& value, usize const offset = 0) const;

		void set(Matrix4x4 const& value, usize const offset = 0) const;

		void setArray(int* const& values, usize const count, usize const offset = 0) const;

		void setArray(uint* const& values, usize const count, usize const offset = 0) const;

		void setArray(float* const& values, usize const count, usize const offset = 0) const;

		void setArray(Vector2* const& values, usize const count, usize const offset = 0) const;

		void setArray(Vector3* const& values, usize const count, usize const offset = 0) const;

		void setArray(Vector4* const& values, usize const count, usize const offset = 0) const;

		template <typename T>
		inline void set(List<T> const& values, usize const offset = 0) const {
			this->offset = 0;
			setArray((T*)values.data(), values.size(), offset);
			this->offset = values.size();
		}

		template <typename T, usize S>
		inline void set(Span<T, S> const& values, usize const offset = 0) const {
			this->offset = 0;
			setArray((T*)values.data(), values.size(), offset);
			this->offset = values.size();
		}

		template <typename... Args>
		inline void set(Args const&... args) const
		requires (sizeof...(Args) > 1) {
			this->offset = 0;
			usize off = 0;
			(setSpecial(args, off), ...);
			offset = off;
		}

		template <Makai::Type::Class T>
		inline void forEach(List<T> const& values, Functor<void(T&, Uniform const&)> func) const {
			for (T& val: values) {
				func(val, Uniform(name, id, location + offset+1));
			}
		}

		template <typename T>			inline void operator()(T const& value) const			{set(value);	}
		template <typename... Args>		inline void operator()(Args const&... args) const		{set(args...);	}
		template <typename T>			inline void operator()(List<T> const& values) const		{set(values);	}
		template <typename T, usize S>	inline void operator()(Span<T, S> const& values) const	{set(values);	}

		Uniform operator[](String const& member) const;

	private:
		constexpr Uniform(
			String  const& _name,
			uint const _id,
			uint const _location
		): name(_name), id(_id), location(_location) {}

		template<typename T>
		inline void setSpecial(T const& value, usize& offset) const {
			set(value, offset);
			++offset;
		}

		template <typename T>
		inline void setSpecial(List<T> const& values, usize& offset) const {
			set(values, offset);
			offset += this->offset;
		}

		template <typename T, usize S>
		inline void setSpecial(Span<T, S> const& values, usize& offset) const {
			set(values, offset);
			offset += this->offset;
		}

		uint getUniformArray(usize const offset) const;

		uint getUniform() const;

		uint getUniform(String const& append) const;

		uint const id;
		intmax const location;

		usize mutable offset;
	};
}

#endif // MAKAILIB_GRAPH_UNIFORM_H
