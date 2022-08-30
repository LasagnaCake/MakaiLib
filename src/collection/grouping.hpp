#include <map>
#include <vector>

namespace Group {
	namespace {
		using namespace std;
	}

	template <class T>
	class Group {
	public:
		/// Gets a group of a given name.
		inline vector<T>& getGroup(size_t group) {
			if (&g[group] == nullptr)
				g[group] = vector<T>();
			return g[group];
		}

		vector<T>& operator[](size_t group) {
			if (&g[group] == nullptr)
				g[group] = vector<T>();
			return g[group];
		}

		/// Get the groups an object belongs to (if any).
		vector<size_t> getGroups(T e) {
			vector<size_t> res;
			try {
				for (auto const& group : g)
					for (auto i : g[group.first])
						if (e == i) {
							res.push_back(group.first);
							break;
						}
			} catch(char e) {}
			return res;
		}

		/// Adds an object to a group. if nonexistent, create group.
		inline void addObject(T e, size_t group) {
			if (&g[group] == nullptr)
				g[group] = vector<T>();
			g[group].push_back(e);
		}

		/// Empties/creates a given group.
		inline void invokeGroup(size_t group) {
			g[group] = vector<T>();
		}

		/// Removes an object from a given group (if it is in said group).
		void removeFromGroup(T e, size_t group) {
			// If group does not exist, return
			if (&g[group] == nullptr) return;
			// Get group
			vector<T>& target = g[group];
			// If group is not empty...
			if (target.size())
				// Loop through group and...
				for (size_t i = 0; i < target.size(); i++)
					// If entity matches...
					if (target[i] == e) {
						// Remove entity from group and end loop
						target.erase(target.begin() + i);
						break;
				}
		}

		/// Removes an object from all groups it is in.
		void removeFromAll(T e) {
			vector<size_t> groups = getGroups(e);
			for (auto grp: groups) {
				removeFromGroup(e, grp);
			}
		}

		/// Returns all of the group's IDs.
		vector<size_t> getAllGroups() {
			vector<size_t> res;
			if (g.size() > 0)
				for (auto const& group : g)
					res.push_back(group.first);
			return res;
		}

		/// Checks if a group has a given entity.
		bool isInGroup(T* e, size_t group) {
			for (auto i : g[group])
				if (e == i)
					return true;
			return false;
		}
	private:
		map<size_t, vector<T>> g;
	};
}
