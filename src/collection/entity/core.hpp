#ifndef COREOBJECT_BASE_OBJECT_H
#define COREOBJECT_BASE_OBJECT_H

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <functional>
#include <cxxabi.h>
#include <map>

#include "../tasking.hpp"
#include "../event.hpp"
#include "../grouping.hpp"

#ifndef _$_ENTITY_ROOT_NAME
/// Default entity root name (MUST NOT CONTAIN '/')
#define _$_ENTITY_ROOT_NAME "@root"
#endif // _$_ENTITY_ROOT_NAME

/// Type getter macro.
#define $gettype(VAR) abi::__cxa_demangle(typeid(VAR).name(),0,0,NULL)

/// Core class construction macro.
#define ENTITY_CLASS(NAME, BASE)\
	using	Entity::Entity;\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return #NAME;}

/// Core derived class construction macro.
#define DERIVED_CLASS(NAME, BASE)\
	using	BASE :: BASE ;\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return BASE::getCoreClass();}

namespace EntityClass {
	// Conflict prevention
	namespace {
		using
		std::vector,
		std::shared_ptr,
		std::string,
		std::invalid_argument,
		std::function,
		std::map,
		Tasking::MultiTasker;

		// Duplicate root prevention system
		namespace { bool rc = false; }
		bool $_ROOT_CREATED() {
			if (!rc) {
				rc = true;
				return false;
			}
			return true;
		}

		vector<const Event::Signal*> destroyQueue;
	}

	/// Deletes all queued objects.
	void destroyQueued() {
		while (!destroyQueue.empty()) {
			(*destroyQueue.back())();
			destroyQueue.pop_back();
		}
	}

	/// The root object's name.
	const string $_ROOT_NAME = _$_ENTITY_ROOT_NAME;

	/**
	***********************
	*                     *
	*  Core Entity Class  *
	*                     *
	***********************
	*/
	class Entity {
	public:
		/// Whether the object and children should be processing or not.
		bool process = true;

		/// Whether this specific object should be processing.
		bool active = true;

		/**
		* The object's list of taskers.
		* Separate from processes, as these happen "asynchronously".
		*/
		MultiTasker taskers;

		// onAction functions

		/// Called when object is created.
		virtual void	onCreate()	{}
		/// Called when object is deleted.
		virtual void	onDelete()	{}
		/// Called every cycle (or frame).
		virtual void	onFrame(float delta)	{}

		/// Gets the object's class.
		inline virtual string getClass() {return "Entity";}
		/// Gets the object's base class.
		inline virtual string getBaseClass() {return nullptr;}
		/// Gets the object's "core" (EClass::) class
		inline static string getCoreClass() {return "Entity";}

		/// Destructor.
		virtual ~Entity() {
			// Call function to be executed at deletion
			onDelete();
			// Remove self from the equation
			removeFromTree();
		}

		/// Parent-less constructor.
		Entity(string name = "Entity") {
			// Set object's name
			if (name.length()) setName(name);
			else
				throw invalid_argument("Name cannot be null or empty.");
			//	Call function to be executed at creation
			onCreate();
		}

		/// Parented constructor.
		Entity(Entity* parent, string name = "Entity", bool uniqueEntity = true) {
			// Set object's name
			if (name.length()) setName(name);
			else
				throw invalid_argument("Name cannot be null or empty.");
			// Parents object
			parent->addChild(this, uniqueEntity);
			// Call function to be executed at creation
			onCreate();
		}

		/**
		* Gets one of the object's child.
		* Also searches the child for an object following a slash "/", as in:
		* child/grandChild/greatGrandChild/object
		* Returns null if child does not exist.
		* TODO: Refactor this code, to be less of a mess.
		*/
		Entity* getChild(string path) {
			// The object's child to look for
			string root;
			// The object to look for in the child, if applicable
			string next;
			// Whether it is the last object to search
			bool isLast = true;
			// Loop through path string and...
			for(char c : path) {
				// If last object to search...
				if (isLast)
					// If no deeper path yet, append character to root
					if (c != '/') root += c;
					// Else, this is not last object
					else isLast = false;
				// Else, append character to next path
				else next += c;
			}
			// If root requested...
			if (root == $_ROOT_NAME) {
				// If not last object to search, search root
				if(!isLast) return getRoot()->getChild(next);
				// Else, return root
				else return getRoot();
			}
			// If object has children...
			if(children.size()) {
				// If not last object to search...
				if (!isLast)
					// Loop through children and...
					for(Entity* child : children){
						// If child is root, search through child
						if (child->name == root)
							return child->getChild(next);
					}
				// else, loop through children (but different!) and...
				else for(Entity* child : children)
					// If child's name matches, return child
					if(child->name == path) return child;
			}
			// If it got to this point, child does not exist
			// throw invalid_argument(string("Child does not exist: ") + path);
			return nullptr;
		}

