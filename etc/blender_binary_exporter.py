import bpy
import struct
from bpy.types import Operator
from bpy_extras.io_utils import ExportHelper

# Exports every mesh into separate files, into a folder of the name given to the file
# e.g. "You name a file and that file's name gets used as the name of the folder"
# Currently does not apply modifiers
class ExportBinaryOperator(Operator, ExportHelper):
    bl_idname = "export_object.msbo"
    bl_label = "Export as MSBO"
    filename_ext = ".msbo"

    apply_transforms: bpy.props.BoolProperty(
        name="Apply Transforms",
        description="Apply object transforms to mesh data",
        default=True,
    )

    apply_modifiers: bpy.props.BoolProperty(
        name="Apply Modifiers",
        description="Apply modifiers to mesh data",
        default=True,
    )

    def execute(self, context):
        filepath = self.filepath.replace('.msbo','')
        objects = [obj for obj in bpy.data.objects if obj.type == "MESH"]

        for obj in objects:
            filename = obj.name + self.filename_ext
            with open(f"{filepath}/{filename}", "wb") as f:
                dg = context.evaluated_depsgraph_get()
                mesh = None
                #TODO: fix this
                if self.apply_transforms:
                    mesh = obj.to_mesh(preserve_all_data_layers=True, depsgraph=dg)
                else:
                    mesh = obj.to_mesh(preserve_all_data_layers=False, depsgraph=dg)
                verts = mesh.vertices
                # iterate through the mesh's loop triangles to collect the vertex data
                vertex_data = []
                for loop_tri in mesh.loop_triangles:
                    for loop_index in loop_tri.loops:
                        vertex = mesh.vertices[mesh.loops[loop_index].vertex_index]
                        normal = vertex.normal
                        uv = mesh.uv_layers.active.data[loop_index].uv if mesh.uv_layers.active else (0, 0)
                        color = mesh.vertex_colors.active.data[loop_index].color if mesh.vertex_colors.active else (1, 1, 1, 1)

                        # apply object transforms to vertex position and normal
                        if self.apply_transforms:
                            vertex_pos = obj.matrix_world @ vertex.co
                            normal = (obj.matrix_world @ normal.to_4d()).to_3d()
                        else:
                            vertex_pos = vertex.co

                        vertex_data.extend([-vertex_pos.x, vertex_pos.z, vertex_pos.y, uv[0], 1-uv[1], color[0], color[1], color[2], color[3], -normal.x, normal.z, normal.y])

                # pack the vertex data into a binary string
                vertex_binary = struct.pack("<" + "f"*len(vertex_data), *vertex_data)

                # write the binary data to the file
                f.write(vertex_binary)

        return {'FINISHED'}

    def draw(self, context):
        layout = self.layout
        layout.prop(self, "apply_transforms")
        layout.prop(self, "apply_modifiers")


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportBinaryOperator.bl_idname, text="Makai Simple Binary Object (.msbo)")


# Register and add to the "file selector" menu (required to use F3 search "Text Export Operator" for quick access).
def register():
    bpy.utils.register_class(ExportBinaryOperator)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportBinaryOperator)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()
