# Structure
| Namespace | Domain | Location |
|:-:|:-|:-:|
| `CTL` | Core classes | `ctl/*` |
| `CTL::Impl` | Implementations | `ctl/*` |
| `CTL::Bases` | Class bases | `ctl/*` |
| `CTL::Math` | Math-related stuff | `ctl/math/` |
| `CTL::RNG` | RNG-related stuff | `ctl/math/rng/` |
| `CTL::Range` | Range-related stuff | `ctl/range/` |
| `CTL::FS` | Filesystem-related stuff | `ctl/fs/` |
| `CTL::Time` | Time-related stuff | `ctl/time/` |
| `CTL::Error` | Error-related stuff | `ctl/container/` |
| `CTL::Meta` | Metaprogramming stuff | `ctl/meta/` |
| `CTL::Regex` | Regex-related stuff | `ctl/regex/` |
| `CTL::Type` | Concepts | `ctl/typetraits/` |
| `CTL::OS` | Operating System stuff | `ctl/regex/` |
| `CTL::Ex` | Extensions | root folder |

## What Goes Where

Some classes, `typedef`s, functions, etc. (**EXCEPT** user-defined literals, which are global)
in the root folder will be moved into the `CTL` namespace (and, subsequently, into `ctl/`).
These are:

- `SmartPointer::*` -> `ReferenceCounter` to `CTL::Base`, rest to `CTL`
- `View::*` -> `CTL` (except `Reference`, which will be deleted)
- `Threaded::*` -> `CTL`
- `Async::*` -> Everything but `TimeKeeper` to `CTL`
- `Math::*` -> `RNG::*` to `CTL::RNG`, rest to `CTL::Math`
- `FileSystem::*` -> `CTL::FS`
- `FileLoader::*` -> `CTL::FS`
- `Time::*` -> `CTL::Time`
- `Error::*` -> `CTL::Error`
- `regex*()` -> `CTL::Regex`
- `to*()` -> globally
- `System::*` -> `CTL::OS`
- [`variant.hpp`] -> `Variant` >> `Variable` to `CTL`
- `Type::*` -> `CTL::Type`

Everything else gets moved to `CTL::Ex`.
