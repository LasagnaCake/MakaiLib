# Collection Refactor

[Will (probably) use C++ modules.](Modules.md#Why)

## Compilation

Maybe it will get compiled into its own library file, but for now it is header only.

## Structure

Root namespace name: `CTL`.

---

| Symbol | Meaning |
|:-:|:-|
| root | The top-level namespace. |
| `::*` | Any namespace inside root, including root itself. |
| `::<NAME>` | A specific namespace inside root. |

---

| Namespace | Domain | Location |
|:-:|:-|:-:|
| root | Core stuff | `ctl/*` |
| `::*::Part` | Full & Partial implementations | `ctl/*` |
| `::*::Base` | Class bases | `ctl/*` |
| `::Math` | Math-related stuff | `ctl/math/*` |
| `::RNG` | RNG-related stuff | `ctl/math/rng/*` |
| `::Range` | Range-related stuff (excluding `range` function) | `ctl/range/*` |
| `::FS` | Filesystem-related stuff | `ctl/fs/*` |
| `::Time` | Time-related stuff | `ctl/time/*` |
| `::Error` | Error-related stuff | `ctl/container/error/*` |
| `::Meta` | Metaprogramming stuff | `ctl/meta/*` |
| `::Regex` | Regex-related stuff | `ctl/regex/*` |
| `::Type` | Concepts | `ctl/typetraits/*` |
| `::OS` | Operating System stuff | `ctl/os/*` |
| `::Sort` | Sorting algorithms (excluding `sort` function) | `ctl/algorithm/sort/*` |
| `::Ex` | Extensions | root folder |

### What Goes Where

Some classes, `typedef`s, functions, etc.
in the root folder will be moved into appropriate `ctl/` folders.
These are:

- [`referential.hpp`]
- - `SmartPointer::*` -> `ReferenceCounter` to `CTL::Base`, rest to `CTL`
- - (`Instance` & `Handle`) -> `CTL`
- `View::*` -> `CTL` (except `Reference`, which will be deleted)
- `Threaded::*` -> `CTL`
- `Async::*` -> Everything but `TimeKeeper` to `CTL`
- `Math::*` -> `RNG::*` to `CTL::RNG`, rest to `CTL::Math`
- `FileLoader::*` -> `CTL::FS`
- `Error::*` -> `CTL::Error`
- [`helper.hpp`]
- - `to*()` -> globally
- - `regex*()` -> `CTL::Regex` drop `regex` prefix
- - `Time::*` -> `CTL::Time`
- - `FileSystem::*` -> `CTL::FS`
- - `Fold::*` -> `CTL::Meta`
- `System::*` -> `CTL::OS`
- [`variant.hpp`]
- - (`Variant` becomes `Variable`) -> `CTL`
- `Type::*` -> `CTL::Type`

Everything else gets moved into `CTL::Ex`[^1]. 

[^1]: **EXCEPTIONS**: 1. user-defined literals, which are global. 2. 
