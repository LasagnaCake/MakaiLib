# Overview

This file contains the Following:

**Classes**
- `Renderable`
- `LineRenderable`
- `PointRenderable`

**`Renderable` loading functions**
- `loadObjectFromBinaryFile`
- `loadObjectFromGLTFFile`
- `loadObjectFromDefinition`
- `loadObjectFromDefinitionFile`

# Classes

## `Renderable`

The `Renderable` class, much like other classes in the `RenderData` namespace, is derived from the `DrawableObject` class[^1]. Thus, it inherits all its functions & variables.

It is defined as such:

```cpp
class Renderable: public Base::DrawableObject {
public:
	Renderable(
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual);

	Renderable(
		vector<Triangle*> triangles,
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual);

	Renderable(
		RawVertex* vertices,
		size_t count,
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual);

	Renderable(
		Renderable* other,
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual);

	virtual ~Renderable();

	template <Reference::PlaneType T>
	T* createReference();

	template <Reference::TrigonType T>
	T* createReference();

	template <Reference::PlaneType T>
	inline T* getReference(size_t index);

	template <Reference::TrigonType T>
	inline T* getReference(size_t index);
	
	template <Reference::PlaneType T>
	void removeReference(T* ref);

	template <Reference::TrigonType T>
	void removeReference(T* ref);
	
	template <Reference::PlaneType T>
	void unbindReference(T* ref);

	template <Reference::TrigonType T>
	void unbindReference(T* ref);
	
	void bakeAndLock();

	void extend(
		RawVertex* vertices,
		size_t size
	);

	void extend(Renderable* other);

	void extend(vector<Renderable*> parts);

	void extendFromBinaryFile(string const& path);

	void extendFromDefinition(nlohmann::json def);

	inline void extendFromDefinitionFile(string const& path);

	void bake();

	void unbake();

	void clearData();

	void saveToBinaryFile(string const& path);

	void saveToDefinitionFile(
		string const& path,
		string const& encoding = "base64",
		bool pretty = false,
		string const& binpath = ""
	);

	nlohmann::json getObjectDefinition(
		string const& encoding = "base64",
		bool integratedBinary = true
	);

	vector<Triangle*> triangles;
};
```

### Variables

- `triangles`: The triangles the object is composed of. Stored as a list of pointers towards them. It is referred as the *triangle buffer*. **Warning:** this list gets traversed and the triangles are deleted when the object is destroyed, so it is generally recommended to avoid using *shared and/or non-heap-allocated triangles*.

### Functions

#### `Renderable()`

Constructor.

1. `Renderable(size_t layer = 0, bool manual = false)`.
2. `Renderable(vector<Triangle*> triangles, size_t layer = 0, bool manual = false)`.
3. `Renderable(RawVertex* vertices, size_t count, size_t layer = 0, bool manual = false)`.
4. `Renderable(Renderable* other, size_t layer = 0, bool manual = false)`.

**Common Variables:**
- `layer`: The renderable layer it resides in.
- `manual`: Whether the renderable object is rendered automatically at *render time*[^2].

**2:**
- `triangles`: A list of triangles to store in the `Renderable`'s own *triangle buffer*.

**3:**
- `vertices`: A pointer to an array of vertices, to copy to the `Renderable`'s own *triangle buffer*.
- `count`: The array's size. Must be a multiple of 3.

**4:**
- `other`: Another renderable object, to copy its *triangle buffer* to the `Renderable`'s own *triangle buffer*.

#### `~Renderable()`

Destructor.

#### `createReference()`

Creates a renderable reference bound to the `Renderable` object. The first is ran for `Reference::Plane` objects and its derivatives, and the second is ran for `Reference::Trigon` objects and its derivatives[^3].

1. `template <Reference::PlaneType T> T* createReference()`.
2. `template <Reference::TrigonType T> T* createReference()`.

#### `getReference()`

Gets a reference bound to the `Renderable` object by its index. The first is ran for `Reference::Plane` objects and its derivatives, and the second is ran for `Reference::Trigon` objects and its derivatives[^3].

1. `template <Reference::PlaneType T> inline T* getReference(size_t index)`.
2. `template <Reference::TrigonType T> inline T* getReference(size_t index)`.

#### `removeReference()`

Deletes a reference bound to the `Renderable` object, including the triangle(s) associated with it. The first is ran for `Reference::Plane` objects and its derivatives, and the second is ran for `Reference::Trigon` objects and its derivatives[^3].

1. `template <Reference::PlaneType T> void removeReference(T* ref)`.
2. `template <Reference::TrigonType T> void removeReference(T* ref)`.

