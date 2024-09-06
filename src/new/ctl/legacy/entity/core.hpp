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
#include "../helper.hpp"

#ifndef DEFAULT_ENTITY_ROOT_NAME
/// Default entity root name (MUST NOT CONTAIN '/')
#define DEFAULT_ENTITY_ROOT_NAME "@root"
#endif // DEFAULT_ENTITY_ROOT_NAME

/// Core class construction macro.
#define ENTITY_CLASS(NAME, BASE)\
	using	Entity::Entity;\
	inline	virtual String getClass() {return #NAME;}\
	inline	virtual String getBaseClass() {return #BASE;}\
	inline	static String getCoreClass() {return #NAME;}

/// Core derived class construction macro.
#define DERIVED_CLASS(NAME, BASE)\
	using	BASE :: BASE ;\
	inline	virtual String getClass() {return #NAME;}\
	inline	virtual String getBaseClass() {return #BASE;}\
	inline	static String getCoreClass() {return #NAME;}

class Entity;

template <typename T>
concept EntityType = Type::Derived<T, Entity>;

/**
***********************
*                     *
*  Core Entity Class  *
*                     *
***********************
*/
class Entity {
public:
	using MultiTasker = Tasking::MultiTasker;

	inline static Entity* ROOT = nullptr;

	constexpr static String ROOT_NAME = DEFAULT_ENTITY_ROOT_NAME;

	typedef Group::Group<Entity*> EntityGroup;

	typedef List<Entity*> EntityList;

	/// Whether the object and children should be processing or not.
	bool process = true;

	/// Whether this specific object should be processing.
	bool active = true;

	inline static EntityGroup groups;

	/**
	* The object's list of taskers.
	* Separate from processes, as these happen "asynchronously".
	*/
	MultiTasker taskers;

	/// The object's notification handler.
	Event::Notifier notifier;

	// onAction functions

	/// Called when object is created.
	virtual void	onCreate()	{DEBUGLN("Hello!");}
	/// Called when object is deleted.
	virtual void	onDelete()	{DEBUGLN("Bye!");}
	/// Called every cycle (or frame).
	virtual void	onFrame(float delta)	{}

	/// Gets the object's class.
	inline virtual String getClass()		{return "Entity";}
	/// Gets the object's base class.
	inline virtual String getBaseClass()	{return "Entity";}
	/// Gets the object's "core" (Entities::) class
	inline static String getCoreClass()		{return "Entity";}

	/// Destructor.
	virtual ~Entity() {
		// Do deletion procedures
		condemn();
	}

	/// Parent-less constructor.
	Entity(
		String const& name = "Entity",
		bool const& uniqueEntity = true
	) {
		// If root doesn't exist, create it
		if (name != ROOT_NAME)
			init();
		// Set object's name
		if (name.length()) setName(name, uniqueEntity);
		else
			Error::InvalidValue("Name cannot be null or empty.");
		// Call function to be executed at creation
		callOnCreate();
		// Add to root tree
		if (name != ROOT_NAME)
			ROOT->addChild(this, uniqueEntity);
	}

	/// Parented constructor.
	Entity(
		Entity* const& parent,
		String const& name = "Entity",
		bool const& uniqueEntity = true
	): Entity(name) {
		// Parent object
		parent->addChild(this, uniqueEntity);
	}

	void addToGroup(size_t const& group) {
		groups.addObject(this, group);
	}

	List<size_t> getGroups(size_t const& group) {
		return groups.getGroups(this);
	}

	bool isInGroup(size_t const& group) {
		return groups.isInGroup(this, group);
	}

	void removeFromAllGroups() {
		groups.removeFromAll(this);
	}

	void removeFromGroup(size_t const& group) {
		groups.removeFromGroup(this, group);
	}

	/**
	* Gets one of the object's child.
	* Also searches the child for an object following a slash "/", as in:
	* child/grandChild/greatGrandChild/object
	* Returns null if child does not exist.
	* TODO: Refactor this code, to be less of a mess.
	*/
	Entity* getChild(String const& path) {
		// The object's child to look for
		auto [root, next] = Helper::splitStringAtFirst(path, '/');
		// Whether it is the last object in the list
		bool isFinalBranch = next.empty();
		// If root requested...
		if (root == ROOT_NAME) {
			// If not last object to search, search root
			if(!isFinalBranch)
				return getRoot()->getChild(next);
			// Else, return root
			return getRoot();
		}
		// If object has children...
		if(children.size()) {
			// If must go a level higher...
			if (root == "..") {
				// If parented, check in parent
				if (parent)
					return parent->getChild(next);
				// Else, return null pointer
				return nullptr;
			}
			// If not last object to search...
			if (!isFinalBranch)
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
	template <EntityType T>
	T* getChild(String const& path) {
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
	inline Entity* getChild(size_t const& index) {
		return children[index];
	}

	/**
	* Gets the object's Nth child, and casts it.
	*/
	template <EntityType T>
	inline Entity* getChild(size_t const& index) {
		if (index < children.size())
			return (T*)children[index];
		return nullptr;
	}

	/// Gets the amount of children the object has.
	inline size_t getChildCount() {
		return children.size();
	}

	/// Gets a copy of the object's children.
	inline EntityList getChildren() {
		return children;
	}

	/**
	* Re-parents a child to object.
	* uniqueChild: Whether to check if child is unique
	* (i.e. must have unique name).
	* Useful if child will need to be searchable.
	*/
	void addChild(
		Entity* const& child,
		bool const& uniqueChild = true
	) {
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
	void reparentChildren(Entity* const& newParent) {
		// If there are children...
		if (children.size())
			// Loop through children and re-parent them
			for (Entity* child : children)
				newParent->addChild(child);
		// Clear vector
		children.clear();
	}

	/// Sets the object's parent, while also avoiding cyclical parenting.
	void setParent(Entity* const& parent) {
		// Check if object is root object
		if (name == ROOT_NAME)
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
	void setName(String const& newName, bool const& mustHaveUniqueName = true) {
		// if root, or trying to rename to root, error
		if ((name == ROOT_NAME || newName == ROOT_NAME) && (ROOT != nullptr))
			throw Error::InvalidValue(
				String("Cannot rename root object, or name object '")
				+ ROOT_NAME
				+ String("'.")
			);
		// Check if name does not contain invalid characters
		for (char c : newName)
			if (c == '/')
				throw Error::InvalidValue(
					String("Name cannot contain '/': ")
					+ newName
				);
		// Name copies count
		size_t copies = 0;
		// Name to check
		String uniqueName = newName;
		// If parented and must have unique name, check for duplicate names
		if (parent && mustHaveUniqueName)
			while (parent->getChild(uniqueName))
				uniqueName = toString(newName, "_", copies++);
		// Set unique name
		name = uniqueName;
	}

	/// Returns the object's name.
	inline String getName() {
		return name;
	}

	/// Compares self with another object, and returns whether they are the same.
	bool compare(Entity* const& other) {
		return this == other;
	}

	/// Deletes a child of a given name.
	void destroyChild(String const& name) {
		getChild(name)->destroy();
	}

	/// Deletes a child at a given index.
	void destroyChild(size_t const& index) {
		children[index]->destroy();
	}

	/// Queues a child of a given name for deletion.
	void queueDestroyChild(String const& name) {
		getChild(name)->queueDestroy();
	}

	/// Queues a child at a given index for deletion.
	void queueDestroyChild(size_t const& index) {
		children[index]->queueDestroy();
	}

	/// Deletes all children.
	void destroyChildren() {
		auto chvec = children;
		for (Entity* child : chvec) {
			removeChild(child);
			child->destroy();
		}
		children.clear();
	}

	/// Equality operator overload.
	bool operator==(Entity& other) {
		return compare(&other);
	}

	/// Equality operator overload (reference).
	bool operator==(Entity* const& other) {
		return (this == other);
	}

	/// Bracket operator overload (name).
	Entity* operator[](String const& name) {
		return getChild(name);
	}

	/// Bracket operator overload (index).
	Entity* operator[](size_t const& idx) {
		return getChild(idx);
	}

	/// Deletes self.
	Event::Signal const destroy = SIGNAL {
		condemn();
		delete this;
	};

	void queueDestroy() {
		destroyQueue.push_back(&destroy);
	}

	Entity& operator=(Entity const& other)	= delete;
	Entity& operator=(Entity&& other)		= delete;

	template <EntityType T>
	inline T* as() {return (T*)this;}

	/// Deletes all queued objects.
	static inline void destroyQueued() {
		auto queue = destroyQueue;
		for (auto queued : queue) {
			(*queued)();
		}
		destroyQueue.clear();
	}

	/**
	* Initializes the root tree.
	* Gets called upon program start.
	*/
	[[gnu::constructor]] inline static void init() {
		if (ROOT == nullptr) {
			DEBUGLN("Creating root tree...");
			ROOT = new Entity(ROOT_NAME);
			DEBUGLN("Root tree created!");
		}
	}

	/**
	* Denitializes the root tree.
	* Gets called upon program end.
	*/
	[[gnu::destructor]] inline static void close() {
		if (ROOT != nullptr) {
			DEBUGLN("Destroying root tree...");
			delete ROOT;
			ROOT = nullptr;
			DEBUGLN("Root tree destroyed!");
		}
	}

protected:
	bool condemned = false;

	/// Called on object deletion.
	void condemn() {
		if (condemned) return;
		condemned = true;
		DEBUGLN("\n<", name, ">\n");
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
		DEBUGLN("\n</", name, ">\n");
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
		// If there are children, delete them
		if (children.size())
			destroyChildren();
		// If parented, remove self from parent's children
		if (parent) parent->removeChild(this);
		// Clear children list
		children.clear();
	}

private:
	/// The object's parent.
	Entity* parent = nullptr;

	/// The object's name.
	String name;

	/// The object's children.
	EntityList children;

	/// Calls the onDelete function.
	void callOnDelete() {
		onDelete();
	}

	/// Calls the onCreate function
	void callOnCreate() {
		onCreate();
	}

	inline static List<Event::Signal const*> destroyQueue;
};

#endif // COREOBJECT_ROOT_OBJECT_H
