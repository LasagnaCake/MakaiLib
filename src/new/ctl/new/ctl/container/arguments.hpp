#ifndef CTL_COMPAT_ARGUMENTS_H
#define CTL_COMPAT_ARGUMENTS_H

#include "../namespace.hpp"
#include "../ctypes.hpp"
#include "../templates.hpp"
#include "../cpperror.hpp"

CTL_NAMESPACE_BEGIN

template <class T>
struct Arguments:
	Typed<T>,
	SelfIdentified<Arguments<T>> {
	using Typed				= CTL::Typed<T>;
	using SelfIdentified	= CTL::SelfIdentified<Arguments<T>>;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Typed::DataType,
		typename Typed::PointerType,
		typename Typed::ConstReferenceType,
		typename Typed::ReferenceType,
		typename Typed::ConstPointerType
	;
	constexpr ~Arguments() noexcept {delete[] start;}

	constexpr Arguments()noexcept : start(nullptr), length(0) {}

	template<usize N>
	constexpr Arguments(Decay::AsType<DataType[N]> const& data):
	start(new DataType[N]), length(N) {
		MX::memcpy(start, data, N);
	}

	template<class... Args>
	constexpr Arguments(DataType const& first, Args const&... rest):
		start(new DataType[sizeof...(Args) + 1]{first, rest...}), length(sizeof...(Args) + 1) {}

	constexpr ReferenceType operator[](ssize index) {
		while (index < 0)
			index += length;
		if (index > (length-1))
			throw OutOfBoundsException("Argument index is out of bounds!");
		return start[index];
	}

	constexpr ConstReferenceType operator[](ssize index) const {
		while (index < 0)
			index += length;
		if (index > (length-1))
			throw OutOfBoundsException("Argument index is out of bounds!");
		return start[index];
	}

	Arguments& operator=(Arguments&&)		= delete;
    Arguments& operator=(Arguments const&)	= delete;

    constexpr ConstPointerType begin() const noexcept	{return start;			}
    constexpr ConstPointerType end() const noexcept		{return start + length;	}

    constexpr usize size() const noexcept	{return length;			}
    constexpr usize empty() const noexcept	{return length == 0;	}

private:
	PointerType		start;
	usize			length;
};

template <class TData>
struct ListInitializable {
	typedef Arguments<TData>	ArgumentListType;
};

CTL_NAMESPACE_END

#endif // CTL_COMPAT_ARGUMENTS_H
