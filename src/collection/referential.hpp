#ifndef REFERENCE_HANDLER_H
#define REFERENCE_HANDLER_H

#include "conceptual.hpp"
#include <map>
#include <stdexcept>

namespace SmartPointer {
	template <typename T> concept Pointable = Type::Safe<T>;
	template <typename NEW, typename T> concept NewPointer = Type::Pointer<NEW> && Type::Convertible<T*, NEW>;

	namespace {
		using
			std::map,
			std::runtime_error
		;

		struct _pData{
			bool exists		= false;
			size_t count	= 0;
		};
		map<void*, _pData> _pointerDB;
	}

	#define ASSERT_STRONG	static_assert(!weak,	"It is forbidden to implicitly convert a strong pointer to a weak pointer!")
	#define ASSERT_WEAK		static_assert(weak,		"It is forbidden to implicitly convert a weak pointer to a strong pointer!")
	#define IF_STRONG	if constexpr(!weak)
	#define SameType	Pointer<T, weak, deleteIfLast>
	template <Pointable T, bool weak = false, bool deleteIfLast = true>
	class Pointer {
	public:
		Pointer() {}

		Pointer(Pointer<T, false,	false	>&& other)		{ASSERT_STRONG;	bind(other.ref);}
		Pointer(Pointer<T, false,	true	>&& other)		{ASSERT_STRONG;	bind(other.ref);}
		Pointer(Pointer<T, true,	false	>&& other)		{ASSERT_WEAK;	bind(other.ref);}
		Pointer(Pointer<T, true,	true	>&& other)		{ASSERT_WEAK;	bind(other.ref);}

		Pointer(const Pointer<T, false,	false	>& other)	{ASSERT_STRONG; bind(other.ref);}
		Pointer(const Pointer<T, false,	true	>& other)	{ASSERT_STRONG; bind(other.ref);}
		Pointer(const Pointer<T, true,	false	>& other)	{ASSERT_WEAK;	bind(other.ref);}
		Pointer(const Pointer<T, true,	true	>& other)	{ASSERT_WEAK;	bind(other.ref);}

		Pointer(const T*& obj) {bind(obj);}

		Pointer(T* obj) {bind(obj);}

		~Pointer() {unbind();}

		SameType& bind(T* obj) {
			/*if (obj == nullptr)
				throw std::runtime_error("Value must not be null!");*/
			unbind();
			if (obj == nullptr) return (*this);
			ref = obj;
			_pointerDB[(void*)obj].exists = true;
			IF_STRONG _pointerDB[(void*)obj].count++;
			return (*this);
		}

		// Destroy if Last Pointer.
		// I.E. Delete object if last Pointer to exist with it.
		SameType& unbind(bool dilp = deleteIfLast) {
			if (!exists()) return (*this);
			IF_STRONG {
				if ((_pointerDB[(void*)ref].count-1 < 1) && dilp) {
					$debug("Deleting reference...");
					return destroy();
				}
				$debug("Updating reference counter...");
				_pointerDB[(void*)ref].count--;
				$debugp("References: ");
				$debug(_pointerDB[(void*)ref].count);
			}
			ref = nullptr;
			return (*this);
		}

		SameType& destroy() {
			IF_STRONG {
				if (!exists()) return (*this);
				_pointerDB[(void*)ref] = {false, 0};
				delete ref;
				ref = nullptr;
			}
			return (*this);
		}

		bool exists() {
			if (ref == nullptr) return false;
			IF_STRONG	return (_pointerDB[(void*)ref].count != 0);
			else		return (_pointerDB[(void*)ref].exists);
		}

		inline bool operator()() {
			return exists();
		}

		template<Pointable NEW_T>
		inline Pointer<NEW_T, weak, deleteIfLast>	castedTo()	{return	(NEW_T*)getPointer();	}
		inline Pointer<T, true, deleteIfLast>		toWeak()	{return	getPointer();			}
		inline T*									raw()		{return	getPointer();			}

		explicit operator T*() const	{return ref;				};

		inline bool operator!()			{return	!exists();			}

		inline bool operator==(T* obj)	{return	ref == obj;			}
		inline bool operator!=(T* obj)	{return	!operator==(obj);	}
		inline bool operator<(T* obj)	{return	ref < obj;			}
		inline bool operator>(T* obj)	{return	operator<(obj);		}
		inline bool operator<=(T* obj)	{return	!operator>(obj);	}
		inline bool operator>=(T* obj)	{return	!operator<(obj);	}

		inline bool operator==(const SameType& other)	{return operator==(other.ref);	}
		inline bool operator!=(const SameType& other)	{return operator!=(other.ref);	}
		inline bool operator<(const SameType& other)	{return operator<(other.ref);	}
		inline bool operator>(const SameType& other)	{return operator>(other.ref);	}
		inline bool operator<=(const SameType& other)	{return operator<=(other.ref);	}
		inline bool operator>=(const SameType& other)	{return operator>=(other.ref);	}

		SameType& operator=(T* obj)								{bind(obj); return (*this);			}
		const SameType& operator=(T* obj) const					{bind(obj); return (*this);			}
		SameType& operator=(const SameType& other)				{bind(other.ref); return (*this);	}
		const SameType& operator=(const SameType& other) const	{bind(other.ref); return (*this);	}

		T* operator->()				{return getPointer();	}
		const T* operator->() const	{return getPointer();	}
		T& operator*()				{return getValue();		}
		const T& operator*() const	{return getValue();		}

	private:
		friend class Pointer<T,	false,	false	>;
		friend class Pointer<T,	false,	true	>;
		friend class Pointer<T,	true,	false	>;
		friend class Pointer<T,	true,	true	>;

		T* ref = nullptr;

		inline T* getPointer()	{
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (ref);
		}

		inline const T* getPointer() const	{
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (ref);
		}

		inline T& getValue() {
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (*ref);
		}

		inline const T& getValue() const {
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (*ref);
		}
	};
	#undef SameType
	#undef ASSERT_STRONG
	#undef ASSERT_WEAK

	template <Pointable T>
	using WeakPointer	= Pointer<T,	true,	false	>;

	template <Pointable T>
	using SafePointer	= Pointer<T,	false,	true	>;
}

#define $ptr SmartPointer::

#endif // REFERENCE_HANDLER_H
