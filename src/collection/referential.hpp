#ifndef REFERENCE_HANDLER_H
#define REFERENCE_HANDLER_H

#include "helper.hpp"
#include "conceptual.hpp"
#include "errors.hpp"
#include "types.hpp"
#include "definitions.hpp"
#include <map>
#include <stdexcept>
#include <functional>

namespace SmartPointer {
	template <typename T> concept Pointable = Type::Safe<T>;

	namespace {
		struct _pData{
			bool exists	= false;
			int64 count	= 0;
		};

		HashMap<void*, _pData> _pointerDB;
	}

	#define ASSERT_STRONG	static_assert(!weak,	"It is forbidden to implicitly convert a strong pointer to a weak pointer!")
	#define ASSERT_WEAK		static_assert(weak,		"It is forbidden to implicitly convert a weak pointer to a strong pointer!")
	#define IF_STRONG	if constexpr(!weak)
	template <Pointable T, bool weak = false>
	class Pointer {
	public:
		typedef T DataType;

		constexpr Pointer() {}

		constexpr Pointer(Pointer<T, false>&& other)		{ASSERT_STRONG;	bind(other.ref);}
		constexpr Pointer(Pointer<T, true>&& other)			{ASSERT_WEAK;	bind(other.ref);}

		constexpr Pointer(Pointer<T, false> const& other)	{ASSERT_STRONG; bind(other.ref);}
		constexpr Pointer(Pointer<T, true> const& other)	{ASSERT_WEAK;	bind(other.ref);}

		constexpr Pointer(T* const& obj)					{bind(obj);}

		constexpr ~Pointer() {unbind();}

		constexpr Pointer& bind(T* const& obj) {
			unbind();
			if (obj == nullptr) return (*this);
			ref = obj;
			_pointerDB[(void*)obj].exists = true;
			IF_STRONG _pointerDB[(void*)obj].count++;
			return (*this);
		}

		constexpr Pointer& unbind() {
			if (!exists()) return (*this);
			IF_STRONG {
				if ((_pointerDB[(void*)ref].count-1 < 1)) {
					DEBUGLN("Deleting reference...");
					return destroy();
				}
				DEBUGLN("Updating reference counter...");
				_pointerDB[(void*)ref].count--;
				DEBUGLN("References: ", _pointerDB[(void*)ref].count);
			}
			ref = nullptr;
			return (*this);
		}

		constexpr Pointer& destroy() {
			IF_STRONG {
				if (!exists()) return (*this);
				_pointerDB[(void*)ref] = {false, 0};
				delete ref;
				ref = nullptr;
			}
			return (*this);
		}

		constexpr Pointer& modify(Operation<T> const& op) {
			if(exists())
				(*ref) = op(*ref);
			return *this;
		}

		constexpr Pointer& modify(T (*op)(T const&)) {
			if(exists())
				(*ref) = (*op)(*ref);
			return *this;
		}

		constexpr bool exists() {
			if (ref == nullptr) return false;
			IF_STRONG	return (_pointerDB[(void*)ref].count > 0);
			else		return (_pointerDB[(void*)ref].exists);
		}

		constexpr bool operator()()	{return exists();}

		constexpr Pointer& operator()(Operation<T> const& op)	{return modify(op);}
		constexpr Pointer& operator()(void (*op)(T const&))	{return modify(op);}

		constexpr T& operator[](size_t const& index)	{return getPointer()[index];}

		template<Pointable NEW_T>
		constexpr Pointer<NEW_T, weak>	castedTo()	{return	(NEW_T*)getPointer();	}
		constexpr Pointer<T, true>		asWeak()	{return	getPointer();			}
		constexpr T*					raw()		{return	getPointer();			}

		constexpr explicit operator T*() const	{return getPointer();		}
		constexpr explicit operator T*()		{return getPointer();		}
		constexpr operator bool() const			{return exists();			}

		constexpr bool operator!()					{return	!exists();			}
		constexpr bool operator==(T* const& obj)	{return	ref == obj;			}
		constexpr bool operator!=(T* const& obj)	{return	!operator==(obj);	}
		constexpr bool operator<(T* const& obj)		{return	ref < obj;			}
		constexpr bool operator>(T* const& obj)		{return	operator<(obj);		}
		constexpr bool operator<=(T* const& obj)	{return	!operator>(obj);	}
		constexpr bool operator>=(T* const& obj)	{return	!operator<(obj);	}

		constexpr bool operator==(Pointer<T, weak> const& other) const	{return operator==(other.ref);	}
		constexpr bool operator!=(Pointer<T, weak> const& other) const	{return operator!=(other.ref);	}
		constexpr bool operator<(Pointer<T, weak> const& other) const	{return operator<(other.ref);	}
		constexpr bool operator>(Pointer<T, weak> const& other) const	{return operator>(other.ref);	}
		constexpr bool operator<=(Pointer<T, weak> const& other) const	{return operator<=(other.ref);	}
		constexpr bool operator>=(Pointer<T, weak> const& other) const	{return operator>=(other.ref);	}

		constexpr Pointer& operator=(T* const& obj)					{bind(obj); return (*this);			}
		constexpr Pointer& operator=(Pointer<T, weak> const& other)	{bind(other.ref); return (*this);	}

		constexpr T* operator->()				{return getPointer();	}
		constexpr const T* operator->() const	{return getPointer();	}
		constexpr T& operator*()				{return getValue();		}
		constexpr const T& operator*() const	{return getValue();		}

	private:
		friend class Pointer<T,	false	>;
		friend class Pointer<T,	true	>;

		T* ref = nullptr;

		constexpr T* getPointer()	{
			if (!exists())
				throw Error::NullPointer(
					"Pointer reference does not exist!",
					__FILE__,
					"",
					"SmartPointer",
					"Pointer might be null or nonexistent"
				);
			return (ref);
		}

		constexpr const T* getPointer() const	{
			if (!exists())
				throw Error::NullPointer(
					"Pointer reference does not exist!",
					__FILE__,
					"",
					"SmartPointer",
					"Pointer might be null or nonexistent"
				);
			return (ref);
		}

		constexpr T& getValue() {
			if (!exists())
				throw Error::NullPointer(
					"Pointer reference does not exist!",
					__FILE__,
					"",
					"SmartPointer",
					"Pointer might be null or nonexistent"
				);
			return (*ref);
		}

		constexpr const T& getValue() const {
			if (!exists())
				throw Error::NullPointer(
					"Pointer reference does not exist!",
					__FILE__,
					"",
					"SmartPointer",
					"Pointer might be null or nonexistent"
				);
			return (*ref);
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

#endif // REFERENCE_HANDLER_H
