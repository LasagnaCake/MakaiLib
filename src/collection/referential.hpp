#ifndef REFERENCE_HANDLER_H
#define REFERENCE_HANDLER_H

#include "helper.hpp"
#include "conceptual.hpp"
#include "errors.hpp"
#include "types.hpp"
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
		Pointer() {}

		Pointer(Pointer<T, false>&& other)		{ASSERT_STRONG;	bind(other.ref);}
		Pointer(Pointer<T, true>&& other)		{ASSERT_WEAK;	bind(other.ref);}

		Pointer(const Pointer<T, false>& other)	{ASSERT_STRONG; bind(other.ref);}
		Pointer(const Pointer<T, true>& other)	{ASSERT_WEAK;	bind(other.ref);}

		Pointer(const T* const& obj)	{bind(obj);}
		Pointer(T* const& obj)			{bind(obj);}

		~Pointer() {unbind();}

		Pointer& bind(T* obj) {
			unbind();
			if (obj == nullptr) return (*this);
			ref = obj;
			_pointerDB[(void*)obj].exists = true;
			IF_STRONG _pointerDB[(void*)obj].count++;
			return (*this);
		}

		Pointer& unbind() {
			if (!exists()) return (*this);
			IF_STRONG {
				if ((_pointerDB[(void*)ref].count-1 < 1)) {
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

		Pointer& destroy() {
			IF_STRONG {
				if (!exists()) return (*this);
				_pointerDB[(void*)ref] = {false, 0};
				delete ref;
				ref = nullptr;
			}
			return (*this);
		}

		Pointer& modify(Operation<T> op) {
			if(exists())
				(*ref) = op(*ref);
			return *this;
		}

		Pointer& modify(T (*op)(T const&)) {
			if(exists())
				(*ref) = (*op)(*ref);
			return *this;
		}

		bool exists() {
			if (ref == nullptr) return false;
			IF_STRONG	return (_pointerDB[(void*)ref].count > 0);
			else		return (_pointerDB[(void*)ref].exists);
		}

		inline bool operator()()	{return exists();}

		inline Pointer& operator()(Operation<T> op)			{return modify(op);}
		inline Pointer& operator()(void (*op)(T const&))	{return modify(op);}

		inline T& operator[](size_t index)	{return getPointer()[index];}

		template<Pointable NEW_T>
		inline Pointer<NEW_T, weak>	castedTo()	{return	(NEW_T*)getPointer();	}
		inline Pointer<T, true>		asWeak()	{return	getPointer();			}
		inline T*					raw()		{return	getPointer();			}

		explicit operator T*() const	{return getPointer();		}
		operator bool() const			{return exists();			}

		inline bool operator!()			{return	!exists();			}
		inline bool operator==(T* obj)	{return	ref == obj;			}
		inline bool operator!=(T* obj)	{return	!operator==(obj);	}
		inline bool operator<(T* obj)	{return	ref < obj;			}
		inline bool operator>(T* obj)	{return	operator<(obj);		}
		inline bool operator<=(T* obj)	{return	!operator>(obj);	}
		inline bool operator>=(T* obj)	{return	!operator<(obj);	}

		inline bool operator==(const Pointer<T, weak>& other)	{return operator==(other.ref);	}
		inline bool operator!=(const Pointer<T, weak>& other)	{return operator!=(other.ref);	}
		inline bool operator<(const Pointer<T, weak>& other)	{return operator<(other.ref);	}
		inline bool operator>(const Pointer<T, weak>& other)	{return operator>(other.ref);	}
		inline bool operator<=(const Pointer<T, weak>& other)	{return operator<=(other.ref);	}
		inline bool operator>=(const Pointer<T, weak>& other)	{return operator>=(other.ref);	}

		Pointer& operator=(T* obj)										{bind(obj); return (*this);			}
		const Pointer& operator=(T* obj) const							{bind(obj); return (*this);			}
		Pointer& operator=(T& obj)										{bind(&obj); return (*this);		}
		const Pointer& operator=(T& obj) const							{bind(&obj); return (*this);		}
		Pointer& operator=(const Pointer<T, weak>& other)				{bind(other.ref); return (*this);	}
		const Pointer& operator=(const Pointer<T, weak>& other) const	{bind(other.ref); return (*this);	}

		T* operator->()				{return getPointer();	}
		const T* operator->() const	{return getPointer();	}
		T& operator*()				{return getValue();		}
		const T& operator*() const	{return getValue();		}

	private:
		friend class Pointer<T,	false	>;
		friend class Pointer<T,	true	>;

		T* ref = nullptr;

		inline T* getPointer()	{
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

		inline const T* getPointer() const	{
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

		inline T& getValue() {
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

		inline const T& getValue() const {
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
