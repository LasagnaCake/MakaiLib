#ifndef DATA_VISUALIZATION_H
#define DATA_VISUALIZATION_H

#include "helper.hpp"
#include "conceptual.hpp"

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
}

using namespace View;

#endif // DATA_VISUALIZATION_H
