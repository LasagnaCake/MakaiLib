#### Simple

- [ ] Reorganize and remove haphazardness/mess
- - [ ] Remove `#include`s inside of namespaces
- - [ ] Unify everything under a single namespace
- - [ ] Separate code from headers & compile _everything_ under a single library
- [ ] Overhaul [Collision System](src/collection/entity/collidable.hpp) (See [New Collision System Proposal](docs/changes/Collision.md))
- 1. [x] Separate collision handling code into separate file
- 2. [x] Move collision handling to a server, that creates, destroys & manages collision shapes (See [Collision::\[Collision Server\]](docs/changes/Collision.md#Collision-Server))
- 3. [ ] Integrate into existing structure, while allowing for custom collision checking (See [Collision::\[Collision Events\]](docs/changes/Collision.md#Collision-Events))
- 4. [ ] Remove dependency on cute_c2

#### Complex

- [ ] Cleanup & polish [graphical](src/graphical) system
- [ ] Cleanup & polish [audio](src/audio) system
- [ ] Replace raw pointers with smart pointers (see [referential.hpp](src/collection/referential.hpp))
- [x] Remove or replace `$` macros
- [ ] "Async"-ify [tasking system](src/collection/tasking.hpp) (see [asynchronous.hpp](src/collection/asynchronous.hpp))
