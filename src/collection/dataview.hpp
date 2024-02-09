#ifndef DATA_VISUALIZATION_H
#define DATA_VISUALIZATION_H

#include "helper.hpp"
#include "conceptual.hpp"
#include "errors.hpp"

namespace View {
	template <typename T> concept Viewable = Type::Safe<T>;

	template<Viewable T>
	class Referend {
	public:
		typedef T DataType;

		Referend(T& _data):					data(&_data)		{}
		Referend(Referend<T> const& other):	data(other.data)	{}
		Referend(Referend<T>&& other):		data(other.data)	{}

		inline Referend& rebind(T& data)	{this->data = &data; return (*this);}

		bool exists() {return data != nullptr;};

		inline Referend& operator=(T const& val)			{if (exists()) (*data) = val; return (*this);	}
		inline Referend& operator=(Referend<T> const& val)	{data = val.data; return (*this);				}

		inline T& operator*()		{assertExistence(); return (*data);}
		inline T& operator*() const	{assertExistence(); return (*data);}

		inline operator T&()		{assertExistence(); return (*data);}
		inline operator T&() const	{assertExistence(); return (*data);}
	private:
		T* data = nullptr;

		inline void assertExistence() {if (!exists()) throw Error::NullPointer("Referend is not bound to any data!");}
	};

	template<Viewable T>
	class DataView {
	public:
		typedef T DataType;

		constexpr DataView(T& _data):					data(_data)			{}
		constexpr DataView(DataView<T> const& other):	data(other.data)	{}
		constexpr DataView(DataView<T>&& other):		data(other.data)	{}

		constexpr DataView& modify(Operation<T> op)		{data = op(data); return (*this);	}
		constexpr DataView& operator()(Operation<T> op)	{return modify(op);					}

		constexpr DataView& operator=(T const& val) {data = val; return (*this);}

		constexpr operator T() const	{return value();	}
		constexpr operator T()			{return value();	}
		constexpr T value() const		{return data;		}
		constexpr T value() 			{return data;		}

	protected:
		constexpr T& view()				{return data;}
		constexpr const T& view() const	{return data;}

	private:
		T& data;
	};

	template<typename T>
	class Nullable {
	public:
		typedef T DataType;
		typedef decltype(nullptr) NullType;

		constexpr Nullable() noexcept											{}
		constexpr Nullable(NullType) noexcept									{}
		constexpr Nullable(T const& value): isSet(true)							{data = value;}
		constexpr Nullable(T&& value): isSet(true)								{data = std::move(value);}
		constexpr Nullable(Nullable<T> const& other): isSet(other.isSet)		{if (other.isSet) data = other.data;}
		constexpr Nullable(Nullable<T>&& other): isSet(std::move(other.isSet))	{if (other.isSet) data = std::move(other.data);}

		constexpr ~Nullable() {}

		constexpr T value(T const& fallback = T()) const {return (isSet) ? data : fallback;}

		constexpr Nullable& then(Operation<T> const& op) {if (isSet) data = op(data); return *this;}

		constexpr bool exists()		const {return isSet;}
		constexpr operator bool()	const {return exists();}
		constexpr bool operator()()	const {return exists();}

		constexpr Nullable& operator()(Operation<T> const& op) {return then();}

		constexpr Nullable& operator=(T const& value)	{data = value; isSet = true; return *this;}
		constexpr Nullable& operator=(T&& value)		{data = std::move(value); isSet = true; return *this;}
		constexpr Nullable& operator=(NullType)			{isSet = false; return *this;}

		constexpr Nullable& operator=(Nullable<T> const& other) {
			if (other.isSet) data = other.data;
			isSet = other.isSet;
			return *this;
		}

		constexpr Nullable& operator=(Nullable<T>&& other) {
			if (other.isSet) data = std::move(other.data);
			isSet = std::move(other.isSet);
			return *this;
		}

		constexpr T operator *() const {
			if (isSet) return value;
			throw Error::NonexistentValue(
				"Value is not set!",
				__FILE__,
				toString(__LINE__),
				"operator T()"
			);
		}

		constexpr operator T() const {
			if (isSet) return value;
			throw Error::NonexistentValue(
				"Value is not set!",
				__FILE__,
				toString(__LINE__),
				"operator T()"
			);
		}

	private:
		T data;
		bool isSet = false;
	};
}

using namespace View;

#endif // DATA_VISUALIZATION_H
