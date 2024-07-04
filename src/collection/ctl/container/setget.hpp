#ifndef CTL_CONTAINER_SETGET_H
#define CTL_CONTAINER_SETGET_H

#include "function.hpp"

template<typename TData, template<typename TData> Function<void(TData&, TData)> setter>
struct Setter:
	SelfIdentified<Setter<TData, setter>>,
	Typed<TData> {
private:
	DataType& data;
public:
	constexpr Setter(DataType& var):	data(var)			{}
	constexpr Setter(SelfType&& other):	Setter(other.data)	{}

	constexpr static Function<void(TData&, TData)> setterFunction = setter;

	constexpr SelfType& set(DataType const& value)			{setter(data, value); return *this;	}
	constexpr SelfType& operator=(DataType const& value)	{return set(value);					}
};

template<typename TData, template<typename TData> Function<TData(TData&)> getter>
struct Getter:
	SelfIdentified<Getter<TData, getter>>,
	Typed<TData> {
private:
	DataType& data;
public:
	constexpr Getter(DataType& var):	data(var)			{}
	constexpr Getter(SelfType&& other):	Getter(other.data)	{}

	constexpr static Function<TData(TData&)> getterFunction = getter;

	constexpr DataType operator DataType() const {return getter(value);	}
	constexpr DataType operator DataType() const {return get(value);	}
};

template<
	typename TData,
	template<typename TData> Function<void(TData&, TData)>	setter,
	template<typename TData> Function<TData(TData&)>		getter
>
struct SetGet:
	SelfIdentified<SetGet<TData, setter, getter>>,
	Typed<TData>,
	Setter<TData, setter>,
	Getter<TData, getter> {
private:
public:
	constexpr SetGet(DataType& var):	Setter(var), Getter(var)	{}
	constexpr SetGet(SelfType&& other):	SetGet(other.data)			{}

	constexpr SelfType& operator=(DataType const& value) {set(value); return *this;}
};

#endif // CTL_CONTAINER_SETGET_H