#### `unbindReference()`

Deletes a reference bound to the `Renderable` object, but keeps the triangle(s) associated to it. The first is ran for `Reference::Plane` objects and its derivatives, and the second is ran for `Reference::Trigon` objects and its derivatives[^3].

1. `template <Reference::PlaneType T> void unbindReference(T* ref)`.
2. `template <Reference::TrigonType T> void unbindReference(T* ref)`.

#### `bakeAndLock()`

Bakes and locks the object.

This does a few things:

1. Performs a `bake()` operation.
2. Deletes all references & triangles associated with the object.
3. Locks it, preventing any further mesh manipulation and reference creation.

**Warning:** This action is irreversible, so make sure any edits to the *triangle buffer* is made before this step.

#### `extend()`

Extends the `Renderable` object's triangle buffer with a given set of data.

1. `void extend(RawVertex* vertices, size_t size)`.
2. `void extend(Renderable* other)`.
3. `void extend(vector<Renderable*> parts)`.

**1:**
- `vertices`: A pointer to an array of vertices, to copy to the `Renderable`'s own *triangle buffer*.
- `count`: The array's size. Must be a multiple of 3.

**2:**
- `other`: Another renderable object, to copy its* triangle buffer* to the `Renderable`'s own *triangle buffer*.

**3:**
- `parts`: A list of pointers to `Renderable` objects, of which to copy their *triangle buffer*s from.

#### `extendFromBinaryFile()`

Extends the `Renderable` object's triangle buffer with the data located in a `.msbo` (Makai Simple Binary Object) or a `.mesh` file, or a binary file containing mesh data.

**Full:** `void extendFromBinaryFile(string const& path)`.

- `path`: The *relative* path to the file.

#### `extendFromDefinition()`

Extends the `Renderable` object with the data located in an *Object Definition*[^4].

**Full:** `void extendFromDefinition(nlohmann::json def)`.

- `def`: The *Object Definition*[^4].

#### `extendFromDefinitionFile()`

Extends the `Renderable` object with the data located in an `.mrod` (Makai Renderable Object Definition) file, or a `JSON`-formatted file containing an *Object Definition*[^4].

**Full:** `inline void extendFromDefinitionFile(string const& path)`.

- `path`: The *relative* path to the file.

#### `bake()`

Bakes the object.

Applies the transformations to all references attached, then copies the vertices of the triangles located in its *triangle buffer* into an internal vertex buffer. Also sets it into *baked* mode.

By default, every frame, the object does something similar to the process of "baking" and "unbaking", so that it can send its vertex data to the GPU for drawing. This procedure is necessary due to how the references and *triangle buffer* function.

This function "pre-loads" that step, which can speed up rendering time. But as a drawback, while in *baked* mode, any modification to the references bound to this object are ignored.

#### `unbake()`

Unbakes the object.

The opposite of the `bake()` function. Clears the internal vertex buffer, and disables *baked* mode.

#### `clearData()`

Deletes all references and triangles attached to this object. 

#### `saveToBinaryFile()`

Saves the data located in its *triangle buffer* into a binary file.

**Full:** `void saveToBinaryFile(string const& path)`.

- `path`: The *relative* path to the file.

#### `saveToDefinitionFile()`

Saves the `Renderable` object into an *Object Definition*[^4] file.

**Full:** `void saveToDefinitionFile(string const& path, string const& encoding = "base64", bool pretty = false, string const& binpath = "")`.

- `path`: The path to the file.
- `encoding`: The encoding used to save the mesh data, if it is to be saved embedded in the file.
- `pretty`: Self-explanatory.
- `binpath`: If it is to have a separate file for the mesh data, then this is set to that file's *relative* path. Else, it is to leave as is.

#### `getObjectDefinition()`

Converts the `Renderable` object into an *Object Definition*[^4].

**Full:** `nlohmann::json getObjectDefinition(string const& encoding = "base64", bool integratedBinary = true)`.

- `encoding`: The encoding used to save the mesh data, if it is to be saved embedded in the file.
- `integratedBinary`: Whether to include the mesh data in the *Object Definition*[^4].

## `LineRenderable`

### Variables

### Functions

## `PointRenderable`

### Variables

### Functions

# Functions

# Footnotes

[^1]: For more information, see [[gl_drawable]] → [gl_drawable.md](gl_drawable.md).

[^2]: For more information, see [[units]] → [units.md](../../info/units.md).

[^3]: For more information, see [[gl_reference]] → [gl_reference.md](gl_reference.md).

[^4]: For more information, see [[formats]] → [formats.md](../../specifications/formats.md).