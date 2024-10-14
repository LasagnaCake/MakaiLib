#ifndef CTL_CONTAINER_FUNCTION_H
#define CTL_CONTAINER_FUNCTION_H

#include "../templates.hpp"
#include "../typetraits/traits.hpp"
#include "../cpperror.hpp"

// Function implementation based off of: https://stackoverflow.com/a/61191826

CTL_NAMESPACE_BEGIN

namespace Impl {
	namespace Partial {
		template <typename TReturn, typename... TArgs>
		struct Function {
			constexpr virtual TReturn invoke(TArgs...) const		= 0;
			constexpr TReturn operator()(TArgs... args) const		{return invoke(args...);}
			constexpr virtual ~Function()							= default;
		};
	}

	template <typename TFunction, typename TReturn, typename... TArgs>
	struct Function: Partial::Function<TReturn, TArgs...> {
		TFunction func;
		constexpr TReturn invoke(TArgs... args) const override {return func(args...);}
		constexpr Function(TFunction&& func): func(func) {}

		constexpr ~Function() {}
	};
}

template<typename T> struct Function;

template<typename TReturn, typename... TArgs>
struct Function<TReturn(TArgs...)>:
	SelfIdentified<Function<TReturn(TArgs...)>>,
	Returnable<TReturn>,
	Argumented<TArgs...>,
	Functional<TReturn(TArgs...)> {
public:
	using Functional		= ::CTL::Functional<TReturn(TArgs...)>;
	using SelfIdentified	= ::CTL::SelfIdentified<Function<TReturn(TArgs...)>>;
	using Returnable		= ::CTL::Returnable<TReturn>;
	using Argumented		= ::CTL::Argumented<TArgs...>;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Functional::FunctionType
	;

	using
		typename Returnable::ReturnType
	;

private:
	Impl::Partial::Function<ReturnType, TArgs...>* func{nullptr};

	template<typename TFunction>
	using Callable = Impl::Function<Decay::AsArgument<TFunction>, TReturn, TArgs...>;

	template<typename TFunction>
	constexpr static Callable<TFunction>*
	makeCallable(TFunction&& f) {
		return ::new Callable<TFunction>(CTL::move(f));
	}

	template<typename TFunction>
	constexpr static Callable<TFunction>*
	makeCallable(TFunction const& f) {
		return ::new Callable<TFunction>(f);
	}

	constexpr void assign(SelfType const& other) {
		if (!other.func) return;
		func = makeCallable(((Callable<FunctionType>*)other.func)->func);
	}

	constexpr void destroy() {
		if (func) delete func;
		func = nullptr;
	}

	[[noreturn]] static void badCallError() {
		throw BadCallException("No function assigned!");
	}

public:
	constexpr ReturnType invoke(TArgs... args) const {
		if (!func) badCallError();
        return func->invoke(args...);
    }

    constexpr ReturnType operator()(TArgs... args) const {
        return invoke(args...);
    }

    constexpr ~Function() {destroy();}

	template<typename TFunction>
    constexpr SelfType& operator=(TFunction&& f)
	requires (!Type::Derived<TFunction, SelfType>)		{destroy(); func = makeCallable(f); return *this;					}
    template<typename TFunction>
    constexpr SelfType& operator=(TFunction const& f)
	requires (!Type::Derived<TFunction, SelfType>)		{destroy(); func = makeCallable(f); return *this;					}
    constexpr SelfType& operator=(SelfType&& f)			{destroy(); func = move(f.func); f.func = nullptr; return *this;	}
    constexpr SelfType& operator=(SelfType const& f)	{destroy(); assign(f); return *this;								}

    constexpr Function() {}

    template<typename TFunction>
    constexpr Function(TFunction&& f)		{operator=(f);	}
    template<typename TFunction>
    constexpr Function(TFunction const& f)	{operator=(f);	}
    constexpr Function(SelfType&& f)		{operator=(f);	}
    constexpr Function(SelfType const& f)	{operator=(f);	}
};

template<typename... Args>
using Procedure		= Function<void(Args...)>;

template<typename T, typename T2 = T const&>
using Operation		= Function<T(T2)>;

template<typename T>
using Acquisition	= Function<T()>;

template<typename... Args>
using Signal	= Procedure<Args...>;

template<typename T = void>
using Trigger	= Acquisition<bool>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_FUNCTION_H
