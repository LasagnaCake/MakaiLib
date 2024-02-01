import bpy
import struct
import os
from bpy.types import Operator
from bpy_extras.io_utils import ExportHelper
import json
import base64

def make_if_not_exists(dir: str):
	if not os.path.isdir(dir):
		os.makedirs(dir)

def save_texture_to_image(material, node_type, file_path):
	# Get the material by name
	if material is not None:
		# Find the Principled BSDF node in the material's node tree
		principled_bsdf_node = None
		for node in material.node_tree.nodes:
			if node.type == "BSDF_PRINCIPLED":
				principled_bsdf_node = node
				break
		# Check if node was found
		if principled_bsdf_node is not None:
			# Check if the base color input is connected
			if principled_bsdf_node.inputs[node_type].is_linked:
				# Get the texture node connected to the base color input
				texture_node = principled_bsdf_node.inputs[node_type].links[0].from_node
				# Check if the texture node is an image texture
				if texture_node.type == "TEX_IMAGE":
					# Get the image used by the texture node
					image_texture = texture_node.image
					image_packed = False
					# Check if the image is not packed
					if image_texture.packed_file is not None:
						image_packed = True
						try:
							image_texture.unpack()
						except:
							return "ERR_IMAGE_UNPACKING_FAILED"
					# Save the image to a file (adjust the path as needed)
					image_path = file_path
					image_texture.file_format = "PNG"
					image_texture.save_render(image_path, scene=None, quality=75)
					if image_packed:
						image_texture.pack()
					return "OK"
				return "ERR_COLOR_IS_NOT_TEXTURE"
			return "ERR_COLOR_NOT_CONNECTED"
		return "ERR_NODE_NOT_FOUND"
	return "ERR_MATERIAL_NONEXISTENT"

def image_to_base64(path):
	with open(path, "rb") as image_file:
		return str(base64.b64encode(image_file.read()))[2:-1]

def as_hex_string(color):
	def to255(x): 
		return max(0, min(x*255, 255))

	"#{0:02x}{1:02x}{2:02x}{2:02x}".format(to255(color[0]), to255(color[1]), to255(color[2]), to255(color[3]))

def get_color(material, node_type):
	# Get the material by name
	if material is not None:
		# Find the Principled BSDF node in the material's node tree
		principled_bsdf_node = None
		for node in material.node_tree.nodes:
			if node.type == "BSDF_PRINCIPLED":
				principled_bsdf_node = node
				break
		# Check if node was found
		if principled_bsdf_node is not None:
			# Check if the base color input is connected
			if principled_bsdf_node.inputs[node_type].is_linked:
				# Get the texture node connected to the base color input
				color_node = principled_bsdf_node.inputs[node_type].links[0].from_node
				# Check if the texture node is an image texture
				if color_node.type.startswith("RGB"):
					return tuple(color_node.outputs["Color"].default_value)
				return None
			else:
				print(principled_bsdf_node.inputs[node_type].default_value)
				# Input is not connected, get the default value from the input socket
				return tuple(principled_bsdf_node.inputs[node_type].default_value)
			return None
		return None
	return None

def process_image_file(embed_texture, material, node_type, path, temp_path, relative_path = ""):
	if not embed_texture:
		result = save_texture_to_image(material, node_type, path)
		print(f"Texture: {result}")
		if result == "OK":
			return {
				"enabled": True,
				"image": {"path": relative_path},
				"alphaClip": 0.1
			}
	else:
		result = save_texture_to_image(material, node_type, temp_path)
		print(f"{node_type}: {result}")
		if result == "OK":
			imgstr = str(image_to_base64(temp_path))
			os.remove(temp_path)
			if imgstr is None:
				return None
			return {
				"enabled": True,
				"image": imgstr,
				"encoding": "base64",
				"alphaClip": 0.1
			}
	return None

