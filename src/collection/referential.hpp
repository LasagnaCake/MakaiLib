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

	template <Pointable T, bool unbindOnDelete = false, bool dilmsState = true>
	class Pointer {
	public:
		Pointer() {}

		Pointer(const Pointer<T>& other) {bind(other.ref);}

		Pointer(T* obj) {bind(obj);}

		~Pointer() {
			if constexpr(unbindOnDelete)
				unbind();
			else
				destroy();
		}

		void bind(T* obj) {
			static_assert(obj != nullptr, "Value must not be null!");
			unbind();
			ref = obj;
			getPointerDB<T>()[obj]++;
		}

		// Destroy if Last Man Standing.
		// I.E. Delete reference if last Pointer to exist with it.
		void unbind(bool dilms = dilmsState) {
			if (exists()) return;
			if (getPointerDB<T>()[ref]-1 == 0 && dilms)
				destroy();
			else
				getPointerDB<T>()[ref]--;
			ref = nullptr;
		}

		void destroy() {
			if (exists()) return;
			getPointerDB<T>()[ref] = 0;
			delete ref;
			ref = nullptr;
		}

		bool exists() {
			return (
				ref != nullptr
			&&	getPointerDB<T>()[ref] != 0
			);
		}

		T* getPointer()	{
			static_assert(ref != nullptr, "Pointer reference does not exist!");
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (ref);
		}

		const T* getPointer() const	{
			static_assert(ref != nullptr, "Pointer reference does not exist!");
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (ref);
		}

		T& getValue() {
			static_assert(ref != nullptr, "Pointer reference does not exist!");
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (*ref);
		}

		const T& getValue() const {
			static_assert(ref != nullptr, "Pointer reference does not exist!");
			if (!exists())
				throw runtime_error("Pointer reference does not exist!");
			return (*ref);
		}

		T* operator->()				{return getPointer();}
		const T* operator->() const	{return getPointer();}
		T& operator*()				{return getValue();}
		const T& operator*() const	{return getValue();}
	private:
		T* ref = nullptr;
		friend class Pointer<T>;
	};

	template <Pointable T>
	using SharedPointer = Pointer<T,	true,	true>;

	template <Pointable T>
	using StrongPointer = Pointer<T,	false,	true>;

	template <Pointable T>
	using WeakPointer	= Pointer<T,	false,	false>;
}

#define $ref Reference::

#endif // REFERENCE_HANDLER_H
