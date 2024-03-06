#ifndef TYPE_ITERATOR_H
#define TYPE_ITERATOR_H

#include <compare>

template<typename T, bool REVERSE = false>
class Iterator {
public:
	typedef T		DataType;
	typedef T const	ConstantType;

	typedef DataType*		PointerType;
	typedef DataType&		ReferenceType;
	typedef ConstantType*	ConstPointerType;
	typedef ConstantType&	ConstReferenceType;

	constexpr Iterator() {}

	constexpr Iterator(PointerType const& value): iterand(value)		{}
	constexpr Iterator(PointerType&& value): iterand(std::move(value))	{}

	constexpr Iterator(Iterator const& other): iterand(other.iterand)		{}
	constexpr Iterator(Iterator&& other): iterand(std::move(other.iterand))	{}

	constexpr ReferenceType operator*()	{return *iterand;	}
	constexpr PointerType operator->()	{return iterand;	}

	constexpr ReferenceType operator*() const	{return *iterand;	}
	constexpr PointerType operator->() const	{return iterand;	}

	Iterator& operator++()		{step(); return *this;							}
	Iterator operator++(int)	{Iterator copy = *this; ++(*this); return copy;	}

	constexpr operator PointerType() const			{return iterand;	}
	constexpr operator ReferenceType()				{return *iterand;	}
	constexpr operator ConstReferenceType() const	{return *iterand;	}

	constexpr bool operator==(Iterator const& other) const						{return iterand == other.iterand;	}
	constexpr std::partial_ordering operator<=>(Iterator const& other) const	{return compare(other.iterand);		}

private:
	constexpr void step() {
		if constexpr (REVERSE)	--iterand;
		else					++iterand;
	}

	constexpr void compare(PointerType const& other) {
		if constexpr (REVERSE)	return other <=> iterand;
		else					return iterand <=> other;
	}

	PointerType iterand = nullptr;
};

template<typename T>
using ReverseIterator = Iterator<T, true>;

#endif // TYPE_ITERATOR_H
