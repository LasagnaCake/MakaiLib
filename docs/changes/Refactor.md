# Great Refactor

## Simple

- [ ] Reorganize and remove haphazardness/mess
- [ ] Overhaul [Collision System](../../src/collection/entity/collidable.hpp) (See [New Collision System Proposal](../changes/Collision.md))
- 1. [x] Separate collision handling code into separate file
- 2. [x] Move collision handling to a server, that creates, destroys & manages collision shapes (See [Collision::\[Collision Server\]](../changes/Collision.md#Collision-Server))
- 3. [ ] Integrate into existing structure, while allowing for custom collision checking (See [Collision::\[Collision Events\]](../changes/Collision.md#Collision-Events))
- 4. [ ] Remove dependency on cute_c2
- [x] Remove or replace `$` macros

## Intermediate

- [ ] Remove dependency on external library types on header files (will require separating implementation from header first)
- - Replace it with its own types
- - - If required, create type conversion functions in implementation files
- [ ] Remove `#include`s inside of namespaces
- [ ] Unify everything under [a single namespace](Makai.md)[^1]
- [ ] Separate code from headers & compile *everything* under a single library[^1]
- [ ] Move danmaku layer `enum` values into own `enum`
- [ ] Separate danmaku game base from core engine
- - Have it be an extension

## Complex
- [ ] Rethink [menu](../../src/legacy/gamedata/menu.hpp) code
- [ ] Rethink [entity](../../src/ctl/legacy/entity) system
- [ ] Cleanup & polish [graphical](../../src/graphical) system
- [ ] Cleanup & polish [audio](../../src/audio) system
- [ ] Replace raw pointers with smart pointers where possible (see [referential.hpp](../../src/collection/referential.hpp))
- [ ] "Async"-ify [tasking system](../../src/collection/tasking.hpp) (see [asynchronous.hpp](../../src/collection/asynchronous.hpp))

## Very Complex

- [ ] Replace STL with own internal library

[^1]: Except anything in the `tools` folder (which should be its own separate library, and thus, separate namespace), and collection, which gets its [own special treatment](Collection.md#Structure).
