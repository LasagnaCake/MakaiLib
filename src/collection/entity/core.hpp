#ifndef COREOBJECT_BASE_OBJECT_H
#define COREOBJECT_BASE_OBJECT_H

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <functional>
#include <cxxabi.h>
#include <execution>
#include <map>

#include "../tasking.hpp"
#include "../event.hpp"
#include "../grouping.hpp"
#include "../conceptual.hpp"
#include "../grouping.hpp"
#include "../errors.hpp"
#include "../definitions.hpp"

#ifndef DEFAULT_ENTITY_ROOT_NAME
/// Default entity root name (MUST NOT CONTAIN '/')
#define DEFAULT_ENTITY_ROOT_NAME "@root"
#endif // DEFAULT_ENTITY_ROOT_NAME

/// Type getter macro.
#define GET_TYPE_STRING(VAR) abi::__cxa_demangle(typeid(VAR).name(),0,0,NULL)

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
	inline	static string getCoreClass() {return #NAME;}

namespace EntityClass {
	// Conflict prevention
	namespace {
		using
		std::vector,
		std::string,
		std::invalid_argument,
		std::function,
		std::unordered_map,
		Tasking::MultiTasker;

		// Duplicate root prevention system
		namespace { bool rc = false; }
		bool ENTITY_ROOT_CREATED() {
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
		auto queue = destroyQueue;
		for (auto queued : queue) {
			(*queued)();
		}
		destroyQueue.clear();
	}

	/// The root object's name.
	const string ENTITY_ROOT_NAME = DEFAULT_ENTITY_ROOT_NAME;

	class Entity;

	template <typename T>
	concept EntityType = Type::Derived<T, Entity>;

	/// The root object.
	Entity* _ROOT = nullptr;

	typedef Group::Group<Entity*> EntityGroup;
	EntityGroup groups;

	void init();

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
		virtual void	onCreate()	{DEBUGLN("Hello!");}
		/// Called when object is deleted.
		virtual void	onDelete()	{DEBUGLN("Bye!");}
		/// Called every cycle (or frame).
		virtual void	onFrame(float delta)	{}

		/// Gets the object's class.
		inline virtual string getClass() {return "Entity";}
		/// Gets the object's base class.
		inline virtual string getBaseClass() {return "Entity";}
		/// Gets the object's "core" (EClass::) class
		inline static string getCoreClass() {return "Entity";}

		/// Destructor.
		virtual ~Entity() {
			// Do deletion procedures
			condemn();
		}

		/// Parent-less constructor.
		Entity(string name = "Entity", bool uniqueEntity = true) {
			// If root doesn't exist, create it
			if (name != ENTITY_ROOT_NAME)
				init();
			// Set object's name
			if (name.length()) setName(name);
			else
				Error::InvalidValue("Name cannot be null or empty.");
			// Call function to be executed at creation
			callOnCreate();
			// Add to root tree
			if (name != ENTITY_ROOT_NAME)
				_ROOT->addChild(this, uniqueEntity);
		}

		/// Parented constructor.
		Entity(Entity* parent, string name = "Entity", bool uniqueEntity = true): Entity(name) {
			// Parent object
			parent->addChild(this, uniqueEntity);
		}

		void addToGroup(size_t group) {
			groups.addObject(this, group);
		}

		vector<size_t> getGroups(size_t group) {
			return groups.getGroups(this);
		}

		bool isInGroup(size_t group) {
			return groups.isInGroup(this, group);
		}

		void removeFromAllGroups() {
			groups.removeFromAll(this);
		}

		void removeFromGroup(size_t group) {
			groups.removeFromGroup(this, group);
		}

		/**
		* Gets one of the object's child.
		* Also searches the child for an object following a slash "/", as in:
		* child/grandChild/greatGrandChild/object
		* Returns null if child does not exist.
		* TODO: Refactor this code, to be less of a mess.
		*/
		Entity* getChild(string path) {
			// The current level of the path
			string level;
			// The object's child to look for
			string root = "";
			// The object to look for in the child, if applicable
			string next = "";
			// Whether it is the last object to search
			bool isLast = true;
			// Create stream and get root
			std::istringstream objPath(path);
			std::getline(objPath, root, '/');
			// Loop through path string and get the rest
			while (getline(objPath, level, '/')) {
				isLast = false;
				next += level + '/';
			}
			// If next isn't empty, remove last character
			if (next != "") next.pop_back();
			// If root requested...
			if (root == ENTITY_ROOT_NAME) {
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
				for (Entity* child : children)
					newParent->addChild(child);
			// Clear vector
			children.clear();
		}

		/// Sets the object's parent, while also avoiding cyclical parenting.
		void setParent(Entity* parent) {
			// Check if object is root object
			if (name == ENTITY_ROOT_NAME)
				throw Error::InvalidValue("Root cannot be parented.");
			// Check if new parent is valid parent
			if (!isValidParent(parent))
				throw Error::InvalidValue("Cyclical parenting prohibited.");
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
					#ifdef ENTITY_TASKERS_BEFORE_MAIN
					taskers.yield(delta * 60.0f);
					onFrame(delta);
					#else
					onFrame(delta);
					taskers.yield(delta * 60.0f);
					#endif // ENTITY_TASKERS_BEFORE_MAIN
				}
				if (children.size()) {
					auto clist = children;
					for (Entity* child : clist)
						child->yield(delta);
				}
			}
		}

		/// Renames object, while also being careful with duplicate names.
		void setName(string newName, bool mustHaveUniqueName = true) {
			// if root, or trying to rename to root, error
			if ((name == ENTITY_ROOT_NAME || newName == ENTITY_ROOT_NAME) && ENTITY_ROOT_CREATED())
				throw Error::InvalidValue(
					string("Cannot rename root object, or name object '")
					+ ENTITY_ROOT_NAME
					+ string("'.")
				);
			// Check if name does not contain invalid characters
			for (char c : newName)
				if (c == '/')
					throw Error::InvalidValue(
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
					uniqueName = newName + "_" + std::to_string(copies++);
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
			getChild(name)->destroy();
		}

		/// Deletes a child at a given index.
		void deleteChild(size_t index) {
			children[index]->destroy();
		}

		/// Deletes all children.
		void deleteChildren() {
			auto chvec = children;
			for (Entity* child : chvec) {
				removeChild(child);
				child->destroy();
			}
			children.clear();
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
		Event::Signal const destroy = SIGNAL {
			condemn();
			delete this;
		};

		void queueDestroy() {
			destroyQueue.push_back(&destroy);
		}

		template <EntityType T>
		inline T* as() {return (T*)this;}

	protected:
		bool condemned = false;

		/// Called on object deletion.
		void condemn() {
			if (condemned) return;
			condemned = true;
			DEBUG("\n<");
			DEBUG(name);
			DEBUGLN(">\n");
			// Clear taskers
			DEBUGLN("Clearing taskers...");
			taskers.clearTaskers();
			// Call function to be executed at deletion
			DEBUGLN("Calling onDelete()...");
			callOnDelete();
			// Remove self from the equation
			DEBUGLN("Removing self from equation...");
			removeFromTree();
			removeFromAllGroups();
			ERASE_IF(destroyQueue, elem == &destroy);
			DEBUGLN("Adieu!");
			DEBUG("\n</");
			DEBUG(name);
			DEBUGLN(">\n");
		}

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
			ERASE_IF(children, elem == child);
			child->parent = nullptr;
		}

		void removeFromTree() {
			// If there are children...
			if (children.size())
				// Delete them
				deleteChildren();
			// If parented, remove self from parent's children
			if (parent) parent->removeChild(this);
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

		/// Calls the onDelete function.
		void callOnDelete() {
			onDelete();
		}

		/// Calls the onCreate function
		void callOnCreate() {
			onCreate();
		}
	};

	// I do not know why, but attributing these fixed a Buffer Overflow. WHAT THE FUCK?

	/**
	* Initializes the root tree.
	* Gets called upon program start.
	*/
	[[gnu::constructor]] void init() {
		if (EntityClass::_ROOT == nullptr) {
			DEBUGLN("Creating root tree...");
			_ROOT = new Entity(ENTITY_ROOT_NAME);
			DEBUGLN("Root tree created!");
		}
	}

	/**
	* Denitializes the root tree.
	* Gets called upon program end.
	*/
	[[gnu::destructor]] void close() {
		if (EntityClass::_ROOT != nullptr) {
			DEBUGLN("Destroying root tree...");
			delete _ROOT;
			_ROOT = nullptr;
			DEBUGLN("Root tree destroyed!");
		}
	}

	/// Gets a specific object in the root tree, and casts it appropriately.
	template <EntityType T>
	T* getEntity(string path) {
		// Try and get object
		Entity* res = _ROOT->getChild(path);
		// If it exists, return object (casted)
		if (res) return (T*)res;
		// Else, return null
		return nullptr;
	}
}

#endif // COREOBJECT_ROOT_OBJECT_H
