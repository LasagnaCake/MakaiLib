# ImageData2D

## Layout
```JavaScript
"image": {
	"encoding": "base64", // Image data encoding
	"width": 255, // Image width
	"height": 255, // Image height
	"channels": "rgba", // Image pixel channels (Accepted: "d", "ds", "r", "rg", "rgb", "rgba")
	"type": "ubyte", // Image pixel channel type
	"data": "", // Image data as string
}
```

---

# Renderable Object

## Layout

```JavaScript
"object": {
	"mesh": {
		"layout": ['x', 'y', 'z', 'u', 'v', 'r', 'g', 'b', 'a', 'nx', 'ny', 'nz'], // Vector components in order they appear
		"format": "float", // Vector components' type
		"encoding": "base64", // Vertex data encoding
		"data": "", // Vertex data as string
	},
	"texture": {
		"image": {/* See ImageData2D:Layout */},
		"clip": 0.1, // Texture alpha clipping
	},
	// Optional
	"warp": {/* Same data as in texture */},
	"material": {/* Options found in ObjectMaterial, except those that involve a texture (does not have to be all of them) */},
}
```

Undefined values are assumed to be their default setting.

---