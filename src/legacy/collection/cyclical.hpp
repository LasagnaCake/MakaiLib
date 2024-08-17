#ifndef CYCLICAL_CARROUSSEL_CYCLE_H
#define CYCLICAL_CARROUSSEL_CYCLE_H

#include <vector>
#include <list>
#include <functional>
#include <stdexcept>
#include <type_traits>

namespace Cyclical{
	namespace {
		using std::list,
		std::vector,
		std::length_error;
	}

	template<class T>
	/**
	**********************
	*                    *
	*  Carroussel Class  *
	*                    *
	**********************
	*/
	class Carrousel{
	public:
		/// Empty constructor.
		Carrousel() {

		}

		/// Vector constructor.
		Carrousel(vector<T*> items) {
			for (T item : items)
				this->items.push_back(item);
		}

		/// List constructor.
		Carrousel(list<T*> items) {
			this->items = items;
		}

		/// Destructor.
		~Carrousel() {
			for (T* item: items) delete item;
			items.clear();
		}

		/// Adds an item to the (current) front of the carrousel.
		void addFront(T* item) {
			items.push_front(item);
		}

		/// Adds an item to the (current) back of the carrousel.
		void addBack(T* item) {
			items.push_back(item);
		}

		/// Gets the previous item on the carrousel.
		T* last() {
			// If carrousel has less than two items, return current item
			if (items.size() < 2) return current();
			// Get current item on carrousel & push to beginning
			T* current = items[items.end()];
			items.pop_back();
			items.push_front(current);
			// Return previous item
			return items[0];
		}

		/// Gets the next item on the carrousel.
		T* next() {
			// If carrousel has less than two items, return current item
			if (items.size() < 2) return current();
			// Get current item on carrousel & push to end
			T* current = items[0];
			items.pop_front();
			items.push_back(current);
			// Return next item
			return items[0];
		}

		/// Gets the current item on the carrousel.
		T* current() {
			// If carrousel has zero items, error
			if (!items.size())
				throw length_error("Carroussel is empty!");
			// Return current item
			return items[0];
		}

	private:
		/// The list of carrousel's items.
		list<T*> items;
	};
}

#endif // CYCLICAL_CARROUSSEL_CYCLE_H
