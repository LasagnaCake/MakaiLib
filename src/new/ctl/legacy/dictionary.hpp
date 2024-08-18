#ifndef DICTIONARY_LIBRARY_H
#define DICTIONARY_LIBRARY_H

#include <unordered_map>

namespace Dictionary {
	namespace {
		using
		std::unordered_map,
		std::vector;
	}

	template <typename K, typename V>
	struct Entry {
		K key;
		V value;
	};

	template <typename K, typename V>
	class Dictionary {
	public:
		V& operator[](K& at) {
			return entries[at];
		}
		vector<K> getKeys() {
			vector<K> res;
			for (auto e: entries) {
				res.push_back(e.first);
			}
			return res;
		}
		vector<V> getValues() {
			vector<V> res;
			for (auto e: entries) {
				res.push_back(e.second);
			}
			return res;
		}
		Entry<K&, V&> getEntry(size_t index) {
			K res = getKeys()[index];
			for (auto& [ek, ev] : entries)
				if(ek == res)
					return Entry<K&, V&> {
						ek,
						ev
					};
		}
	private:
		unordered_map<K, V> entries;
	};
}

#endif // DICTIONARY_LIBRARY_H
