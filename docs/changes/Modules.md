# Modules!

## But why?

See:
- [https://en.cppreference.com/w/cpp/language/modules](https://en.cppreference.com/w/cpp/language/modules)
- [https://www.geeksforgeeks.org/modules-in-cpp-20/](https://www.geeksforgeeks.org/modules-in-cpp-20/)
- [https://learn.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-170](https://learn.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-170)

TL,DR: Better header files!

## Drawbacks

- It's new
- How would it work with conventional libraries, like SDL, GLEW, CryptoPP, etc?
- - The issue isn't *really* the DLLs, it's more of the `.h` & `.o` files - do they also need to be shipped together (what I'm trying to avoid)?
- - Also, how do I even do (most of) the  value macros the engine uses?
- - - The macros in `collection/definitions.hpp` & `predefs.hpp` can just be ditched
- - - The game template inclusion macros will not be a problem (as they will be separated into extensions, making them unnecessary)
- - - - The macros in `gamedata/danmaku/predefs.hpp`, though, WILL be a problem (Could just also ditch them entirely)
- - - [ ] Could do something like this: [https://stackoverflow.com/a/75116431](https://stackoverflow.com/a/75116431)
- - - [x] Or just use command-line arguments (which means that if you want a different configuration, you'd have to recompile the source)
- - - - Which will end up being the case, anyways

## File Naming Convention

| Name | Usage |
|:-:|:-|
|`.hxx`| Module interface |
|`.cxx`| Module implementation |
