import bpy
import struct
import os
from bpy.types import Operator
from bpy_extras.io_utils import ExportHelper
import json
import base64

bl_info = {
	"name": "MROD (Makai Renderable Object Definition) Exporter",
	"blender": (3, 4, 0),
	"category": "Import-Export",
}

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
					try:
						image_texture.save_render(image_path, None, 75)
					except:
						return "ERR_IMAGE_SAVE_FAILED"
					if image_packed:
						image_texture.pack()
					return "OK"
				return "ERR_COLOR_IS_NOT_TEXTURE"
			return "ERR_COLOR_NOT_CONNECTED"
		return "ERR_NODE_NOT_FOUND"
	return "ERR_MATERIAL_NONEXISTENT"

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

# Exports every mesh into separate files, into a folder of the name given to the file
# e.g. "You name a file and that file's name gets used as the name of the folder"
# Currently does not apply modifiers
class ExportMRODOperator(Operator, ExportHelper):
	bl_idname = "export_object.mrod"
	bl_label = "Export as MROD"
	filename_ext = ""
	filetype = ".mrod"

	apply_modifiers: bpy.props.BoolProperty(
		name="Apply Modifiers",
		description="Apply modifiers to mesh data",
		default=True,
	)

	embed_texture: bpy.props.BoolProperty(
		name="Embed Image Texture(s)",
		description="Embed image data into file",
		default=False,
	)
	
	tx_folder: bpy.props.StringProperty(
		name="Texture(s) Folder",
		description="Where to save the texure(s)",
		default="tx",
	)

	embed_mesh: bpy.props.BoolProperty(
		name="Embed Mesh(es)",
		description="Embed mesh data into file",
		default=False,
	)

	mesh_folder: bpy.props.StringProperty(
		name="Mesh(es) Folder",
		description="Where to save the mesh(es)",
		default="",
	)

	def execute(self, context):
		filepath = self.filepath
		objects = [obj for obj in bpy.data.objects if obj.type == "MESH"]
		print(f"Objects: {len(objects)}")
		make_if_not_exists(filepath)
		for obj in objects:
			mrodpath = self.filepath + "\\" + obj.name
			txpath = mrodpath + "\\" + self.tx_folder
			meshpath = mrodpath + "\\" + self.mesh_folder
			make_if_not_exists(txpath)
			make_if_not_exists(meshpath)
			dg = context.evaluated_depsgraph_get()
			mesh = None
			#TODO: fix this
			if self.apply_modifiers:
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
				"mesh": {
					"data": None,
					"components": component_data
				}
			}
			if self.embed_mesh:
				strfile["mesh"]["data"] = base64.b64encode(str(vertex_binary))
				strfile["mesh"]["encoding"] = "base64"
			else:
				with open(f"{meshpath}\\{obj.name}.mesh", "wb") as f:
					# write the binary data to the file
					f.write(vertex_binary)
				strfile["mesh"]["data"] = {"path": f"{self.mesh_folder}\\{obj.name}.mesh"}

			with open(f"{meshpath}\\{obj.name}.mrod", "wt") as f:
				pos, rot, scale = obj.matrix_world.decompose()
				rot = rot.to_euler('YXZ')
				strfile["trans"] = {
					"position": [pos.x, pos.y, pos.z],
					"rotation": [rot.x, rot.y, rot.z],
					"scale": [scale.x, scale.y, scale.z]
				}
				strfile["material"] = {
					"color": [1.0, 1.0, 1.0, 1.0],
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
				strfile["active"] = True
				if len(obj.material_slots) > 0:
					mat = obj.material_slots[0].material
					result = save_texture_to_image(mat, "Base Color", f"{txpath}\\texture.png")
					print(f"Texture: {result}")
					if result == "OK":
						strfile["texture"] = {
							"enabled": True,
							"image": {"path": f"{self.tx_folder}\\texture.png"},
							"alphaClip": 0.1
						}
					else:
						color_v = get_color(mat, "Base Color")
						if color_v is not None:
							strfile["material"]["color"] = [color_v[0], color_v[1], color_v[2], color_v[3]]
							result = save_texture_to_image(mat, "Emission", f"{txpath}\\emission.png")
					print(f"Emission: {result}")
					if result == "OK":
						strfile["emission"] = {
							"enabled": True,
							"image": {"path": f"{self.tx_folder}\\texture.png"},
							"alphaClip": 0.1
						}
				f.write(json.dumps(strfile, indent="	"))
			
		return {'FINISHED'}

	def draw(self, context):
		layout = self.layout
		layout.prop(self, "tx_folder")
		layout.prop(self, "embed_mesh")
		if not self.embed_mesh:
			layout.prop(self, "mesh_folder")
		#layout.prop(self, "apply_modifiers")


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
	self.layout.operator(ExportMRODOperator.bl_idname, text="Makai Renderable Object Definition (.mrod)")


# Register and add to the "file selector" menu (required to use F3 search "Text Export Operator" for quick access).
def register():
	bpy.utils.register_class(ExportMRODOperator)
	bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
	bpy.utils.unregister_class(ExportMRODOperator)
	bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
	register()
