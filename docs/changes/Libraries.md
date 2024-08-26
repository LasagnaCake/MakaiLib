# Solving the "External Libraries" issue

## Problem

Having to ship external libraries (the *.a files, in this case)

## Possible Solutions

- https://stackoverflow.com/a/2980126

- https://stackoverflow.com/a/23621751

- https://stackoverflow.com/a/8170851

### Possible `libtool` command

```
ibtool -static -o libmakai.a \
...
<every object file> \
...
lib/SDL2-2.0.10/lib/libSDL2.dll.a \
lib/SDL2-2.0.10/lib/libSDL2_mixer.dll.a \
lib/SDL2-2.0.10/lib/libSDL2_net.dll.a \
lib/SDL2-2.0.10/lib/libSDL2main.dll.a \
lib/OpenGL/GLEW/lib/libglew32.dll.a \
lib/cryptopp/lib/libcryptopp.a
```

### Possible MRI Script

Assuming all objects were combined into a library file called `libmakai.a`:

```mri
open libmakai.a
addlib libSDL2.dll.a
addlib libSDL2_mixer.dll.a
addlib libSDL2_net.dll.a
addlib libSDL2main.dll.a
addlib libglew32.dll.a
addlib libcryptopp.a
save
end
```