		/// Gets one of the object's child, and casts it.
		template <class T>
		T* getChild(string path) {
			// Try and get object
			Entity* child = getChild(path);
			// If it exists, return object (casted)
			if (child) return (T*)child;
			// Else, return null
			return nullptr;
		}

		/**
		* Gets the object's Nth child.
		*/
		inline Entity* getChild(size_t index) {
			return children[index];
		}

		/// Gets the amount of children the object has.
		inline size_t getChildCount() {
			return children.size();
		}

		/**
		* Re-parents a child to object.
		* uniqueChild: Whether to check if child is unique
		* (i.e. must have unique name).
		* Useful if child will need to be searchable.
		*/
		void addChild(Entity* child, bool uniqueChild = true) {
			// If parented, remove child from parent
			Entity* oldParent = child->getParent();
			if (oldParent) oldParent->removeChild(child);
			// Set new child's parent
			child->setParent(this);
			// Check if child has same name as other child
			child->setName(child->name, uniqueChild);
			// Add child to children
			children.push_back(child);
		}

		/// Re-parents children to new parent.
		void reparentChildren(Entity* newParent) {
			// If there are children...
			if (children.size())
				// Loop through children and re-parent them
				for (Entity* child : children) newParent->addChild(child);
			// Clear vector
			children.clear();
		}

		/// Sets the object's parent, while also avoiding cyclical parenting.
		void setParent(Entity* parent) {
			// Check if object is root object
			if (name == $_ROOT_NAME)
				throw invalid_argument("Root cannot be parented.");
			// Check if new parent is valid parent
			if (!isValidParent(parent))
				throw invalid_argument("Cyclical parenting prohibited.");
			// If it got to this point, parent is valid
			this->parent = parent;
		}

		/// Returns the object's parent.
		inline Entity* getParent() {
			return parent;
		}

		/// Gets object's "root" parent.
		Entity* getRoot() {
			// If has parent, return parent's result
			if (parent) return parent->getRoot();
			// Else, return self
			return this;
		}

		/// Processes an object cycle, if object is processing. Also processes children, if active.
		void yield(float delta = 0.0f) {
			if (process) {
				if (active) {
					#ifdef _$_TASKERS_BEFORE_MAIN
					taskers.yield(delta * 60.0f);
					onFrame(delta);
					#else
					onFrame(delta);
					taskers.yield(delta * 60.0f);
					#endif // _$_TASKERS_BEFORE_MAIN
				}
				if (children.size())
					for (Entity* child : children)
						child->yield(delta);
			}
		}

		/// Renames object, while also being careful with duplicate names.
		void setName(string newName, bool mustHaveUniqueName = true) {
			// if root, or trying to rename to root, error
			if ((name == $_ROOT_NAME || newName == $_ROOT_NAME) && $_ROOT_CREATED())
				throw invalid_argument(
					string("Cannot rename root object, or name object '")
					+ $_ROOT_NAME
					+ string("'.")
				);
			// Check if name does not contain invalid characters
			for (char c : newName)
				if (c == '/')
					throw invalid_argument(
						string("Name cannot contain '/': ")
						+ newName
					);
			// Name copies count
			size_t copies = 0;
			// Name to check
			string uniqueName = newName;
			// If parented and must have unique name, check for duplicate names
			if (parent && mustHaveUniqueName)
				while (parent->getChild(uniqueName))
					uniqueName = newName + std::to_string(copies++);
			// Set unique name
			name = uniqueName;
		}

		/// Returns the object's name.
		inline string getName() {
			return name;
		}

		/// Compares self with another object, and returns whether they are the same.
		bool compare(Entity* other) {
			return this == other;
		}

		/// Deletes a child of a given name.
		void deleteChild(string name) {
			delete getChild(name);
		}

		/// Deletes a child at a given index.
		void deleteChild(size_t index) {
			delete children[index];
		}

		/// Deletes all children.
		void deleteChildren() {
			for (Entity* child : children)
				delete child;
		}

		/// Equality operator overload.
		bool operator==(Entity other) {
			return compare(&other);
		}

