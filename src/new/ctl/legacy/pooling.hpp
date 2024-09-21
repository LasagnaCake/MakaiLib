#ifndef OBJECT_POOL_POOLABLE_H
#define OBJECT_POOL_POOLABLE_H

#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>

/// Object state checking function macro
#define POOL_CHECH(TYPE)	[&](TYPE* object) -> bool

/// Object state setting function macro
#define POOL_SET(TYPE)		[&](TYPE* object) -> void

namespace Pooling{
	namespace {
		using std::vector,
		std::shared_ptr,
		std::function,
		std::overflow_error;
	}

	template<class T>
	/// Object state checking function
	using Checker	= function<bool(T*)>;

	template<class T>
	/// Object state setting function macro
	using Setter	= function<void(T*)>;

	template<class T>
	/**
	**********************
	*                    *
	*  ObjectPool Class  *
	*                    *
	**********************
	*/
	class ObjectPool{
	public:

		/// State checking function.
		Checker<T>	checkFree;

		/// Object initialization function.
		Setter<T>	initObject() {}

		/// State setting function (Active).
		Setter<T>	setActive;

		/// State setting function (Free).
		Setter<T>	setFree;

		/// Non-initialization constructor.
		ObjectPool(size_t prealloc = 1) {
			if (prealloc) allocateFree(prealloc);
			else allocateFree();
		}

		/// Initialization constructor.
		ObjectPool(Setter<T> initFunc, size_t prealloc = 1) {
			initObject = initFunc;
			if (prealloc) allocateFree(prealloc);
			else allocateFree();
		}

		/// Destructor.
		~ObjectPool() {
			for (T* obj: all) delete obj;
			all.clear();
			free.clear();
		}

		/// Gets a free object from the pool. If none is available, make new object.
		T* getFreeObject(){
			// A pointer to the free object
			T* ret;
			// Update list of free objects
			if (!freeCount) updateFree();
			// If a free object exists...
			if (freeCount) {
				// Get topmost object in the list
				ret = free[--freeCount];
				free.pop_back();
			// Else, throw error
			} else {
				throw overflow_error("No free objects available. Free up objects, or increase pool size.");
			}
			// Set object active, and return pointer to it
			setActive(ret);
			return ret;
		}

		/// Updates the list of free objects.
		void updateFree() {
			if (all.size())
				for (size_t i = 0; i < all.size(); i++) {
					T* obj = all[i];
					if (checkFree(obj)) free.push_back(obj);
				}
			freeCount = free.size();
		}

		/// Frees all objects in the pool.
		void setAllFree() {
			if (all.size())
				for (size_t i = 0; i < all.size(); i++) {
					T* obj = all[i];
					setFree(obj);
					free.push_back(obj);
				}
			freeCount = free.size();
		}

		/// Frees a specific object in the pool.
		void freeObject(T* obj) {
			for (T* i : all) {
					if (obj == i) {
						setFree(obj);
						free.push_back(obj);
						return;
					}
				}
		}

		/// Allocates a given number of objects.
		void allocateFree(size_t n = 1) {
			if (n) {
				T* obj;
				if (n == 1) {
					obj = new T;
					free.push_back(obj);
					all.push_back(obj);
					initObject(obj);

				}
				else for (size_t i = 0; i < n; i++) {
					obj = new T;
					free.push_back(obj);
					all.push_back(obj);
					initObject(obj);
				}
			}
		}

	private:
		/// The list of free objects.
		vector<T*>	free;
		size_t freeCount = 0;

		/// All objects in the pool.
		vector<T*>	all;
	};
}

#endif // OBJECT_POOL_POOLABLE_H
