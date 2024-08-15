# New Codebase Layout

Root namespace name options:
- [ ] `Makai`
- [ ] `MGE`
- [ ] `Mk`

---

| Symbol | Meaning |
|:-:|:-|
| root | Self-explanatory. |
| `::*` | Any namespace inside root. |
| `::<NAME>` | A specific namespace inside root. |

---

| Namespace | Domain |
|:-:|:-|
| root | Core System |
| `::*::Part` | Full & Partial implementations |
| `::*::Base` | Class bases |
| `::Graph(ical)` | Graphical Subsystem |
| `::Audio` | Audio Subsystem |
| `::Game` | Game-related Stuff |
| `::Ex` | Game Engine Extensions |


- Everything from `program` folder → `Makai` (rename `Program` to `App`)
- Everything from `gamedata` folder (except subfolders & related content) → `Makai`
- Some program-related stuff from `gamedata` should be moved into the core program system
