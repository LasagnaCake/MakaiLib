# Structure
| Namespace | Scope |
|:---------:|:------|
| `CTL` | Classes in the `ctl/` folder |
| `CTL::Impl` | Implementations |
| `CTL::Bases` | Class bases |
| `CTL::Ex` | Everything else |

Some classes, `typedef`s, functions, etrc. will be moved into the `ctl/` folder. These are:

- `SmartPointer::*` -> `ReferenceCounter` to `CTL::Base`, rest to `CTL`
- `View::*` -> `CTL`
- `Threaded::*` -> `CTL`
- `Async::*` -> Everything but `TimeKeeper` to `CTL`
- `View::*` -> `CTL`
