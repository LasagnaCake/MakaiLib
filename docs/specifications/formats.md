Undefined optional values are set to their defaults.

NOTE: Any 4D vector representing color can also

# Transform2D

## Layout

```javascript
"trans": {
	// Required
	"position": [0.0, 0.0], // 2D vector representing position
	"rotation": 0.0,
	"scale": [1.0, 1.0], // 2D vector representing scale
}
```

# Transform3D

## Layout

```javascript
"trans": {
	// Required
	"position": [0.0, 0.0, 0.0], // 3D vector representing position
	"rotation": [0.0, 0.0, 0.0], // 3D vector representing rotation
	"scale": [1.0, 1.0, 1.0], // 3D vector representing scale
}
```

---

# ImageData2D

## Layout
If image is to be embedded in the file:

```javascript
"image": {
	// Required
	"data": "", // Image file data, encoded as string
	"encoding": "" // Image file data's encoding
	// Optional
	"minFilter": 0, // See GL's GL_TEXTURE_MIN_FILTER
	"magFilter": 0  // See GL's GL_TEXTURE_MAG_FILTER
}
```

If image is to be saved in a separate file:

```javascript
"image": {
	// Required
	data: {
		"path": "" // Path to image file, relative to object definition file
	},
	// Optional
	"minFilter": 0, // See GL's GL_TEXTURE_MIN_FILTER
	"magFilter": 0  // See GL's GL_TEXTURE_MAG_FILTER
}
```


# ObjectMaterial

## Layout

```javascript
"material": {
	// Optional
	"color": [1.0, 1.0, 1.0, 1.0], // 4D vector representing the object's color
	"shaded": false,
	"illuminated": false,
	"hue": 0.0,
	"saturation": 1.0, // Ranges from -1.0 to 1.0
	"luminosity": 1.0, // Ranges from -1.0 to 1.0
	"texture": {
		// Required
		"enabled": true,
		"image": {/* See ImageData2D:Layout */},
		"alphaClip": 0.1, // Texture alpha clipping
	},
	"emission": {
		// Required
		"enabled": true,
		"image": {/* See ImageData2D:Layout */},
		// Optional
		"alphaClip": 0.1, // Texture alpha clipping
		"strength": 1.0, // Emission strength
	},
	"warp": {
		// Required
		"enabled": true,
		"image": {/* See ImageData2D:Layout */},
		"trans": {/* See Transform2D:Layout */},
		// Optional
		"channelX": 0, // Index of the pixel's channel to be used for horizontal transformation
		"channelY": 1, // Index of the pixel's channel to be used for vertical transformation
	},
	"negative": {
		// Required
		"enabled": true,
		"strength": 1.0,
	},
	"gradient": {
		// Required
		"enabled": true,
		"channel": 0, // Index of the pixel's channel to be used for gradient transformation. If -1, an average of the RGB is used
		"invert": false,
		"begin": [0.0, 0.0, 0.0, 1.0], // 4D vector representing the start color
		"end": [1.0, 1.0, 1.0, 1.0], // 4D vector representing the end color
	},
	"instances": [
		// Required
		[0.0, 0.0, 0.0], // 3D vector representing the instance's position
	], // A list of instances of the object (including itself)
	"culling": 0, // Culling mode (0: none, 1: front, 2: back)
	"fill": 0, // Fill mode (0: fill, 1: line, 2: point)
}
```

# WorldMaterial

## FogEffect

### Layout
	
```javascript
"fog": {
	// Required
	"enabled": true,
	"start": 0.0, // The fog's start position (distance from camera)
	"stop": 10.0, // The fog's end position (distance from camera)
	"color": [0.0, 0.0, 0.0, 0.0], // 4D vector representing the fog's color
	"strength": 0.0 // The fog's strength
}
```

## Layout

