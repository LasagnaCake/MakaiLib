#ifndef GL_GRAPH_UNIFORM_H
#define GL_GRAPH_UNIFORM_H

#include "../../ctl/ctl.hpp"

namespace Makai::Graph {
	struct Uniform {
		String const name;

		Uniform(String const& name, uint const& id);

		void set(bool const& value, usize const& offset = 0) const;

		void set(int const& value, usize const& offset = 0) const;

		void set(unsigned int const& value, usize const& offset = 0) const;

		void set(float const& value, usize const& offset = 0) const;

		void set(Vector2 const& value, usize const& offset = 0) const;

		void set(Vector3 const& value, usize const& offset = 0) const;

		void set(Vector4 const& value, usize const& offset = 0) const;

		void set(Matrix3x3 const& value, usize const& offset = 0) const;

		void set(Matrix4x4 const& value, usize const& offset = 0) const;

		void setArray(int* const& values, usize const& count, usize const& offset = 0) const;

		void setArray(unsigned int* const& values, usize const& count, usize const& offset = 0) const;

		void setArray(float* const& values, usize const& count, usize const& offset = 0) const;

		void setArray(Vector2* const& values, usize const& count, usize const& offset = 0) const;

		void setArray(Vector3* const& values, usize const& count, usize const& offset = 0) const;

		void setArray(Vector4* const& values, usize const& count, usize const& offset = 0) const;

		template <typename T>
		void set(List<T> const& values, usize const& offset = 0) const;

		template <typename T, usize S>
		void set(Span<T, S> const& values, usize const& offset = 0) const;

		template <typename... Args>
		void set(Args const&... args) const
		requires (sizeof...(Args) > 1);

		template <Type::Class T>
		void forEach(List<T> const& values, Functor<void(T&, Uniform const&)> func) const;

		template <typename T>			void operator()(T const& value)				{set(value);	}
		template <typename... Args>		void operator()(Args const&... args)		{set(args...);	}
		template <typename T>			void operator()(List<T> const& values)		{set(values);	}
		template <typename T, usize S>	void operator()(Span<T, S> const& values)	{set(values);	}

	private:
		constexpr Uniform(
			String  const& _name,
			uint const& _id,
			uint const& _location
		): name(_name), id(_id), location(_location) {}

		template<typename T>
		void setSpecial(T const& value, usize& offset) const;

		template <typename T>
		void setSpecial(List<T> const& values, usize& offset) const;

		template <typename T, usize S>
		void setSpecial(Span<T, S> const& values, usize& offset) const;

		uint getUniformArray(usize const& offset) const;

		inline uint getUniform() const;

		inline uint getUniform(String const& append) const;

		uint const id;
		intmax const location;

		usize mutable offset;
	};
}

#endif // GL_GRAPH_UNIFORM_H
