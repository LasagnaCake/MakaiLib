# Structure
| Namespace | Domain | Location |
|:-:|:-|:-:|
| `CTL` | Core classes | `ctl/*` |
| `CTL::Impl` | Implementations | `ctl/*` |
| `CTL::Bases` | Class bases | `ctl/*` |
| `CTL::Math` | Math-related stuff | `ctl/*` |
| `CTL::Range` | Range-related stuff | `ctl/*` |
| `CTL::FS` | Filesystem-related stuff | `ctl/*` |
| `CTL::Time` | Time-related stuff | `ctl/*` |
| `CTL::Error` | Error-related stuff | `ctl/*` |
| `CTL::Fold` | Fold-expressions | `ctl/*` |
| `CTL::Regex` | Regex-related stuff | `ctl/*` |
| `CTL::Ex` | Extensions | root folder |

## What Goes Where

Some classes, `typedef`s, functions, etc. (**EXCEPT** user-defined literals, which do not get put in a namespace)
in the root folder will be moved into the `CTL` namespace (and, subsequently, into `ctl/`).
These are:

- `SmartPointer::*` -> `ReferenceCounter` to `CTL::Base`, rest to `CTL`
- `View::*` -> `CTL` (except `Reference`, which will be deleted)
- `Threaded::*` -> `CTL`
- `Async::*` -> Everything but `TimeKeeper` to `CTL`
- `Math::*` -> `RNG::*` to `CTL::RNG`, rest to `CTL::Math`
- `FileSystem::*` -> `CTL::FS`
- `FileLoader::*` -> `CTL::FS`
- `Clock::*` -> `CTL::Time`
- `Error::*` -> `CTL::Error`
- `regex*` -> `CTL::Regex`

Everything else gets moved to `CTL::Ex`.
