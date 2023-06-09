# Overview

The `Camera` namespace is defined as the such:

```cpp
namespace Camera {
    // Data Structures
    struct Camera3D {
		Vector3 eye;
		Vector3 at     = Vector3(0, 0, -1);
		Vector3 up     = Vector3(0, 1, 0);
		Vector2 aspect = Vector2(4.0, 3.0);
		float fov   = 2.0;
		float zNear = 0.01;
		float zFar  = 100.0;
		struct {
			bool enabled   = false;
			Vector2 origin = Vector2(0);
			Vector2 size   = Vector2(1);
		} ortho;
		bool relativeToEye = false;
	};
	// Functions
	Camera3D getCamera2D(Vector2 size);
	Camera3D getCamera2D(float scale, Vector2 ratio);
}
```

## Data Structures

### Camera3D

The base camera the engine uses. Contains the following member variables:

> The 2D camera is simply just a 3D camera with orthographic projection enabled.

**Physical Position**
- `eye`: The camera's position.
- `at`: Where the camera is "looking" at. Essentially, the camera's "target".
- `up`: Which direction the camera considers "Upwards".
- `relativeToEye`: Whether the camera's target position is relative to the camera's own position.

**Aperture & View Space**
- `aspect`: The camera's aspect ratio[^1]. The default camera's[^2] aspect ratio is set to the screen's aspect ratio.
- `fov`: The camera's *Field of View*.
- `zNear`: The camera's near clipping plane.
- `zFar`: The camera's far clipping plane.

**Orthography**
- `ortho.enabled`: Whether orthographic projection is enabled.
- `ortho.origin`: The orthographic projection's origin in 2D space.
- `ortho.size`: The orthographic projection's size in Vertex Units.

## Functions

### `getCamera2D(Vector2 size)`

Creates a 2D camera of a given size in Vertex Units[^3].

### `getCamera2D(float scale, Vector2 ratio)`

Creates a 2D camera of a given aspect ratio, at a specific scale in Vertex Units[^3].

The size of the view space is determined by the formula:
```math
\begin{bmatrix}
V_{width}\newline
V_{height}
\end{bmatrix}
=
\begin{bmatrix}
(scale \times \frac{R_{width}}{R_{height}})\newline
scale
\end{bmatrix}
```
Where $R_{width}$ is `ratio.x` and $R_{height}$ is `ratio.y`[^1].

# Footnotes

[^1]: If the aspect ratio is unknown, the values can be substituted for the resolution's pixel width and pixel height, which will give the same result as the aspect ratio.

[^2]: The "default camera" mentioned here is the camera included in the `GameData::GameApp` class and its subclasses.

[^3]: For more information, see [[units]] \([units.md](../../info/units.md)\)