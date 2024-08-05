# Structure
| Namespace | Scope |
|:---------:|:------|
| `CTL` | Classes in the `ctl/` folder |
| `CTL::Impl` | Specific classes |
| `CTL::Ex` | Everything else |

Some classes, `typedef`s, functions, etrc. will be moved into the `ctl/` folder. These classes are:

- `SmartPointer::*` -> `ReferenceCounter` to `CTL::Base`, rest to `CTL`
- `View::*` -> `CTL`
- `Threaded::*` -> `CTL`
- `Async::*` -> `CTL`
- `View::*` -> `CTL`
