# Codebase

## Layout

Root namespace name options:
- [x] `Makai` (Most likely)
- [ ] `MGE` (Just no.)
- [ ] `Mk` (Could also be used as the shorthand for the namespace)

---

| Symbol | Meaning |
|:-:|:-|
| root | The top-level namespace. |
| `::*` | Any namespace inside root, including root itself. |
| `::<NAME>` | A specific namespace inside root. |

---

| Namespace | Domain |
|:-:|:-|
| root | Core System |
| `::*::Part` | Full & Partial implementations |
| `::*::Base` | Class bases |
| `::Graph` | Graphical Subsystem |
| `::Audio` | Audio Subsystem |
| `::Game` | Game-related Stuff |
| `::Tool` | Engine tools |
| `::File` | File-related Stuff |
| `::JSON` | JSON-related Stuff |
| `::Data` | Data Processing Stuff |
| `::SLF` | SLF-related Stuff |
| `::Ex` | Extensions |

- Should `CTL` be included globally inside the namespace (i.e. `using namespace CTL;` inside root)?
- Everything from `program` folder → root folder (rename `Program` to `App`)
- Everything from `gamedata` folder (except game bases & related content) → `::Game`
- Game bases & related content → [see below](#Game-Bases)
- Some `*App`-related stuff from `gamedata` should be moved into the core system

### Game Bases

| Namespace | Domain |
|:-:|:-|
| `::Ex(::Game)::Danmaku` | Danmaku (bullet hell) game base |

### (Potential) Name Shorthands


| Namespace | Shorthand |
|:-:|:-|
| `::Graph(ical)` | `MkGraph` |
| `::Audio` | `MkAudio` |
| `::Game` | `MkGame` |
| `::Tool` | `MkTool` |
| `::Ex` | `MkEx` |
