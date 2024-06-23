#ifndef REFERENCE_HANDLER_H
#define REFERENCE_HANDLER_H

#include "helper.hpp"
#include "conceptual.hpp"
#include "errors.hpp"
#include "ctl/ctypes.hpp"
#include "definitions.hpp"

namespace SmartPointer {
	template <typename T> concept Pointable = Type::Safe<T>;

	struct ReferenceCounter {
		struct ReferenceData {
			bool	exists	= false;
			int64	count	= 0;
		};

		typedef HashMap<void*, ReferenceData> ReferenceBank;

		inline static bool isBound(void* const& ptr) {return database.contains(ptr);}

	protected:
		inline static ReferenceBank database;
	};

	#define ASSERT_STRONG	static_assert(!weak,	"It is forbidden to implicitly convert a strong pointer to a weak pointer!")
	#define ASSERT_WEAK		static_assert(weak,		"It is forbidden to implicitly convert a weak pointer to a strong pointer!")
	#define IF_STRONG	if constexpr(!weak)
	template <Pointable T, bool weak = false>
	class Pointer: private ReferenceCounter {
	public:
		typedef T DataType;

		using ReferenceCounter::isBound;

		constexpr Pointer() {}

		constexpr Pointer(Pointer<T, false>&& other)		{ASSERT_STRONG;	bind(other.ref);}
		constexpr Pointer(Pointer<T, true>&& other)			{ASSERT_WEAK;	bind(other.ref);}

		constexpr Pointer(Pointer<T, false> const& other)	{				bind(other.ref);}
		constexpr Pointer(Pointer<T, true> const& other)	{ASSERT_WEAK;	bind(other.ref);}

		constexpr Pointer(T* const& obj)					{bind(obj);}

		constexpr ~Pointer() {unbind();}

		constexpr size_t count() {
			if (!exists()) return 0;
			return database[ref].count;
		}

		constexpr Pointer& bind(T* const& obj) {
			if (ref == obj) return (*this);
			unbind();
			if (obj == nullptr) return (*this);
			//DEBUGLN("Binding reference...");
			ref = obj;
			database[(void*)obj].exists = true;
			IF_STRONG {
				//DEBUGLN("Updating reference counter...");
				database[(void*)obj].count++;
				//DEBUGLN("References: ", database[(void*)ref].count);
			}
			return (*this);
		}

		constexpr Pointer& unbind() {
			if (!exists()) return (*this);
			//DEBUGLN("Unbinding reference...");
			IF_STRONG {
				if ((database[(void*)ref].count-1 < 1))
					return destroy();
				//DEBUGLN("Updating reference counter...");
				database[(void*)ref].count--;
				//DEBUGLN("References: ", database[(void*)ref].count);
			}
			ref = nullptr;
			return (*this);
		}

		constexpr Pointer& destroy() {
			IF_STRONG {
				//DEBUGLN("Deleting reference...");
				if (!exists()) return (*this);
				database[(void*)ref] = {false, 0};
				delete ref;
				ref = nullptr;
				//DEBUGLN("Reference deleted!");
			}
			return (*this);
		}

		constexpr Pointer& release() {
			IF_STRONG {
				//DEBUGLN("Releasing reference...");
				if (!exists()) return (*this);
				database[(void*)ref] = {false, 0};
				ref = nullptr;
				//DEBUGLN("Reference released!");
			}
			return (*this);
		}

		constexpr Pointer& modify(Operation<T const&> const& op) const {
			if(exists())
				mutate(op(*ref));
			return *this;
		}

		constexpr Pointer& modify(T (*const& op)(T const&)) const {
			if(exists())
				mutate((*op)(*ref));
			return *this;
		}

		constexpr bool exists() const {
			if (ref == nullptr) return false;
			IF_STRONG	return (database[(void*)ref].count > 0);
			else		return (database[(void*)ref].exists);
		}

		constexpr bool operator()() const	{return exists();}

		constexpr Pointer& operator()(Operation<T const&> const& op) const	{return modify(op);}
		constexpr Pointer& operator()(void (*op)(T const&)) const			{return modify(op);}

		constexpr T& operator[](size_t const& index) const	{return getPointer()[index];}

		constexpr T* operator&()				{return getPointer();}
		constexpr T* const operator&() const	{return getPointer();}

		template<Pointable TNew>
		constexpr Pointer<TNew, weak>	castedTo() const	{return	(TNew*)getPointer();	}
		constexpr Pointer<T, true>		asWeak() const		{return	getPointer();			}
		constexpr T*					raw() const			{return	getPointer();			}

		constexpr explicit operator T* const() const	{return getPointer();		}
		constexpr explicit operator T*()				{return getPointer();		}

		constexpr operator bool() const	{return exists();	}

		constexpr bool operator!() const	{return	!exists();			}

		constexpr bool operator==(T* const& obj) const					{return	ref == obj;			}
		constexpr Helper::PartialOrder operator<=>(T* const& obj) const	{return	ref <=> obj;		}

		constexpr bool operator==(Pointer<T, weak> const& other) const					{return ref == other.ref;	}
		constexpr Helper::PartialOrder operator<=>(Pointer<T, weak> const& other) const	{return ref <=> other.ref;	}

		constexpr Pointer& operator=(T* const& obj)					{bind(obj); return (*this);					}
		constexpr Pointer& operator=(Pointer<T, weak> const& other)	{bind(other.ref); return (*this);			}
		constexpr Pointer& operator=(T const& v) const				{*getPointer() = v; return (*this);			}
	//	constexpr Pointer& operator=(T const& v) const				{if (exists()) *ref = v; return (*this);	}

		constexpr T& value() const {
			if (!exists()) nullPointerError();
			return (*ref);
		}

		constexpr explicit operator T() const	{return value();	}

		constexpr T* operator->()				{return getPointer();	}
		constexpr T* const operator->() const	{return getPointer();	}
		constexpr T& operator*() const			{return value();		}

	private:
		friend class Pointer<T,	false	>;
		friend class Pointer<T,	true	>;

		using ReferenceCounter::database;

		T* ref = nullptr;

		constexpr void mutate(T const& v) const requires (!Type::Constant<T>)	{(*ref) = v;	}
		constexpr void mutate(T const& v) const requires (Type::Constant<T>)	{return;		}

		constexpr T* getPointer()	{
			if (!exists()) nullPointerError();
			return (ref);
		}

		constexpr T* const getPointer() const	{
			if (!exists()) nullPointerError();
			return (ref);
		}

		[[noreturn]] void nullPointerError() const {
			throw Error::NullPointer(
				toString("Pointer reference of type '", NAMEOF(typeid(T)), "' does not exist!"),
				__FILE__,
				"unspecified",
				"Pointer",
				"Pointer might be null or nonexistent."
			);
		}
	};
	#undef ASSERT_STRONG
	#undef ASSERT_WEAK
	#undef IF_STRONG

	template <Pointable T>
	using WeakPointer	= Pointer<T,	true>;

	template <Pointable T>
	using StrongPointer	= Pointer<T,	false>;
}

using SmartPointer::StrongPointer;
using SmartPointer::WeakPointer;

template<SmartPointer::Pointable T>
using Instance	= SmartPointer::StrongPointer<T>;

template<SmartPointer::Pointable T>
using Handle	= SmartPointer::WeakPointer<T>;

#endif // REFERENCE_HANDLER_H
