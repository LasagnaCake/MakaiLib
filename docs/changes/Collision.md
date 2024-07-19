# Collision System

## Collision Server

A collision server should:
* Check collision between every object, based on its layer mask
* Fire appropriate callbacks for collision events
* Support manual collision checking events

## Collision Events

### Event Type

* Object-Object: An object collides with another object.
* Object-Layer: An object collides with the objects in a layer.
* Layer-Layer: A layer of objects collides with another layer of objects.

### Event Direction

* One-Way: X **affects** Y, but Y **doesn't affect** X.
* Two-Way: X **affects** Y, and Y **affects** X.