Undefined optional values are set to their defaults.

# Transform2D

## Layout

```JavaScript
"trans": {
	// Optional
	"position": [0.0, 0.0], // 2D vector representing position
	"rotation": 0.0,
	"scale": [1.0, 1.0], // 2D vector representing scale
}
```

# Transform3D

## Layout

```JavaScript
"trans": {
	// Optional
	"position": [0.0, 0.0, 0.0], // 3D vector representing position
	"rotation": [0.0, 0.0, 0.0], // 3D vector representing rotation
	"scale": [1.0, 1.0, 1.0], // 3D vector representing scale
}
```

---

# ImageData2D

## Layout

If image data is to be integrated in the file:

```JavaScript
"image": {
	// Required
	"encoding": "base64", // Image data encoding
	"format": "png", // Image data format
	"width": 255, // Image width
	"height": 255, // Image height
	"channels": "rgba", // Image pixel channels (Accepted: "d", "ds", "r", "rg", "rgb", "rgba")
	"type": "ubyte", // Image pixel channel type
	"data": "", // Image data as string
}
```

If image data is saved in a different file:

```JavaScript
"image": {
	// Required
	"path": "", // Path to image file
}
```

# ObjectMaterial

## Layout

```JavaScript
"material": {
	// Optional
	"color": [1.0, 1.0, 1.0, 1.0], // 4D vector representing the object's color
	"shaded": false,
	"illuminated": false,
	"hue": 0.0,
	"saturation": 1.0, // Ranges from -1.0 to 1.0
	"luminosity": 1.0, // Ranges from -1.0 to 1.0
	"texture": {
		"enabled": true,
		"image": {/* See ImageData2D:Layout */},
		"clip": 0.1, // Texture alpha clipping
	},
	"warp": {
		"enabled": true,
		"image": {/* See ImageData2D:Layout */},
		"trans": {/* See Transform2D:Layout */},
		"channelX": 0, // Index of the pixel's channel to be used for horizontal transformation
		"channelY": 1, // Index of the pixel's channel to be used for vertical transformation
	},
	"negative": {
		"enabled": true,
		"strength": 1.0,
	},
	"gradient": {
		"enabled": true,
		"channel": 0, // Index of the pixel's channel to be used for gradient transformation. If -1, an average of the RGB is used
		"invert": false,
		"begin": [0.0, 0.0, 0.0, 1.0], // 4D vector representing the start color
		"end": [1.0, 1.0, 1.0, 1.0], // 4D vector representing the end color
	},
	"instances": [
		[0.0, 0.0, 0.0], // 3D vector representing the instance's position
	], // A list of instances of the object (including itself)
	"culling": 0, // Culling mode (0: none, 1: front, 2: back)
	"fill": 0, // Fill mode (0: fill, 1: line, 2: point)
}
```

# Renderable Object

## Layout

This can be saved as a `JSON` file, or a file of extension `.mrod` (Makai Renderable Object Definition).

If data is to be integrated in the file:

```JavaScript
"object": {
	// Required
	"mesh": {
		"components": "x,y,z,u,v,r,g,b,a,nx,ny,nz", // Vector components in order they appear, separated by commas
		"encoding": "base64", // Vertex data's encoding
		"data": "", // Vertex data as string
	},
	// Optional
	"trans": {/* See Transform3D:Layout */},
	"material": {/* See ObjectMaterial:Layout */},
	"active": true,
}
```

If data gets saved in a different file:

```JavaScript
"object": {
	// Required
	"mesh": {
		"components": "x,y,z,u,v,r,g,b,a,nx,ny,nz", // Vector components in order they appear, separated by commas
		"data": {
			"path": "" // Path to ".mesh" file containing binary data
		},
	},
	// Optional
	"trans": {/* See Transform3D:Layout */},
	"material": {/* See ObjectMaterial:Layout */},
	"active": true,
}
```