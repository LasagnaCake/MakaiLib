#ifndef CTL_CONTAINER_POINTER_H
#define CTL_CONTAINER_POINTER_H

#include "../namespace.hpp"
#include "../templates.hpp"
#include "../typeinfo.hpp"
#include "../ctypes.hpp"
#include "../typetraits/traits.hpp"
#include "../algorithm/strconv.hpp"
#include "error.hpp"
#include "function.hpp"
#include "map.hpp"

CTL_NAMESPACE_BEGIN

template <typename T> concept Pointable = Type::Safe<T>;

namespace Base {
	struct ReferenceCounter {
		struct ReferenceData {
			bool	exists	= false;
			ssize	count	= 0;

			constexpr ReferenceData() = default;

			constexpr ReferenceData(bool const& exists, int64 const& count = 0): exists(exists), count(count) {}
		};

		typedef Map<void*, ReferenceData> ReferenceBank;

		inline static bool isBound(void* const& ptr) {return database.contains(ptr);}

	protected:
		inline static ReferenceBank database;
	};
}

#define CTL_PTR_ASSERT_STRONG_MOVE	static_assert(!WEAK,	"It is forbidden to move construct a weak pointer from a strong pointer!")
#define CTL_PTR_ASSERT_STRONG		static_assert(!WEAK,	"It is forbidden to implicitly convert a strong pointer to a weak pointer!")
#define CTL_PTR_ASSERT_WEAK			static_assert(WEAK,		"It is forbidden to implicitly convert a weak pointer to a strong pointer!")
#define CTL_PTR_IF_STRONG			if constexpr(!WEAK)
template <Pointable T, bool W>
class Pointer:
	private Base::ReferenceCounter,
	Derived<Base::ReferenceCounter>,
	Typed<T>,
	SelfIdentified<Pointer<T, W>>,
	Ordered {
public:
	constexpr static bool WEAK = W;

	using ReferenceCounter::isBound;

	using Typed				= ::CTL::Typed<T>;
	using SelfIdentified	= ::CTL::SelfIdentified<Pointer<T, WEAK>>;

	using
		typename Typed::DataType,
		typename Typed::ConstantType,
		typename Typed::ReferenceType,
		typename Typed::ConstReferenceType,
		typename Typed::PointerType,
		typename Typed::ConstPointerType
	;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Ordered::OrderType
	;

	template<bool NW>
	using NewPointerType = Pointer<DataType, NW>;

	using OtherType = NewPointerType<!WEAK>;

	using OperationType = Function<DataType(ConstReferenceType)>;

	constexpr Pointer() {}

	constexpr Pointer(NewPointerType<false>&& other)		{CTL_PTR_ASSERT_STRONG_MOVE;	bind(other.ref);}
	constexpr Pointer(NewPointerType<true>&& other)			{CTL_PTR_ASSERT_WEAK;			bind(other.ref);}

	constexpr Pointer(NewPointerType<false> const& other)	{								bind(other.ref);}
	constexpr Pointer(NewPointerType<true> const& other)	{CTL_PTR_ASSERT_WEAK;			bind(other.ref);}

	constexpr Pointer(PointerType const& obj) {bind(obj);}

	constexpr ~Pointer() {unbind();}

	constexpr ssize count() const {
		if (!exists()) return 0;
		return database[ref].count;
	}

	constexpr SelfType& bind(PointerType const& obj) {
		if (ref == obj) return (*this);
		unbind();
		if (obj == nullptr) return (*this);
		ref = obj;
		database[(void*)obj].exists = true;
		CTL_PTR_IF_STRONG database[(void*)obj].count++;
		return (*this);
	}

	constexpr SelfType& unbind() {
		if (!exists()) return (*this);
		CTL_PTR_IF_STRONG {
			if ((count()-1 < 1))
				return destroy();
			else database[(void*)ref].count--;
		}
		ref = nullptr;
		return (*this);
	}

	constexpr SelfType& destroy() {
		CTL_PTR_IF_STRONG {
			delete ref;
			release();
		}
		return (*this);
	}

	constexpr SelfType& release() {
		CTL_PTR_IF_STRONG {
			if (!exists()) return (*this);
			database[(void*)ref].exists	= false;
			database[(void*)ref].count	= 0;
			ref = nullptr;
		}
		return (*this);
	}

	constexpr bool exists() const {
		//if (!ref)			return false;
		CTL_PTR_IF_STRONG	return ref && (database[(void*)ref].count > 0);
		// BUG: returns true even if exists is set to false (because it is somehow set to true???)
		else				return ref && (database[(void*)ref].exists);
	}

	constexpr ssize unique() const	{return count() == 1;}

	constexpr bool operator()() const	{return exists();}

	constexpr PointerType operator&()				{return getPointer();}
	constexpr PointerType const operator&() const	{return getPointer();}

	template<Pointable TNew>
	constexpr Pointer<TNew, WEAK>		castedTo() const	{return	(TNew*)getPointer();	}
	constexpr Pointer<DataType, true>	asWeak() const		{return	getPointer();			}
	constexpr PointerType				raw() const			{return	getPointer();			}
	//constexpr ConstPointerType		raw() const			{return	getPointer();			}

	constexpr explicit operator PointerType const() const	{return getPointer();		}
	constexpr explicit operator PointerType()				{return getPointer();		}

	constexpr operator bool() const	{return exists();	}

	constexpr Pointer& modify(OperationType const& op)		{T& ref = *getPointer(); ref = op(ref); return (*this);	}
	constexpr Pointer& operator()(OperationType const& op)	{return modify(op);										}

	constexpr bool operator!() const	{return	!exists();			}

	constexpr bool operator==(PointerType const& obj) const			{return	ref == obj;			}
	constexpr OrderType operator<=>(PointerType const& obj) const	{return	ref <=> obj;		}

	constexpr bool operator==(SelfType const& other) const			{return ref == other.ref;	}
	constexpr OrderType operator<=>(SelfType const& other) const	{return ref <=> other.ref;	}

	constexpr SelfType& operator=(PointerType const& obj)	{bind(obj); return (*this);					}
	constexpr SelfType& operator=(SelfType const& other)	{bind(other.ref); return (*this);			}
//	constexpr SelfType& operator=(DataType const& v) const	{*getPointer() = v; return (*this);			}
//	constexpr SelfType& operator=(DataType const& v) const	{if (exists()) *ref = v; return (*this);	}

	constexpr ReferenceType value() const {
		if (!exists()) nullPointerError();
		return (*ref);
	}

	constexpr explicit operator DataType() const	{return value();		}

	constexpr PointerType operator->()				{return getPointer();	}
	constexpr PointerType const operator->() const	{return getPointer();	}
	constexpr ReferenceType operator*() const		{return value();		}

private:
	friend SelfType;
	friend OtherType;

	using ReferenceCounter::database;

	PointerType ref = nullptr;

	constexpr PointerType getPointer()	{
		if (!exists()) nullPointerError();
		return (ref);
	}

	constexpr PointerType getPointer() const	{
		if (!exists()) nullPointerError();
		return (ref);
	}

	[[noreturn]]
	inline void nullPointerError() const {
		throw Error::NullPointer(
			toString("Pointer reference of type '", TypeInfo<T>::name(), "' does not exist!"),
			__FILE__,
			"unspecified",
			"Pointer",
			"Pointer might be null or nonexistent."
		);
	}
};
#undef CTL_PTR_ASSERT_STRONG_MOVE
#undef CTL_PTR_ASSERT_STRONG
#undef CTL_PTR_ASSERT_WEAK
#undef CTL_PTR_IF_STRONG

template <Pointable T>
using WeakPointer	= Pointer<T, true>;

template <Pointable T>
using StrongPointer	= Pointer<T, false>;

template<Pointable T>
using Instance	= StrongPointer<T>;

template<Pointable T>
using Handle	= WeakPointer<T>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_POINTER_H