		/// Equality operator overload (reference).
		bool operator==(Entity* other) {
			return (this == other);
		}

		/// Inequality operator overload.
		bool operator!=(Entity other) {
			return !compare(&other);
		}

		/// Inequality operator overload (reference).
		bool operator!=(Entity* other) {
			return !(this == other);
		}

		/// Bracket operator overload (name).
		Entity* operator[](string name) {
			return getChild(name);
		}

		/// Bracket operator overload (index).
		Entity* operator[](size_t idx) {
			return getChild(idx);
		}

		/// Addition Assignment operator overload.
		void operator+=(Entity* child) {
			addChild(child);
		}

		/// Subtraction Assignment operator overload (name).
		void operator-=(string name) {
			deleteChild(name);
		}

		/// Subtraction Assignment operator overload (index).
		void operator-=(size_t index) {
			deleteChild(index);
		}

		/// Deletes self.
		const Event::Signal destroy = $signal {
			delete this;
		};

		void queueDestroy() {
			destroyQueue.push_back(&destroy);
		}

	protected:
		/// Returns whether a given object would be a valid parent object.
		bool isValidParent(Entity* obj) {
			if (obj == this || obj->isDistantParent(this)) return false;
			return true;
		}

		/// Returns whether the object (self) has a given object as a distant parent.
		bool isDistantParent(Entity* obj) {
			// If match, return true
			if (this == obj) return true;
			// If parented, return parent's result
			if (parent) return parent->isDistantParent(obj);
			// Else, return false
			return false;
		}

		/// Removes a child from the object's children. Does not delete child.
		void removeChild(Entity* child) {
			// If there are children
			if (children.size())
				// Loop through children and...
				for (size_t i = 0; i < children.size(); i++)
					// If child matches...
					if (children[i] == child) {
						// Remove child from children and end loop
						children.erase(children.begin() + i);
						break;
				}
		}

		void removeFromTree() {
			// If parented, remove self from parent's children
			if (parent) parent->removeChild(this);
			// If there are children...
			if (children.size())
				// Delete them
				deleteChildren();
			// Clear children list
			children.clear();
		}

	private:
		/// The object's parent.
		Entity* parent = nullptr;

		/// The object's name.
		string name;

		/// The object's children.
		vector<Entity*> children;
	};

	/// The root object.
	Entity $_ROOT($_ROOT_NAME);

	/// Gets a specific object in the root tree, and casts it appropriately.
	template <class T>
	T* getEntity(string path) {
		// Try and get object
		Entity* res = $_ROOT[path];
		// If it exists, return object (casted)
		if (res) return (T*)res;
		// Else, return null
		return nullptr;
	}

	struct EntityGroup {
		/// Gets a group of a given name.
		vector<Entity*>& getGroup(size_t group) {
			return g[group];
		}

		/// Get the groups an entity belongs to (if any).
		vector<size_t> getGroups(Entity* e) {
			vector<size_t> res;
			try {
				for (auto const& group : g)
					for (auto i : g[group.first])
						if (e == i)
							res.push_back(group.first);
			} catch(char e) {}
			return res;
		}

		/// Adds an entity to a group. if nonexistent, create group.
		void addEntity(Entity* e, size_t group) {
			if (&g[group] == nullptr)
				g[group] = vector<Entity*>();
			g[group].push_back(e);
		}

		/// Empties/creates a given group.
		void invokeGroup(size_t group) {
			g[group] = vector<Entity*>();
		}

		/// Removes an entity from a given group (if it is in said group).
		void removeEntity(Entity* e, size_t group) {
			// If group does not exist, return
			if (&g[group] == nullptr) return;
			// Get group
			vector<Entity*>& target = g[group];
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

		/// Returns all of the group's IDs.
		vector<size_t> getAllGroups() {
			vector<size_t> res;
			if (g.size() > 0)
				for (auto const& group : g)
					res.push_back(group.first);
			return res;
		}

		/// Checks if a group has a given entity.
		bool hasEntity(Entity* e, size_t group) {
			for (auto i : g[group])
				if (e == i)
					return true;
			return false;
		}

		/// Removes an object from all groups it is in.
		void removeFromAll(Entity* e) {
			vector<size_t> groups = getGroups(e);
			for (auto grp: groups) {
				removeEntity(e, grp);
			}
		}
	private:
		map<size_t, vector<Entity*>> g;
	} groups;
}

#define $ecl EntityClass::

#endif // COREOBJECT_ROOT_OBJECT_H
