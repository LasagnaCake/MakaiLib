## TODO

### Current
- [ ] Project creation recipe (`make project`)
- [ ] Add initializer lists wherever possible
- [ ] Add HSL (Hue-Saturation-Luminosity) & BC (Brightness-Contrast) to buffer mask effect
- [x] Separate `itembox` and `grazebox`
- [x] `data/` folder packing system
- [x] "Optimize" GPU usage
- - Instead of rendering every layer from the layerbuffer to the framebuffer, maybe implement sublayers?
- - ( ) A: Group every X layers to be rendered by clearing the depth bit in-between layers
- - - - Would play nicely with the sublayers in gamedata
- - - - Would still be somewhat resource-intensive, but not as much
- - (x) B: Create special functions to tell the system when to render the layerbuffer to the framebuffer
- - - - Would be called in the `onPreLayerDraw` step
- - - - Would also play nicely with the sublayers
- - - - Would also be the fastest, but more complicated on the end-user
- - - - This one would also play nicely without messing the current rendering pipeline too much
- - It is now up to the developer to figure out when to render their own layers, or to just just leave it like the old system
- [ ] Implement a framebuffer push-pop stack system, perhaps?
- - On second thought, this might be a stupid idea. Or not
- [x] Replace `.ini`-based savefile system with a different format
- - Using JSON
- [x] GUI system
- [x] Menu system
- [x] Dialogue system
- [ ] Curvy/Bent lasers
- [ ] Bugs & Errors (see [Issues.txt](../../Issues.txt))
- [ ] File format implementation (These need better names):
- - For more info, see the [specifications](../specifications) folder
- - [ ] Material configuration definitions
- - - [x] ObjectMaterial
- - - [x] WorldMaterial
- - - [ ] BufferMaterial
- - [x] Renderable object definition
- - - [x] Mesh Parser
- - - [x] Transform Parser
- - - [x] Material Parser
- - - - Requires ObjectMaterial definition
- - [ ] Animation keyframes definition
- - [ ] Danmaku stage layout definition
- - - Possibly via combining 2nd & 3rd implementations
- [ ] Embedded language (compiled (preferable, even if to bytecode), JIT or interpreted)
- - Currently have an idea, don't know if it would be feasible
- - Could also implement support for Squirrel/LUA/AngelScript
- - Or C# even, this one might be the more feasible option
- - - This one sounds enticing

### Future

- [ ] Other game types
- [ ] More "multi-purpose" game classes
- [ ] Cleanup & polish [entity](../../src/collection/entity/entity.hpp) system
- [ ] (Maybe) replace JSON to other specification
- - For the *custom file formats*, that is
- - See [File Format Alternatives Proposal](../../docs/changes/AltFormats.md) for more informaion
- [ ] Multi-platform support:
- - [ ] Linux
- [ ] Add [QOI](https://github.com/phoboslab/qoi/blob/master/qoi.h) support
- [ ] Add [QOA](https://github.com/phoboslab/qoa/blob/master/qoa.h) support
- [ ] Create `Label` class for fancy text support
- - ( ) BBCode
- - ( ) Custom text tags (defined via `[`*`chr-seq`*`:`*`text`*`]` - ex: `[*:abcde]`, `[_*:example]`, etc.)
- - - `*` for **bold**
- - - `/` for *italics*
- - - `+` for <ins>underline</ins>
- - - `-` for ~~strikethrough~~
- - - `^` for <sup>superscript</sup>
- - - `_` for <sub>subscript</sub>
- - - `@[link]` for [links](https://www.youtube.com/watch?v=ihCc2MoLF9k)
- - - - *`text`* is optional if this is tag included
- - - `=` for inserting values
- - - - To be loaded from a JSON structure attached to the `Label`, with `text` being the key
- - - `#[style]` for (limited) text stylization
- - - - `style` parameters are of the form `key=value`, and are separated via semicolons
- - - - Available parameters:
- - - - - `color`: Takes in a hex color (ex: `#00f`, `#f045cf0f`, `#fc0d`) as value
- - - - - `font`: Takes in an unsigned integer as a value
- - - - - - To be used as an index into an array of fonts
- - - "`[`" and "`]`" are escaped as "`[[`" and "`]]`"

### Major Refactor

See [Major Refactor](Refactor.md)

#### Under consideration

- Rename stuff

### Very far future (may never happen)

- Vulkanize graphical system (VERY DIFFICULT, might not be done)
- Maybe WebGPU instead?
