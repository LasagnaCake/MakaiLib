import bpy
import struct
import os
from bpy.types import Operator
from bpy_extras.io_utils import ExportHelper
import fileprocessor as fp
import mathutils
import json

bl_info = {
	"name": "MSD (Makai Scene Definition) Exporter",
	"blender": (3, 4, 0),
	"category": "Import-Export",
}

class ExportMSDOperator(Operator, ExportHelper):
	bl_idname = "export_object.msd"
	bl_label = "Export as MSD"
	filename_ext = ""
	filetype = ".msd"

	apply_modifiers: bpy.props.BoolProperty(
		name="Apply Modifiers",
		description="Apply modifiers to mesh data",
		default=True,
	)

	embed_texture: bpy.props.BoolProperty(
		name="Embed Image Texture(s)",
		description="Embed image data into file",
		default=True,
	)
	
	tx_folder: bpy.props.StringProperty(
		name="Texture(s) Folder",
		description="Where to save the texure(s)",
		default="tx",
	)

	embed_mesh: bpy.props.BoolProperty(
		name="Embed Mesh(es)",
		description="Embed mesh data into file",
		default=True,
	)

	mesh_folder: bpy.props.StringProperty(
		name="Mesh(es) Folder",
		description="Where to save the mesh(es)",
		default="",
	)

	embed_objects: bpy.props.BoolProperty(
		name="Embed Object(s)",
		description="Embed object data into file",
		default=True,
	)

	camera_up_pos_y: bpy.props.BoolProperty(
		name="Camera 'up' is positive Y",
		description="Force camera's 'up' vector to point towards the positive Y axis",
		default=True,
	)

	def execute(self, context):
		objects = [obj for obj in bpy.data.objects if obj.type == "MESH"]
		print(f"Objects: {len(objects)}")
		fp.make_if_not_exists(self.filepath)
		camera = context.scene.camera
		cpos, crot, cscale = camera.matrix_world.decompose()
		cat = crot @ mathutils.Vector((0, -1, 0))
		cup = crot @ mathutils.Vector((0, 0, 1))
		scenedef = {
			"camera": {
				"eye": [cpos.x, cpos.y, cpos.z],
				"at": [cat.x, cat.y, cat.z],
				"up": [0.0, 1.0, 0.0] if self.camera_up_pos_y else [cup.x, cup.y, cup.z],
				"fov": camera.data.angle,
				"zNear": camera.data.clip_start,
				"zFar": camera.data.clip_end,
				"ortho": {
					"enabled": camera.data.type == "ORTHO",
					"origin": [0.0, 0.0],
					"size": [camera.data.ortho_scale, camera.data.ortho_scale]
				},
				"relativeToEye": True
			}
		}
		mist = context.scene.world.mist_settings
		scenedef["farFog"] = {
			"start": mist.start,
			"stop": mist.start + mist.depth,
			"strength": mist.intensity,
			"color": [
				mist.color[0],
				mist.color[1],
				mist.color[2],
				mist.color[3]
			],
			"enabled": camera.data.show_mist
		}
		if self.embed_objects:
			scenedef["data"] = []
		else:
			scenedef["data"] = {"path": []}
		for obj in objects:
			rendef = fp.create_render_definition(
				context,
				obj,
				self.filepath,
				self.tx_folder,
				self.mesh_folder,
				self.embed_texture,
				self.embed_mesh,
				self.apply_modifiers
			)
			if self.embed_objects:
				scenedef["data"].append(rendef)
			else:
				defpath = f"{self.filepath}\\{obj.name}\\{obj.name}.mrod"
				with open(defpath, "wt") as f:
					f.write(json.dumps(rendef, indent="	"))
				scenedef["data"]["path"].append({
					"source": f"{obj.name}.mrod",
					"type": "MROD"
				})
			with open(f"{self.filepath}\\scene.msd", "wt") as f:
				f.write(json.dumps(scenedef, indent="	"))
		return {'FINISHED'}

	def draw(self, context):
		layout = self.layout
		layout.prop(self, "apply_modifiers")
		layout.prop(self, "camera_up_pos_y")
		layout.prop(self, "embed_objects")
		layout.prop(self, "embed_texture")
		if not self.embed_texture:
			layout.prop(self, "tx_folder")
		layout.prop(self, "embed_mesh")
		if not self.embed_mesh:
			layout.prop(self, "mesh_folder")


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
	self.layout.operator(ExportMSDOperator.bl_idname, text="Makai Scene Definition (.msd)")


# Register and add to the "file selector" menu (required to use F3 search "Text Export Operator" for quick access).
def register():
	bpy.utils.register_class(ExportMSDOperator)
	bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
	bpy.utils.unregister_class(ExportMSDOperator)
	bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)