#ifndef CTL_CONTAINER_FUNCTOR_H
#define CTL_CONTAINER_FUNCTOR_H

#include "../namespace.hpp"
#include "nullable.hpp"
#include "../staticvalue.hpp"
#include "function.hpp"

CTL_NAMESPACE_BEGIN

template<typename T> class Functor;

template<typename TReturn, typename... TArgs>
struct Functor<TReturn(TArgs...)>:
	Ordered,
	Typed<TReturn>,
	SelfIdentified<Functor<TReturn, TArgs...>>,
	Returnable<TReturn>,
	Argumented<TArgs...>,
	Functional<TReturn(TArgs...)>,
	Derived<Function<TReturn(TArgs...)>>,
	private Function<TReturn(TArgs...)> {
private:
public:
	using Typed				= Typed<TReturn>;
	using SelfIdentified	= SelfIdentified<Functor<TReturn, TArgs...>>;
	using Returnable		= Returnable<TReturn>;
	using Argumented		= Argumented<TArgs...>;
	using Functional		= Functional<TReturn(TArgs...)>;
	using Derived			= Derived<Function<TReturn(TArgs...)>>;

	using typename Derived::BaseType;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Returnable::ReturnType
	;

	using
		typename Ordered::OrderType,
		typename Ordered::Order
	;

	constexpr static bool PROCEDURE = Type::Different<ReturnType, void>;

	constexpr Functor(): id(0) {}

	constexpr Functor(BaseType const& f): BaseType(f), id(++count)			{}
	constexpr Functor(SelfType const& other): BaseType(other), id(other.id)	{}

	constexpr SelfType& operator=(BaseType const& f)		{BaseType::operator=(f); id = ++count; return *this;		}
	constexpr SelfType& operator=(SelfType const& other)	{BaseType::operator=(other); id = other.id; return *this;	}

	constexpr Nullable<ReturnType> evoke(TArgs... args) const requires (PROCEDURE)		{if (id) return BaseType::operator()(forward<TArgs>(args)...); return nullptr;	}
	constexpr Nullable<ReturnType> operator()(TArgs... args) const requires (PROCEDURE)	{return evoke(forward<TArgs>(args)...);											}

	constexpr void evoke(TArgs... args) const requires (!PROCEDURE)			{if (id) BaseType::operator()(forward<TArgs>(args)...);	}
	constexpr void operator()(TArgs... args) const requires (!PROCEDURE)	{evoke(forward<TArgs>(args)...);						}

	constexpr operator bool() const {return id;}

	constexpr bool operator==(SelfType const& other) const {return id == other.id;}

	constexpr OrderType operator<=>(SelfType const& other) const {return id <=> other.id;}

private:
	WrapperType			func;
	usize				id = 0;
	inline static usize	count;
};

template<class T>						using Procedure	= Functor<void(T)>;
template<class T, class T2 = T const&>	using Operation	= Functor<T2(T)>;

template<class T = void>
using Signal	= Procedure<T>;

template<class T = void>
using Trigger	= Functor<bool(T)>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_FUNCTOR_H
