#ifndef CTL_CONTAINER_VIEW_H
#define CTL_CONTAINER_VIEW_H

#include "../ctypes.hpp"
#include "../typetraits/traits.hpp"
#include "../templates.hpp"
#include "../namespace.hpp"

CTL_NAMESPACE_BEGIN

template <typename T> concept Viewable = Type::Safe<T>;

template<Viewable T>
class View: Typed<T>, SelfIdentified<View<T>> {
public:
	using Typed				= Typed<T>;
	using SelfIdentified	= SelfIdentified<View<T>>;

	using
		typename Typed::DataType,
		typename Typed::ReferenceType,
		typename Typed::ConstReferenceType
	;

	using
		typename SelfIdentified::SelfType
	;

	constexpr DataView(ReferenceType _data):	data(_data)			{}
	constexpr DataView(SelfType const& other):	data(other.data)	{}
	constexpr DataView(SelfType&& other):		data(other.data)	{}

	constexpr SelfType& operator=(T const& val) {data = val; return (*this);}

	constexpr operator DataType() const	{return value();	}
	constexpr operator DataType()		{return value();	}
	constexpr DataType value() const	{return data;		}
	constexpr DataType value() 			{return data;		}

protected:
	constexpr ReferenceType view()				{return data;}
	constexpr ConstReferenceType view() const	{return data;}

private:
	T& data;
};

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_VIEW_H
