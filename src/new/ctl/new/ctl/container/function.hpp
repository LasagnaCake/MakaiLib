#ifndef CTL_CONTAINER_FUNCTION_H
#define CTL_CONTAINER_FUNCTION_H

#include "../templates.hpp"
#include "../typetraits/traits.hpp"
#include "../container/error.hpp"
#include "nullable.hpp"
#include "../staticvalue.hpp"

// Function implementation based off of: https://stackoverflow.com/a/61191826

CTL_NAMESPACE_BEGIN

namespace Impl {
	namespace Partial {
		template <typename TReturn, typename... TArgs>
		struct Function {
			constexpr virtual TReturn invoke(TArgs...) const		= 0;
			constexpr TReturn operator()(TArgs... args) const		{return invoke(forward<Args>(args)...);}
			constexpr virtual ~Function()							= default;
		};
	}

	template <typename TFunction, typename TReturn, typename... TArgs>
	class Function: Partial::Function<TReturn, TArgs...> {
		TFunction func;
		constexpr TReturn invoke(TArgs... args) const override {return func(forward<Args>(args)...);}
		constexpr Function(TFunction&& _func):func(_func) {}
	};
}

template<typename T> struct Function;

template<typename TReturn, typename... TArgs>
struct Function<TReturn(TArgs...)>:
	Typed<TReturn>,
	SelfIdentified<Function<TReturn, TArgs...>>,
	Returnable<TReturn>,
	Argumented<TArgs...>,
	Functional<TReturn(TArgs...)> {
private:
	using Functional		= Functional<TReturn(TArgs...)>;
	using SelfIdentified	= SelfIdentified<Function<TReturn, TArgs...>>;
	using Typed				= Typed<TReturn>;
	using Returnable		= Returnable<TReturn>;
	using Argumented		= Argumented<TArgs...>;

	using SelfIdentified::SelfType;

	Impl::Partial::Function<ReturnType, TArgs...>* func{nullptr};

	template<typename TFunction>
	constexpr static Impl::Function<
		Decay::AsArgument<TFunction>,
		TReturn,
		TArgs...
	>
	makeCallable(TFunction&& f) {
		return new Impl::Function(move(f));
	}

	template<typename TFunction>
	constexpr static Impl::Function<
		Decay::AsArgument<TFunction>,
		TReturn,
		TArgs...
	>
	makeCallable(TFunction const& f) {
		return new Impl::Function(f);
	}

	constexpr void destroy() {
		if (func) delete func;
		func = nullptr;
	}

	[[noreturn]] void badCallError() {
		throw Error::InvalidCall(
			"No function was assigned!",
			__FILE__,
			"unspecified",
			"Function::invoke"
		);
	}

public:
	constexpr ReturnType invoke(TArgs... args) const {
		if (!func) badCallError();
        return func->invoke(forward<TArgs>(args)...);
    }

    constexpr ReturnType operator()(TArgs... args) const {
        return invoke(forward<TArgs>(args)...);
    }

    constexpr ~Function() {destroy();}

	template<typename TFunction>
    constexpr SelfType& operator=(TFunction&& f)		{destroy(); func = makeCallable(f); return *this;					}
    template<typename TFunction>
    constexpr SelfType& operator=(TFunction const& f)	{destroy(); func = makeCallable(f); return *this;					}
    constexpr SelfType& operator=(SelfType&& f)			{destroy(); func = move(f.func); f.func = nullptr; return *this;	}
    constexpr SelfType& operator=(SelfType const& f)	{destroy(); return operator=(f.func->func);							}


    template<typename TFunction>
    constexpr Function(TFunction&& f)		{operator=(f);	}
    template<typename TFunction>
    constexpr Function(TFunction const& f)	{operator=(f);	}
    constexpr Function(SelfType&& f)		{operator=(f);	}
    constexpr Function(SelfType const& f)	{operator=(f);	}
};

template<typename T> class Functor;

namespace Base {
	class FunctorID: private StaticValue<usize, 0> {
		constexpr static usize operator++(int) {return ++value;}

		template <typename T> friend class CTL::Functor;
	};
}

template<typename TReturn, typename... TArgs>
struct Functor<TReturn(TArgs...)>:
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

	using BaseType = typename Derived::Bases::FirstType;

	using
		SelfIdentified::SelfType
	;

	using
		Returnable::ReturnType
	;

	constexpr static bool PROCEDURE = Type::Different<ReturnType, void>;

	constexpr Functor(): id(0) {}

	constexpr Functor(BaseType const& f): BaseType(f), id(++count)			{}
	constexpr Functor(SelfType const& other): BaseType(other), id(other.id)	{}

	constexpr SelfType& operator=(BaseType const& f)		{BaseType::operator=(f); id = ++count; return *this;		}
	constexpr SelfType& operator=(SelfType const& other)	{BaseType::operator=(other); id = other.id; return *this;	}

	constexpr Nullable<ReturnType> evoke(TArgs... args) const requires (PROCEDURE)		{if (id) return BaseType::operator()(args...); return nullptr;	}
	constexpr Nullable<ReturnType> operator()(TArgs... args) const requires (PROCEDURE)	{return evoke(args...);											}

	constexpr void evoke(TArgs... args) const requires (!PROCEDURE)			{if (id) BaseType::operator()(args...);	}
	constexpr void operator()(TArgs... args) const requires (!PROCEDURE)	{evoke(args...);						}

	constexpr operator bool() const {return id;}

	constexpr bool operator==(SelfType const& other) const {return id == other.id;}

	constexpr Helper::PartialOrder operator<=>(SelfType const& other) const {return id <=> other.id;}

private:
	WrapperType			func;
	usize				id = 0;
	Base::FunctorID		count;
};

template<class T>						using Procedure	= Functor<void(T)>;
template<class T, class T2 = T const&>	using Operation	= Functor<T2(T)>;

template<class T = void>
using Signal	= Procedure<T>;

template<class T = void>
using Trigger	= Functor<bool(T)>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_FUNCTION_H
