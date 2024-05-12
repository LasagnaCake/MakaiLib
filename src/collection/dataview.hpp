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

		constexpr Referend(T& _data):					data(&_data)		{}
		constexpr Referend(Referend<T> const& other):	data(other.data)	{}
		constexpr Referend(Referend<T>&& other):		data(other.data)	{}

		constexpr Referend& rebind(T& data)	{this->data = &data; return (*this);}

		constexpr bool exists() {return data != nullptr;};

		constexpr Referend& operator=(T const& val)				{if (exists()) (*data) = val; return (*this);	}
		constexpr Referend& operator=(Referend<T> const& val)	{data = val.data; return (*this);				}

		constexpr T& operator*()		{assertExistence(); return (*data);}
		constexpr T& operator*() const	{assertExistence(); return (*data);}

		constexpr operator T&()			{assertExistence(); return (*data);}
		constexpr operator T&() const	{assertExistence(); return (*data);}
	private:
		T* data = nullptr;

		constexpr void assertExistence() {if (!exists()) throw Error::NullPointer("Referend is not bound to any data!");}
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

	template<class T> class Nullable;

	template<> class Nullable<void> {
	public:
		typedef decltype(nullptr) NullType;

		constexpr Nullable() noexcept			{}
		constexpr Nullable(NullType) noexcept	{}

		constexpr ~Nullable() {}
	};

	template<Type::Different<void> T>
	class Nullable<T> {
	public:
		typedef T DataType;
		typedef decltype(nullptr) NullType;

		constexpr Nullable() noexcept											{}
		constexpr Nullable(NullType) noexcept									{}
		constexpr Nullable(DataType const& value): isSet(true)					{data = value;}
		constexpr Nullable(DataType&& value): isSet(true)						{data = std::move(value);}
		constexpr Nullable(Nullable const& other): isSet(other.isSet)			{if (other.isSet) data = other.data;}
		constexpr Nullable(Nullable && other): isSet(std::move(other.isSet))	{if (other.isSet) data = std::move(other.data);}

		constexpr ~Nullable() {}

		constexpr DataType value(DataType const& fallback = DataType()) const {return (isSet) ? data : fallback;}

		constexpr Nullable& then(Operation<DataType> const& op) {if (isSet) data = op(data); return *this;}

		constexpr bool exists()		const {return isSet;	}
		constexpr operator bool()	const {return exists();	}
		constexpr bool operator()()	const {return exists();	}

		constexpr Nullable& operator()(Operation<DataType> const& op) {return then();}

		constexpr Nullable& operator=(DataType const& value)	{data = value; isSet = true; return *this;				}
		constexpr Nullable& operator=(DataType&& value)			{data = std::move(value); isSet = true; return *this;	}
		constexpr Nullable& operator=(NullType)					{isSet = false; return *this;							}

		constexpr bool operator==(Nullable const& other) const	{if (isSet) return other == data; return false;}
		constexpr bool operator==(T const& value) const			{if (isSet) return data == value; return false;}
		constexpr bool operator==(T&& value) const				{if (isSet) return data == value; return false;}
		constexpr bool operator==(NullType) const				{return !isSet;}

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

		constexpr DataType operator*() const {
			if (isSet) return data;
			throw Error::NonexistentValue(
				"Value is not set!",
				__FILE__,
				toString(__LINE__),
				"operator T()"
			);
		}

		/*constexpr operator DataType() const requires Type::Different<DataType, bool> {
			if (isSet) return data;
			throw Error::NonexistentValue(
				"Value is not set!",
				__FILE__,
				toString(__LINE__),
				"operator T()"
			);
		}*/

	private:
		DataType data;
		bool isSet = false;
	};

	template<typename T> class Functor;

	template<typename F, typename... Args>
	class Functor<F(Args...)> {
	private:
	public:
		typedef Function<F(Args...)>	FunctionType;
		typedef F						ReturnType;

		Functor(): id(0) {}

		Functor(FunctionType const& f): func(f), id(++count)			{}
		Functor(Functor const& other): func(other.func), id(other.id)	{}

		Functor& operator=(FunctionType const& f)	{func = f; id = ++count; return *this;				}
		Functor& operator=(Functor const& other)	{func = other.func; id = other.id; return *this;	}

		Nullable<F> evoke(Args... args) const requires Type::Different<F, void>			{if (id) return func(args...); return nullptr;	}
		Nullable<F> operator()(Args... args) const requires Type::Different<F, void>	{return evoke(args...);							}

		void evoke(Args... args) const requires Type::Equal<F, void>					{if (id) func(args...);							}
		void operator()(Args... args) const requires Type::Equal<F, void>				{evoke(args...);								}

		operator bool() const {return id;}

		bool operator==(Functor const& other) const {return id == other.id;}

		Helper::PartialOrder operator<=>(Functor const& other) const {return id <=> other.id;}

	private:
		FunctionType	func;
		size_t			id = 0;
		inline static size_t count = 0;
	};
}

using namespace View;

#endif // DATA_VISUALIZATION_H