def create_render_definition(context, obj, filepath, tx_folder, mesh_folder, embed_texture, embed_mesh, apply_modifiers, hexcolor):
	mrodpath = filepath + "\\" + obj.name
	txpath = mrodpath + "\\" + tx_folder
	meshpath = mrodpath + "\\" + mesh_folder
	if not embed_texture:
		make_if_not_exists(txpath)
	if not embed_mesh:
		make_if_not_exists(meshpath)
	dg = context.evaluated_depsgraph_get()
	mesh = None
	#TODO: fix this
	if apply_modifiers:
		mesh = obj.to_mesh(preserve_all_data_layers=False, depsgraph=dg)
	else:
		mesh = obj.to_mesh(preserve_all_data_layers=True, depsgraph=dg)
	verts = mesh.vertices
	# iterate through the mesh's loop triangles to collect the vertex data
	vertex_data = []
	component_data = "x,y,z"
	if mesh.uv_layers.active: component_data += ",u,v"
	if mesh.vertex_colors.active: component_data += ",r,g,b,a"
	component_data += ",nx,ny,nz"
	for loop_tri in mesh.loop_triangles:
		for loop_index in loop_tri.loops:
			vertex = mesh.vertices[mesh.loops[loop_index].vertex_index]
			normal = vertex.normal
			uv = mesh.uv_layers.active.data[loop_index].uv if mesh.uv_layers.active else None
			color = mesh.vertex_colors.active.data[loop_index].color if mesh.vertex_colors.active else None
			vertex_pos = vertex.co
			# Copy position data
			vtxdat = [-vertex_pos.x, vertex_pos.z, vertex_pos.y]
			# Get UV & color data, if applicable
			if mesh.uv_layers.active:
				vtxdat.extend([uv[0], 1-uv[1]])
			if mesh.vertex_colors.active:
				vtxdat.extend([color[0], color[1], color[2], color[3]])
			# Append normal data
			vtxdat.extend([-normal.x, normal.z, normal.y])
			# Append to array
			vertex_data.extend(vtxdat)
	# Pack binary
	vertex_binary = struct.pack("<" + "f"*len(vertex_data), *vertex_data)
	# Do appropriate procedure
	strfile = {
		"version": 0,
		"mesh": {
			"data": None,
			"components": component_data
		}
	}
	if embed_mesh:
		strfile["mesh"]["data"] = str(base64.b64encode(vertex_binary))[2:-1]
		strfile["mesh"]["encoding"] = "base64"
	else:
		with open(f"{meshpath}\\{obj.name}.mesh", "wb") as f:
			# write the binary data to the file
			f.write(vertex_binary)
		strfile["mesh"]["data"] = {"path": f"{mesh_folder}\\{obj.name}.mesh"}
	pos, rot, scale = obj.matrix_world.decompose()
	rot = rot.to_euler('YXZ')
	strfile["trans"] = {
		"position": [pos.x, pos.y, pos.z],
		"rotation": [rot.x, rot.y, rot.z],
		"scale": [scale.x, scale.y, scale.z]
	}
	strfile["material"] = {
		"color": "#FFFFFFFF" if hexcolor else [0.0, 0.0, 0.0, 0.0],
		"shaded": True,
		"illuminated": False,
		"hue": 0.0,
		"saturation": 1.0,
		"luminosity": 1.0,
		"instances": [
			[0.0, 0.0, 0.0],
		],
		"culling": 0,
		"fill": 0
	}
	strfile["blend"] = {
		"function": {
			"srcColor": 0x06, 
			"dstColor": 0x07,
			"srcAlpha": 0x01,
			"dstAlpha": 0x00
		},
		"equation": {
			"color": 0x02,
			"alpha": 0x06
		}
	}
	strfile["active"] = True
	if len(obj.material_slots) > 0:
		mat = obj.material_slots[0].material
		# Process emission txture
		imgsave = process_image_file(
			embed_texture,
			mat,
			0,
			f"{txpath}\\texture.png",
			f"{mrodpath}\\_tx_TMP.png",
			f"{tx_folder}\\texture.png"
		)
		if imgsave is not None:
			strfile["texture"] = imgsave
		else:
			cdat = get_color(mat, "Base Color")
			if cdat is not None:
				strfile["material"]["color"] = to_hex_string(cdat) if hexcolor else [cdat[0], cdat[1], cdat[2], cdat[3]]
		# Process image texture
		imgsave = process_image_file(
			embed_texture,
			mat,
			26,
			f"{txpath}\\emission.png",
			f"{mrodpath}\\_em_TMP.png",
			f"{tx_folder}\\emission.png"
		)
		if imgsave is not None:
			strfile["emission"] = imgsave
	return strfile;