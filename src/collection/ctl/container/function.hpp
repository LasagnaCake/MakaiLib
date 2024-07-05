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

public:
	constexpr ReturnType invoke(TArgs... args) const {
        return func->invoke(forward<TArgs>(args)...);
    }

    constexpr ReturnType operator()(TArgs... args) const {
        return invoke(forward<TArgs>(args)...);
    }

    constexpr ~Function() {destroy();}

	constexpr SelfType& destroy() {
		if (func) delete func;
		func = nullptr;
		return *this;
	}

	template<typename TFunction>
	constexpr SelfType& create(TFunction&& f)		{if (!func) func = makeCallable(f); return *this;					}
	template<typename TFunction>
	constexpr SelfType& create(TFunction const& f)	{if (!func) func = makeCallable(f); return *this;					}
	constexpr SelfType& create(SelfType&& f)		{if (!func) {func = move(f.func); f.func = nullptr;} return *this;	}
	constexpr SelfType& create(SelfType const& f)	{return create(f.func->func);										}

	template<typename TFunction>
	constexpr SelfType& make(TFunction&& f)			{destroy(); return create(f);	}
	template<typename TFunction>
	constexpr SelfType& make(TFunction const& f)	{destroy(); return create(f);	}
	constexpr SelfType& make(SelfType&& f)			{destroy(); return create(f);	}
	constexpr SelfType& make(SelfType const& f)		{destroy(); return create(f);	}

	template<typename TFunction>
    constexpr SelfType& operator=(TFunction&& f)		{make(f);	}
    template<typename TFunction>
    constexpr SelfType& operator=(TFunction const& f)	{make(f);	}
    constexpr SelfType& operator=(SelfType&& f)			{make(f);	}
    constexpr SelfType& operator=(SelfType const& f)	{make(f);	}


    template<typename TFunction>
    constexpr Function(TFunction&& f)		{create(f);	}
    template<typename TFunction>
    constexpr Function(TFunction const& f)	{create(f);	}
    constexpr Function(SelfType&& f)		{create(f);	}
    constexpr Function(SelfType const& f)	{create(f);	}
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

	constexpr Functor& operator=(WrapperType const& f)	{func = f; id = ++count; return *this;				}
	constexpr Functor& operator=(Functor const& other)	{func = other.func; id = other.id; return *this;	}

	constexpr Nullable<ReturnType> evoke(Args... args) const requires Type::Different<F, void>		{if (id) return func(args...); return nullptr;	}
	constexpr Nullable<ReturnType> operator()(Args... args) const requires Type::Different<F, void>	{return evoke(args...);							}

	constexpr void evoke(Args... args) const requires Type::Equal<F, void>					{if (id) func(args...);	}
	constexpr void operator()(Args... args) const requires Type::Equal<F, void>				{evoke(args...);		}

	constexpr operator bool() const {return id;}

	constexpr bool operator==(Functor const& other) const {return id == other.id;}

	constexpr Helper::PartialOrder operator<=>(Functor const& other) const {return id <=> other.id;}

private:
	WrapperType				func;
	size_t					id		= 0;
	inline static size_t	count	= 0;
};

#endif // CTL_CONTAINER_FUNCTION_H
