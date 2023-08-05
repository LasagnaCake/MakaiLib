import bpy
import struct
import os
from bpy.types import Operator
from bpy_extras.io_utils import ExportHelper
import fileprocessor as fp
import mathutils
import json

bl_info = {
	"name": "MROD (Makai Renderable Object Definition) Exporter",
	"blender": (3, 4, 0),
	"category": "Import-Export",
}

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

	def execute(self, context):
		objects = [obj for obj in bpy.data.objects if obj.type == "MESH"]
		print(f"Objects: {len(objects)}")
		fp.make_if_not_exists(self.filepath)
		for obj in objects:
			with open(f"{self.filepath}\\{obj.name}.mrod", "wt") as f:
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
				f.write(json.dumps(rendef, indent="	"))
			
		return {'FINISHED'}

	def draw(self, context):
		layout = self.layout
		layout.prop(self, "apply_modifiers")
		layout.prop(self, "embed_texture")
		if not self.embed_texture:
			layout.prop(self, "tx_folder")
		layout.prop(self, "embed_mesh")
		if not self.embed_mesh:
			layout.prop(self, "mesh_folder")


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
