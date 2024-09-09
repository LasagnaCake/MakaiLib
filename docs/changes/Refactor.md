# Great Refactor

## Simple

- [x] Reorganize and remove haphazardness/mess
- [ ] Overhaul [Collision System](../../src/new/ctl/legacy/entity/collidable.hpp) (See [New Collision System Proposal](../changes/Collision.md))
- 1. [x] Separate collision handling code into separate file
- 2. [x] Move collision handling to a server, that creates, destroys & manages collision shapes (See [Collision::\[Collision Server\]](../changes/Collision.md#Collision-Server))
- 3. [ ] Integrate into existing structure, while allowing for custom collision checking (See [Collision::\[Collision Events\]](../changes/Collision.md#Collision-Events))

## Intermediate

- [x] Remove dependency on external library types on header files (will require separating implementation from header first)
- - Replace it with its own types
- - - If required, create type conversion functions in implementation files
- [x] Remove `#include`s inside of namespaces
- [x] Unify everything under [a single namespace](Makai.md)[^1]
- [x] Separate code from headers & compile *everything* under a single library[^1]
- [ ] Move danmaku layer `enum` values into own `enum`
- [ ] Separate game-related stuff from core engine
- - Have it be an extension

## Complex
- [x] Polish [graphical](../../src/new/graph) system
- [x] Polish [audio](../../src/new/audio) system
- [ ] Replace raw pointers with smart pointers where possible (see [referential.hpp](../../src/new/ctl/legacy/referential.hpp))
- [ ] "Async"-ify [tasking system](../../src/new/ctl/legacy/tasking.hpp) (see [asynchronous.hpp](../../src/new/ctl/legacy/asynchronous.hpp))
- [ ] Remove dependency on cute_c2

## Very Complex
- [ ] Rethink [menu](../../src/legacy/gamedata/menu.hpp) code
- - Have it also be a 
- [ ] Rethink [entity](../../src/new/ctl/legacy/entity) system
- [ ] Replace STL with own internal library

[^1]: Except collection, which gets its [own special treatment](Collection.md#Structure).
