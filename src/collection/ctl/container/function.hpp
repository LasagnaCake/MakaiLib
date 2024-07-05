#ifndef CTL_CONTAINER_FUNCTION_H
#define CTL_CONTAINER_FUNCTION_H

#include "../templates.hpp"
#include "../typetraits/traits.hpp"
#include "nullable.hpp"

// Function implementation based off of: https://stackoverflow.com/a/61191826

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
class Function<TReturn(TArgs...)>:
	Typed<TReturn>,
	SelfIdentified<Function<TReturn, TArgs..>>,
	Returnable<TReturn>,
	Argumented<TArgs...>,
	Functional<TReturn, TArgs...> {
private:
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
		throw Error::InvalidAction(
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

template<typename TReturn, typename... TArgs>
class Functor<TReturn(TArgs...)>:
	Typed<TReturn>,
	SelfIdentified<Functor<TReturn, TArgs..>>,
	Returnable<TReturn>,
	Argumented<TArgs...>,
	Functional<TReturn, TArgs...> {
private:
public:
	typedef Function<FunctionType>	WrapperType;

	constexpr Functor(): id(0) {}

	constexpr Functor(WrapperType const& f): func(f), id(++count)			{}
	constexpr Functor(Functor const& other): func(other.func), id(other.id)	{}

	constexpr SelfType& operator=(WrapperType const& f)		{func = f; id = ++count; return *this;				}
	constexpr SelfType& operator=(SelfType const& other)	{func = other.func; id = other.id; return *this;	}

	constexpr Nullable<ReturnType> evoke(TArgs... args) const requires Type::Different<F, void>			{if (id) return func(args...); return nullptr;	}
	constexpr Nullable<ReturnType> operator()(TArgs... args) const requires Type::Different<F, void>	{return evoke(args...);							}

	constexpr void evoke(TArgs... args) const requires Type::Equal<F, void>			{if (id) func(args...);	}
	constexpr void operator()(TArgs... args) const requires Type::Equal<F, void>	{evoke(args...);		}

	constexpr operator bool() const {return id;}

	constexpr bool operator==(Functor const& other) const {return id == other.id;}

	constexpr Helper::PartialOrder operator<=>(Functor const& other) const {return id <=> other.id;}

private:
	WrapperType				func;
	size_t					id		= 0;
	inline static size_t	count	= 0;
};

#endif // CTL_CONTAINER_FUNCTION_H
