# Vertex Unit

Vertex Unit (VU) is the measurement unit the engine uses. This unit of measurement is independent of the screen's pixel resolution, therefore, any application will keep the same layout & position of its components regardless of screen size. The only thing that changes this behaviour is modifying the screen's aspect ratio to one different than the intended aspect ratio.

Of note: The "default cameras" mentioned here are the cameras included in the `GameApp` located in the `GameData` namespace.

## 2D

When using the default 2D camera, the screen has the size of:

$$
\begin{bmatrix}
w\newline
h
\end{bmatrix}
=
\begin{bmatrix}
(64 \times \frac{A_{width}}{A_{height}})\newline
64
\end{bmatrix}
$$

With $A_{width}$ and $A_{height}$ being the aspect ratio's width and height, respectively. If that is unknow, the values can be substituted for the resolution's pixel width and pixel height, respectively, which will give the same result as the aspect ratio.

Whenever a new 2D camera is created (via the `Camera::getCamera2D()` set of functions), the following formula is used to determine its width:

$$
w = h \times \frac{A_{width}}{A_{height}}
$$

With $h$ being its height in VU.

For the default 2D camera, its origin is at coordinates `[0, 0]` (top-left), and follows the right-handed coordinate system: `+X` goes right and `+Y` goes up.

## 3D

In 3D, much like in 2D,is also a right-handed system: `+X` goes right, `+Y` goes up, and `+Z` goes "forward" (towards the screen).

The default 3D camera starts at position `[0, 5, -10]` and points towards the world's origin `[0, 0, 0]`. That means that any object that is moving towards the positive X would appear to move towards the left, and any object that is moving towards the positive Z would appear to move backwards.

# Angles

Angles are determined in radians. They start pointing towards `+X` and move counterclockwise.

The `Math` namespace provides functions to convert to and from *degrees* to *radians*. They are also `constexpr` and `inline`, so there is no need to worry about speed when converting a constant expression (e.g. `Math::degrees(1.5 * PI)` and `Math::radians(49.0)`).