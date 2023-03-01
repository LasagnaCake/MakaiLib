import bpy
import struct
from bpy.types import Operator
from bpy_extras.io_utils import ExportHelper

class ExportBinaryOperator(Operator, ExportHelper):
    bl_idname = "export_object.binary"
    bl_label = "Export as MSBO"
    filename_ext = ".msbo"

    def execute(self, context):
        "Export Object as Makai Simple Binary Object"
        filepath = self.filepath
        obj = bpy.context.active_object
        
        with open(filepath, "wb") as f:
            # iterate through the object's loop triangles to collect the vertex data
            vertex_data = []
            for loop_tri in obj.data.loop_triangles:
                for loop_index in loop_tri.loops:
                    vertex = obj.data.vertices[obj.data.loops[loop_index].vertex_index]
                    normal = vertex.normal
                    uv = obj.data.uv_layers.active.data[loop_index].uv if obj.data.uv_layers.active else (0, 0)
                    color = obj.data.vertex_colors.active.data[loop_index].color if obj.data.vertex_colors.active else (1, 1, 1, 1)
                    vertex_data.extend([vertex.co.x, vertex.co.y, vertex.co.z, uv[0], uv[1], color[0], color[1], color[2], color[3], normal.x, normal.y, normal.z])

            # pack the vertex data into a binary string
            vertex_binary = struct.pack("<" + "f"*len(vertex_data), *vertex_data)

            # write the binary data to the file
            f.write(vertex_binary)

        return {'FINISHED'}


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
