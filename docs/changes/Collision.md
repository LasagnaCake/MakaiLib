# Collision System

## Collision Server

A collision server should:
* Create, destroy, register, deregister & manage every collision object
* Check collision for every collision object, based on its layer mask
* Fire appropriate callbacks for collision events, based on its layer mask
* Support manual collision events

### Collision Objects

An object should posses, at least, the following data:
* Its collision shape & type (or a structure handling this information)
* Collision layers it can affect
* Collision layers it can be affected by
* A callback function for handling collisions

## Collision Events

### Event Type

* **Object-Object**: An object collides with another object.
* **Object-Layer**: An object collides with the objects in a layer.
* **Layer-Layer**: A layer of objects collides with another layer of objects.

Automatic events are of the type **Object-Layer**, and each possible event should only need to be checked *once* per frame.

### Event Direction

* One-Way: A **affects** B, but B **doesn't affect** A.
* Two-Way: A **affects** B, and B **affects** A.

### Event Handling

For an event to succeed, the following conditions must be true:
1. Both objects have matching layers
2. Both objects can collide
3. The collision check succeeds

The collision object's callback function should be called if and only if all of the following conditions are true:
1. The appropriate collision event succeeds
2. Is affected by the event