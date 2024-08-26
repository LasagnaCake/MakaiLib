# Solving the "External Libraries" issue

## Problem
Having to ship external libraries (the *.a files, in this case)

## Solution

- https://stackoverflow.com/a/2980126

- https://stackoverflow.com/a/23621751

- https://stackoverflow.com/a/8170851

```mri
open libmakai.a
addlib libSDL2.dll.a
addlib libSDL2_mixer.dll.a
addlib libSDL2_net.dll.a
addlib libSDL2nain.dll.a
addlib libglew.a
save
end
```
