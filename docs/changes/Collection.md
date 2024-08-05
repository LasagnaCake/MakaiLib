# Structure
| Namespace | Domain | Location |
|:-:|:-|:-:|
| `CTL` | Core classes | `ctl/` and subfolders |
| `CTL::Impl` | Implementations | `ctl/` and subfolders |
| `CTL::Bases` | Class bases | `ctl/` and subfolders |
| `CTL::Math` | Math-related stuff | `ctl/` and subfolders |
| `CTL::Range` | Range-related stuff | `ctl/` and subfolders |
| `CTL::FS` | Filesystem-related stuff | `ctl/` and subfolders |
| `CTL::Time` | Time-related stuff | `ctl/` and subfolders |
| `CTL::Error` | Error-related stuff | `ctl/` and subfolders |
| `CTL::Fold` | Fold-expressions | `ctl/` and subfolders |
| `CTL::Ex` | Extensions (files located in the root folder) ||

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

Everything else gets moved to `CTL::Ex`.
