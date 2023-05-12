# Renderable Object

## Layout
```TOML
{
	mesh = {
		data = "", // Vertex data as binary string
		layout = ['x', 'y', 'z', 'u', 'v', 'r', 'g', 'b', 'a', 'nx', 'ny', 'nz'], // Vector components in order they appear
		format = "float", // Vector components' type
		encoding = "base64", // Vertex data encoding
	},
	texture = {
		image = {			
			data = "", // Image data as binary string (maybe in base64?)
			width = 255, // Image width
			height = 255, // Image height
			type = "float", // Image data type
			channels = "rgba", // Image channels
		},
		clip = 0.1, // Texture alpha clipping
	},
	warp = {/* Same data as in texture */},
	material = {/* Options found in ObjectMaterial, except those that involve a texture */},
}
```

Undefined values are assumed to be their default setting.