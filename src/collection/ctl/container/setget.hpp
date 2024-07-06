#ifndef CTL_CONTAINER_SETGET_H
#define CTL_CONTAINER_SETGET_H

#include "function.hpp"

template<typename TData>
struct Setter:
	SelfIdentified<Setter<TData>>,
	Typed<TData> {
private:
public:
	typedef Function<void(DataType)>  SetterFunction;

	constexpr Setter(SetterFunction&& func):		setter(move(func))	{}
	constexpr Setter(SetterFunction const& func):	setter(func)		{}
	constexpr Setter(SelfType&& func):				Setter(other.func)	{}
	constexpr Setter(SelfType const& func):			Setter(other.func)	{}

	constexpr SelfType& set(DataType const& value)			{setter(value); return *this;	}
	constexpr SelfType& operator=(DataType const& value)	{return set(value);				}

protected:
	SetterFunction const setter;
private:
};

template<typename TData>
struct Getter:
	SelfIdentified<Getter<TData>>,
	Typed<TData> {
private:
public:
	typedef Function<DataType()> GetterFunction;

	constexpr Getter(GetterFunction&& func):		getter(move(func))	{}
	constexpr Getter(GetterFunction const& func):	getter(func)		{}
	constexpr Getter(SelfType&& other):				Getter(other.func)	{}
	constexpr Getter(SelfType const& other):		Getter(other.func)	{}

	constexpr DataType operator DataType() const {return getter(value);	}
	constexpr DataType operator DataType() const {return get(value);	}

protected:
	GetterFunction const getter;
private:
};

template<typename TData>
struct SetGet:
	Setter<TData>,
	Getter<TData>,
	SelfIdentified<SetGet<TData>>,
	Typed<TData> {
private:
public:
	constexpr SetGet(SetterFunction&& set, GetterFunction&& get):			Setter(move(set)), Getter(move(get))			{}
	constexpr SetGet(SetterFunction&& set, GetterFunction const& get):		Setter(move(set)), Getter(get)					{}
	constexpr SetGet(SetterFunction	const& set, GetterFunction&& get):		Setter(set), Getter(move(get))					{}
	constexpr SetGet(SetterFunction const& set, GetterFunction const& get):	Setter(set), Getter(get)						{}
	constexpr SetGet(SelfType&& other):										SetGet(move(other.setter), move(other.getter))	{}
	constexpr SetGet(SelfType const& other):								SetGet(other.setter, other.getter)				{}

	constexpr SelfType& set(DataType const& value)			{Setter::set(value); return *this;	}
	constexpr SelfType& operator=(DataType const& value)	{return set(value);					}
};

#endif // CTL_CONTAINER_SETGET_H
