#ifndef REFERENCE_HANDLER_H
#define REFERENCE_HANDLER_H

#include "conceptual.hpp"
#include <map>
#include <stdexcept>

namespace Reference {
	template <typename T> concept Pointable = Type::Safe<T>;

	namespace {
		using
			std::map,
			std::runtime_error;
	}

	template <Pointable T>
	using PointerDB = map<T*, size_t>;

	template<Pointable T>
	PointerDB<T>& getPointerDB() {
		static PointerDB<T> db;
		return db;
	}

	template <Pointable T, bool deleteOnLast = true>
	class Pointer {
	private:
		typedef Pointer<T, deleteOnLast> SameType;
	public:
		Pointer() {}

		Pointer(Pointer<T, deleteOnLast>&& other) {bind(other.ref);}

		Pointer(T* obj) {bind(obj);}

		~Pointer() {unbind();}

		SameType& bind(T* obj) {
			static_assert(obj != nullptr, "Value must not be null!");
			unbind();
			ref = obj;
			getPointerDB<T>()[obj]++;
			return (*this);
		}

		// Destroy if Last Pointer.
		// I.E. Delete object if last Pointer to exist with it.
		SameType& unbind(bool dilp = deleteOnLast) {
			if (exists()) return (*this);
			if (getPointerDB<T>()[ref]-1 == 0 && dilp)
				destroy();
			else
				getPointerDB<T>()[ref]--;
			ref = nullptr;
			return (*this);
		}

		SameType& destroy() {
			if (exists()) return (*this);
			getPointerDB<T>()[ref] = 0;
			delete ref;
			ref = nullptr;
			return (*this);
		}

		bool exists() {
			return (
				ref != nullptr
			&&	getPointerDB<T>()[ref] != 0
			);
		}

		SameType& operator=(T* obj)								{bind(obj); return (*this);}
		const SameType& operator=(T* obj) const					{bind(obj); return (*this);}
		SameType& operator=(SameType other)						{bind(other.ref); return (*this);}
		const SameType& operator=(SameType other) const			{bind(other.ref); return (*this);}
		SameType& operator=(const SameType& other)				{bind(other.ref); return (*this);}
		const SameType& operator=(const SameType& other) const	{bind(other.ref); return (*this);}

		T* operator->()				{return getPointer();}
		const T* operator->() const	{return getPointer();}
		T& operator*()				{return getValue();}
		const T& operator*() const	{return getValue();}

	private:
		friend class Pointer<T, deleteOnLast>;

		T* ref = nullptr;

		inline T* getPointer()	{
			static_assert(ref != nullptr, "Pointer reference does not exist!");
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (ref);
		}

		inline const T* getPointer() const	{
			static_assert(ref != nullptr, "Pointer reference does not exist!");
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (ref);
		}

		inline T& getValue() {
			static_assert(ref != nullptr, "Pointer reference does not exist!");
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (*ref);
		}

		inline const T& getValue() const {
			static_assert(ref != nullptr, "Pointer reference does not exist!");
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (*ref);
		}
	};

	template <Pointable T>
	using WeakPointer	= Pointer<T,	false>;
}

#define $ptr Reference::

#endif // REFERENCE_HANDLER_H