```javascript
"material": {
	// Optional
	"nearFog": {/* See WorldMaterial:FogEffect:Layout */},
	"farFog": {/* See WorldMaterial:FogEffect:Layout */},
	"ambient": {
		// Required
		"color": [1.0, 1.0, 1.0], // eD vector representing the ambient light's color
		"strength": 1.0 // How bright the ambient light is
	}
}
```

# Renderable Object

## Layout

This can be saved as a `JSON` file of extension `.mrod` (Makai Renderable Object Definition).

If data is to be integrated in the file:

```javascript
"object": {
	// Required
	"mesh": {
		"components": "x,y,z,u,v,r,g,b,a,nx,ny,nz", // Vector components in order they appear, separated by commas
		"encoding": "base64", // Vertex data's encoding
		"data": "", // Vertex data as string
	},
	// Optional
	"version": "0.0.0" // The renderable definition version
	"trans": {/* See Transform3D:Layout */},
	"material": {/* See ObjectMaterial:Layout */},
	"active": true,
}
```

If data gets saved in a different file:

```javascript
"object": {
	// Required
	"mesh": {
		"components": "x,y,z,u,v,r,g,b,a,nx,ny,nz", // Vector components in order they appear, separated by commas
		"data": {
			"path": "" // Path to ".mesh" file containing binary data, relative to object definition file
		},
	},
	// Optional
	"version": "0.0.0" // The renderable definition version
	"trans": {/* See Transform3D:Layout */},
	"material": {/* See ObjectMaterial:Layout */},
	"active": true,
}
```
# Camera3D

## Layout

```javascript
"camera": {
	// Required
	"eye": [0.0, 0.0, 0.0],
	"at": [0.0, 0.0, -1.0],
	"up": [0.0, 1.0, 0.0],
	"aspect": [4.0, 3.0],
	"fov": 2.0,
	"zNear": 0.01,
	"zFar": 100.0,
	// Optional
	"ortho": {
		// Required
		"origin": [0, 0],
		"size": [1, 1],
		"enabled": false
	},
	"relativeToEye": false
}
```

# 3D Scene

## Layout

This can be saved as a `JSON` file of extension `.msd` (Makai Scene Definition).

If data gets saved in a separate file:

```javascript
"scene": {
	// Required
	"camera": {/* See Camera3D:Layout */}, // Scene's camera
	"world": {/* See WorldMaterial:Layout */}, // Scene's WorldMaterial
	"data": {
		"path": [{
			"source": "", // Object file source relative to file
			"type": "MROD" // Object file type
		}] // Paths to objects
	} 
	// Optional
	"version": "0.0.0" // The scene definition version
}
```

If data is to be integrated in the file:

```javascript
"scene": {
	// Required
	"camera": {/*See Camera3D:Layout */}, // Scene's camera
	"world": {/* See WorldMaterial:Layout */}, // Scene's WorldMaterial
	"data": [{/* See Renderable Object:Layout */}] // List of object definitions
	// Optional
	"version": "0.0.0" // The scene definition version
}
```

# Dialogue

## ActorData

### Layout

```javascript
"actor": {
	// Required
	"name": "",
	"frame": [0.0, 0.0],
	// Optional
	"tint": [1.0, 1.0, 1.0, 1.0],
	"leaving": false
}
```

## Layout

This can be saved as a `JSON` file of extension `.mdd` (Makai Dialogue Definition). This does not include actor sprite data, purely only message text data.

```javascript
"dialogue": {
	// Required
	"transitionTime": 100,
	"messages": [
		{
			// Required
			"title": "",
			// Required if title is not an action tag, else optional
			"text": "",
			// Optional
			"titleColor": [1.0, 1.0, 1.0, 1.0],
			"textColor": [1.0, 1.0, 1.0, 1.0],
			"actors": [{/* See Dialogue:ActorData:Layout */}],
			"easing": "out.cubic",
			"duration": 600,
			"autoplay": false
		}
	]
	// Optional, will be used if corresponding message parameter is not set
	"easing": "out.cubic",
	"duration": 600,
	"autoplay": false
}
``````