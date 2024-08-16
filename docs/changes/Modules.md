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

## File Naming Convention

| Name | Usage |
|:-:|:-|
|`.hxx`| Module interface |
|`.cxx`| Module implementation |
