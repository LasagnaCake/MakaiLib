# Overview

This file contains all information on the engine's units of measurments.

# Position & Size

Vertex Unit (VU) is the 2D & 3D coordinate unit the engine uses. This unit of measurement is independent of the screen's pixel resolution, therefore, any application will keep the same layout & position of its components regardless of screen size. The only thing that changes this behaviour is modifying the screen's aspect ratio to one different than the intended aspect ratio.

## 2D

When using the default 2D camera[^1], the view space has the size of:

$$
V_{width} = ( scale \times \frac{R_{width}}{R_{height}} )
\ ,\ 
V_{height} = scale
$$

With $A_{width}$ and $A_{height}$ being the aspect ratio's width and height, respectively[^2]<sup></sup>[^3].

For the default 2D camera[^1], its origin is at coordinates `[0, 0]` (top-left), and follows the right-handed coordinate system: `+X` goes right and `+Y` goes up.

## 3D

In 3D, much like in 2D,is also a right-handed system: `+X` goes right, `+Y` goes up, and `+Z` goes "forward" (towards the screen).

The default 3D camera[^1] starts at position `[0, 5, -10]` and points towards the world's origin `[0, 0, 0]`. That means that any object that is moving towards the positive X would appear to move towards the left, and any object that is moving towards the positive Z would appear to move backwards.

# Rotation

## 2D

Angles are determined in radians. They start pointing towards `+X` and move counterclockwise.

The `Math` namespace provides functions to convert to and from *degrees* to *radians*. They are also `constexpr` and `inline`, so there is no need to worry about speed when converting a constant expression (For example, `Math::degrees(1.5 * PI)` or `Math::radians(49.0)`).

## 3D

No god can save you.

# Time

Time is divided in two separate categories: *render time* and *logic time*. Of note:

- If `$_FRAME_DEPENDENT_PROCESS` has not been `#define`d, then the program and all entities gets processed during *logic time*. All timers & tweens are processed during *logic time*.
- If `$_PROCESS_RENDER_BEFORE_LOGIC` has not been `#define`d, then *logic time* happens before *render time*.

## Logic Time

When everything gets processed. It happens independently of *render time*, and is counted in *cycles*.

By default, it processes at *60 cycles per second*. The `speed` variable, defined in the `Makai::Program` class, affects it. This variable is automatically clamped to a value between 0 and 1.

## Render Time

When everything gets rendered. It happens independently of *logic time*, and is counted in *frames*.

By default, it processes at *60 frames per second*.

# Footnotes

[^1]: The "default camera" mentioned here is the camera included in the `GameData::GameApp` class and its subclasses.

[^2]: If the aspect ratio is unknown, the values can be substituted for the resolution's pixel width and pixel height, which will give the same result as the aspect ratio.

[^3]: For more information, see [[gl_camera]] \([gl_camera.md](../systems/graphical/gl_camera.md)\).