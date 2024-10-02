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
			constexpr TReturn operator()(TArgs... args) const		{return invoke(forward<Args>(args)...);}
			constexpr virtual ~Function()							= default;
		};
	}

	template <typename TFunction, typename TReturn, typename... TArgs>
	class Function: Partial::Function<TReturn, TArgs...> {
		TFunction func;
		constexpr TReturn invoke(TArgs... args) const override {return func(forward<TArgs>(args)...);}
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

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Returnable::ReturnType
	;

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
		throw BadCallException("No function assigned!");
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

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_FUNCTION_H
