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
#include "../referential.hpp"

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
		std::string,
		std::invalid_argument,
		std::function,
		std::unordered_map,
		Tasking::MultiTasker;

		using namespace SmartPointer;

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
		for (auto queued : destroyQueue) {
			(*queued)();
		}
		destroyQueue.clear();
	}

	/// The root object's name.
	const string $_ROOT_NAME = _$_ENTITY_ROOT_NAME;

	class Entity;

	typedef Pointer<Entity> EntityRef;

	template <typename T>
	concept EntityType = Type::Derived<T, Entity>;

	/// The root object.
	EntityRef $_ROOT;

	template <EntityType T>
	EntityRef create(string name = "Entity", bool uniqueEntity = true) {
		return new T(name);
	}

	template <EntityType T>
	EntityRef create(Entity* parent, string name = "Entity", bool uniqueEntity = true) {
		return new T(parent, name, uniqueEntity);
	}

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
		template <EntityType T> friend T* create() {};

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
		inline virtual string getBaseClass() {return "";}
		/// Gets the object's "core" (EClass::) class
		inline static string getCoreClass() {return "Entity";}

		/// Destructor.
		virtual ~Entity() {
			// Call function to be executed at deletion
			callOnDelete();
			// Remove self from the equation
			removeFromTree();
			removeFromAllGroups();
		}

		/// Parent-less constructor.
		Entity(string name = "Entity", bool uniqueEntity = true) {
			// If root doesn't exist, create it
			if (name != $_ROOT_NAME && !$_ROOT.exists())
				init();
			// Set object's name
			if (name.length()) setName(name);
			else
				throw invalid_argument("Name cannot be null or empty.");
			// Call function to be executed at creation
			onCreate();
			// Add to root tree
			if (name != $_ROOT_NAME)
				$_ROOT->addChild(this, uniqueEntity);
		}

		/// Parented constructor.
		Entity(EntityRef parent, string name = "Entity", bool uniqueEntity = true): Entity(name) {
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
		EntityRef getChild(string path) {
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
					for(EntityRef child : children){
						// If child is root, search through child
						if (child->name == root)
							return child->getChild(next);
					}
				// else, loop through children (but different!) and...
				else for(EntityRef child : children)
					// If child's name matches, return child
					if(child->name == path) return child;
			}
			// If it got to this point, child does not exist
			// throw invalid_argument(string("Child does not exist: ") + path);
			return EntityRef();
		}

		/// Gets one of the object's child, and casts it.
		template <class T>
		EntityRef getChild(string path) {
			// Try and get object
			EntityRef child = getChild(path);
			// If it exists, return object (casted)
			if (child.exists()) return (T*)child;
			// Else, return null
			return EntityRef();
		}

		/**
		* Gets the object's Nth child.
		*/
		inline EntityRef getChild(size_t index) {
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
		void addChild(EntityRef child, bool uniqueChild = true) {
			// If parented, remove child from parent
			EntityRef oldParent = child->getParent();
			if (oldParent.exists()) oldParent->removeChild(&(*child));
			// Set new child's parent
			child->setParent(this);
			// Check if child has same name as other child
			child->setName(child->name, uniqueChild);
			// Add child to children
			children.push_back(child);
		}

		/// Re-parents children to new parent.
		void reparentChildren(EntityRef newParent) {
			// If there are children...
			if (children.size())
				// Loop through children and re-parent them
				for (EntityRef child : children) newParent->addChild(child);
			// Clear vector
			children.clear();
		}

		/// Sets the object's parent, while also avoiding cyclical parenting.
		void setParent(EntityRef parent) {
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
		inline EntityRef getParent() {
			return parent;
		}

		/// Gets object's "root" parent.
		EntityRef getRoot() {
			// If has parent, return parent's result
			if (parent.exists()) return parent->getRoot();
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
					std::for_each(
						std::execution::par,
						children.begin(),
						children.end(),
						[&](auto&& child){
							child->yield(delta);
						}
					);
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
			if (parent.exists() && mustHaveUniqueName)
				while (parent->getChild(uniqueName).exists())
					uniqueName = newName + std::to_string(copies++);
			// Set unique name
			name = uniqueName;
		}

		/// Returns the object's name.
		inline string getName() {
			return name;
		}

		/// Compares self with another object, and returns whether they are the same.
		bool compare(EntityRef other) {
			return this == other;
		}

		/// Deletes a child of a given name.
		void deleteChild(string name) {
			getChild(name).destroy();
		}

		/// Deletes a child at a given index.
		void deleteChild(size_t index) {
			children[index].destroy();
		}

		/// Deletes all children.
		void deleteChildren() {
			for (EntityRef child : children)
				child.destroy();
		}

		/// Equality operator overload.
		bool operator==(Entity& other) {
			return compare(&other);
		}

		/// Equality operator overload (reference).
		bool operator==(EntityRef other) {
			return (other == this);
		}

		/// Inequality operator overload.
		bool operator!=(Entity& other) {
			return !compare(&other);
		}

		/// Inequality operator overload (reference).
		bool operator!=(EntityRef other) {
			return !(other == this);
		}

		/// Bracket operator overload (name).
		EntityRef operator[](string name) {
			return getChild(name);
		}

		/// Bracket operator overload (index).
		EntityRef operator[](size_t idx) {
			return getChild(idx);
		}

		/// Addition Assignment operator overload.
		void operator+=(EntityRef child) {
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
			removeFromTree();
			EntityRef(this).destroy();
		};

		void queueDestroy() {
			destroyQueue.push_back(&destroy);
		}

	protected:
		/// Returns whether a given object would be a valid parent object.
		bool isValidParent(EntityRef obj) {
			if (obj == this || obj->isDistantParent(this)) return false;
			return true;
		}

		/// Returns whether the object (self) has a given object as a distant parent.
		bool isDistantParent(EntityRef obj) {
			// If match, return true
			if (this == obj) return true;
			// If parented, return parent's result
			if (parent.exists()) return parent->isDistantParent(obj);
			// Else, return false
			return false;
		}

		/// Removes a child from the object's children. Does not delete child.
		void removeChild(WeakPointer<Entity> child) {
			$debug("Removing child '" + child->getName() + "'...");
			// If there are children
			if (children.size())
				// Loop through children and...
				for (size_t i = 0; i < children.size(); i++)
					// If child matches...
					if (children[i] == child) {
						$debug("Child found!");
						children[i].unbind(false);
						// Remove child from children and end loop
						children.erase(children.begin() + i);
						break;
					}
		}

		void removeFromTree() {
			// If parented, remove self from parent's children
			if (parent.exists()) parent->removeChild(this);
			// If there are children...
			if (children.size())
				// Delete them
				deleteChildren();
			// Clear children list
			children.clear();
		}

	private:
		/// The object's parent.
		EntityRef parent;

		/// The object's name.
		string name;

		/// The object's children.
		vector<EntityRef> children;

		/// Calls the onDelete function.
		void callOnDelete() {
			onDelete();
		}
	};

	/**
	* Initializes the root tree.
	* Gets called upon the first entity's creation, so it
	* is not necessary to call again.
	*/
	void init() {
		if (!EntityClass::$_ROOT.exists()) {
			$debug("Creating root tree...");
			$_ROOT.bind(new Entity($_ROOT_NAME));
			$debugp("Root tree created: ");
			$debug(EntityClass::$_ROOT.exists() ? "True" : "False");
		}
	}

	/// Gets a specific object in the root tree, and casts it appropriately.
	template <EntityType T>
	Pointer<T> getEntity(string path) {
		// Try and get object
		EntityRef res = $_ROOT->getChild(path);
		// If it exists, return object (casted)
		//if (res) return (T*)&(*res);
		if (res.exists()) return (T*)(res);
		// Else, return null
		return EntityRef();
	}
}

#define $ecl EntityClass::

#endif // COREOBJECT_ROOT_OBJECT_H
