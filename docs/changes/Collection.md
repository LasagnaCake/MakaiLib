# Structure
| Namespace | Contents |
|:---------:|:------|
| `CTL` | Core classes (located in the `ctl/` folder) |
| `CTL::Impl` | Implementations |
| `CTL::Bases` | Class bases |
| `CTL::Math` | Math-related stuff |
| `CTL::Range` | Math-related stuff |
| `CTL::Ex` | Extensions (located in the root folder) |

## What Goes Where

Some classes, `typedef`s, functions, etc. (EXCEPT user-defined literals, which do not get put in a namespace)
in the root folder will be moved into the `CTL` namespace (and, subsequently, into `ctl/`).
These are:

- `SmartPointer::*` -> `ReferenceCounter` to `CTL::Base`, rest to `CTL`
- `View::*` -> `CTL` (except `Reference`, which will be deleted)
- `Threaded::*` -> `CTL`
- `Async::*` -> Everything but `TimeKeeper` to `CTL`
- `Math::*` -> `RNG::*` to `CTL::RNG`, rest to `CTL::Math`

Everything else gets moved to `CTL::Ex`.
