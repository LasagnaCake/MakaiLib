# Collection Refactor

## Compilation

Gets compiled into its own library file, but for now it is header only.

## Structure
| Namespace | Domain | Location |
|:-:|:-|:-:|
| `CTL` | Core stuff | `ctl/*` |
| `CTL::*::Part` | Full & Partial implementations | `ctl/*` |
| `CTL::*::Base` | Class bases | `ctl/*` |
| `CTL::Math` | Math-related stuff | `ctl/math/*` |
| `CTL::RNG` | RNG-related stuff | `ctl/math/rng/*` |
| `CTL::Range` | Range-related stuff (excluding `range` function) | `ctl/range/*` |
| `CTL::FS` | Filesystem-related stuff | `ctl/fs/*` |
| `CTL::Time` | Time-related stuff | `ctl/time/*` |
| `CTL::Error` | Error-related stuff | `ctl/container/error/*` |
| `CTL::Meta` | Metaprogramming stuff | `ctl/meta/*` |
| `CTL::Regex` | Regex-related stuff | `ctl/regex/*` |
| `CTL::Type` | Concepts | `ctl/typetraits/*` |
| `CTL::OS` | Operating System stuff | `ctl/os/*` |
| `CTL::Sort` | Sorting algorithms (excluding `sort` function) | `ctl/algorithm/sort/*` |
| `CTL::Ex` | Extensions | root folder |

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
