#ifndef CTL_CONTAINER_SETGET_H
#define CTL_CONTAINER_SETGET_H

#include "function.hpp"

CTL_NAMESPACE_BEGIN

template<typename TData>
struct Setter:
	SelfIdentified<Setter<TData>>,
	Typed<TData> {
private:
public:
	using Typed				= ::CTL::Typed<TData>;
	using SelfIdentified	= ::CTL::SelfIdentified<Setter<TData>>;

	using
		typename Typed::DataType,
		typename Typed::ConstReferenceType
	;

	using typename SelfIdentified::SelfType;

	using SetterFunction = Function<void(DataType const&)>;

	constexpr Setter(SetterFunction&& func):		setter(CTL::move(func))	{}
	constexpr Setter(SetterFunction const& func):	setter(func)			{}
	constexpr Setter(SelfType&& other)		= delete;
	constexpr Setter(SelfType const& other)	= delete;

	constexpr SelfType& set(ConstReferenceType value)		{setter(value); return *this;	}
	constexpr SelfType& operator=(ConstReferenceType value)	{return set(value);				}

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
	using Typed				= ::CTL::Typed<TData>;
	using SelfIdentified	= ::CTL::SelfIdentified<Getter<TData>>;

	using typename Typed::DataType;

	using typename SelfIdentified::SelfType;

	using GetterFunction = Function<DataType()>;

	constexpr Getter(GetterFunction&& func):		getter(CTL::move(func))	{}
	constexpr Getter(GetterFunction const& func):	getter(func)			{}
	constexpr Getter(SelfType&& other)		= delete;
	constexpr Getter(SelfType const& other)	= delete;

	constexpr DataType get() const		{return getter();	}
	constexpr operator DataType() const	{return get();		}

protected:
	GetterFunction const getter;
private:
};

template<typename TData>
struct SetGet:
	Setter<TData>,
	Getter<TData>,
	SelfIdentified<SetGet<TData>>,
	Typed<TData>,
	Derived<Setter<TData>, Getter<TData>> {
private:
public:
	using Setter			= ::CTL::Setter<TData>;
	using Getter			= ::CTL::Getter<TData>;
	using SelfIdentified	= ::CTL::SelfIdentified<SetGet<TData>>;

	static_assert(Type::Equal<typename Setter::DataType, typename Getter::DataType>);

	using typename Setter::SetterFunction;

	using typename Getter::GetterFunction;

	using typename SelfIdentified::SelfType;

	using
		typename Setter::DataType,
		typename Setter::ConstReferenceType
	;

	constexpr SetGet(SetterFunction&& set, GetterFunction&& get):			Setter(CTL::move(set)), Getter(CTL::move(get))	{}
	constexpr SetGet(SetterFunction&& set, GetterFunction const& get):		Setter(CTL::move(set)), Getter(get)				{}
	constexpr SetGet(SetterFunction	const& set, GetterFunction&& get):		Setter(set), Getter(CTL::move(get))				{}
	constexpr SetGet(SetterFunction const& set, GetterFunction const& get):	Setter(set), Getter(get)						{}
	constexpr SetGet(SelfType&& other)		= delete;
	constexpr SetGet(SelfType const& other)	= delete;

	constexpr SelfType& set(ConstReferenceType value)		{Setter::set(value); return *this;	}
	constexpr SelfType& operator=(ConstReferenceType value)	{return set(value);					}

	constexpr DataType get() const		{return Getter::get();	}
	constexpr operator DataType() const	{return get();			}
};

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_SETGET_H
