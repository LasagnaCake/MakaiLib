#ifndef CTL_CONTAINER_FUNCTOR_H
#define CTL_CONTAINER_FUNCTOR_H

#include "../namespace.hpp"
#include "nullable.hpp"
#include "../staticvalue.hpp"
#include "function.hpp"

CTL_NAMESPACE_BEGIN

template<typename TFunction> class Functor;

template<typename TReturn, typename... TArgs>
struct Functor<TReturn(TArgs...)>:
	Ordered,
	SelfIdentified<Functor<TReturn(TArgs...)>>,
	Returnable<TReturn>,
	Argumented<TArgs...>,
	Functional<TReturn(TArgs...)> {
private:
public:
	using SelfIdentified	= ::CTL::SelfIdentified<Functor<TReturn(TArgs...)>>;
	using Returnable		= ::CTL::Returnable<TReturn>;
	using Argumented		= ::CTL::Argumented<TArgs...>;
	using Functional		= ::CTL::Functional<TReturn(TArgs...)>;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Returnable::ReturnType
	;

	using
		typename Functional::FunctionType
	;

	using
		typename Ordered::OrderType,
		typename Ordered::Order
	;

	using WrapperType = Function<FunctionType>;

	constexpr static bool PROCEDURE = Type::Different<ReturnType, void>;

	constexpr Functor(): id(0) {}
	constexpr Functor(WrapperType const& f): func(f), id(++count)				{}
	constexpr Functor(SelfType const& other): func(other.func), id(other.id)	{}

	constexpr SelfType& operator=(WrapperType const& f)		{func = f; id = ++count; return *this;				}
	constexpr SelfType& operator=(SelfType const& other)	{func = other.func; id = other.id; return *this;	}

	constexpr Nullable<ReturnType> evoke(TArgs... args) const requires (PROCEDURE)		{if (id) return func(args...); return nullptr;	}
	constexpr Nullable<ReturnType> operator()(TArgs... args) const requires (PROCEDURE)	{return evoke(args...);							}

	constexpr void evoke(TArgs... args) const requires (!PROCEDURE)			{if (id) func(args...);	}
	constexpr void operator()(TArgs... args) const requires (!PROCEDURE)	{evoke(args...);		}

	constexpr operator bool() const {return id;}

	constexpr bool operator==(SelfType const& other) const {return id == other.id;}

	constexpr OrderType operator<=>(SelfType const& other) const {return id <=> other.id;}

private:
	WrapperType			func;
	usize				id = 0;
	inline static usize	count;
};

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_FUNCTOR_H
