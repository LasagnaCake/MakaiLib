# New Codebase Layout

Root namespace name options:
- [ ] `Makai` (Most likely)
- [ ] `MGE` (Just no.)
- [ ] `Mk` (Could also be used as the shorthand for the namespace (and namespace shorthands))

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
| `::Ex` | Extensions |

- `CTL` should be included globally inside the namespace (i.e. `using namespace CTL;`)
- Everything from `program` folder → root folder (rename `Program` to `App`)
- Everything from `gamedata` folder (except subfolders & related content) → `::Game`
- Some `*App`-related stuff from `gamedata` should be moved into the core system

## Game Bases

| Namespace | Domain |
|:-:|:-|
| `::Ex::Danmaku` | Danmaku (bullet hell) game base |
