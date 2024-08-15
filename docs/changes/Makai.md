# New Codebase Layout

| Namespace | Domain |
|:-:|:-|
| `Makai` | Core System |
| `Makai::Graph(ical)` | Graphical Subsystem |
| `Makai::Audio` | Audio Subsystem |
| `Makai::Game` | Game-related Stuff |
| `Makai::Ex` | Game Engine Extensions |


- Everything from `program` folder → `Makai` (rename `Program` to `App`)
- Everything from `gamedata` folder (except subfolders & related content) → `Makai`
- Some program-related stuff from `gamedata` should be moved into the core program system
