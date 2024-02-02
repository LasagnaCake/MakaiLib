import bpy
import struct
import os
import sys
import inspect

from bpy import types as bt
from bpy import props as bp
from bpy import utils as bu

bl_info = {
    "name": "Makai SceneMaker",
    "author": "LasagnaCake",
    "version": (0, 0, 1),
    "blender": (3, 4, 0),
    "location": "3D Viewport > Sidebar > [Scene Object, Scene]",
    "description": "Custom scene builder & exporter for MakaiLib's scene format",
    "category": "Development"
}

"""
    ----------

    Properties

    ----------
"""

def BoolProperty(prop_name, prop_default=False):
    return bp.BoolProperty(
        name=prop_name,
        default=prop_default,
        options=set()
    )

def IntProperty(prop_name, prop_default=0, prop_min=sys.float_info.min, prop_max=sys.float_info.max):
    return bp.IntProperty(
        name=prop_name,
        default=prop_default,
        min=prop_min,
        max=prop_max,
        options=set()
    )

def FloatProperty(prop_name, prop_default=0, prop_min=sys.float_info.min, prop_max=sys.float_info.max):
    return bp.FloatProperty(
        name=prop_name,
        default=prop_default,
        min=prop_min,
        max=prop_max,
        options=set()
    )

def Vector2Property(prop_name, prop_default=(0, 0), prop_subtype="XYZ"):
    return bp.FloatVectorProperty(
        name=prop_name,
        size=2,
        default=prop_default,
        subtype=prop_subtype,
        options=set()
    )

def Vector3Property(prop_name, prop_default=(0, 0, 0), prop_subtype="XYZ"):
    return bp.FloatVectorProperty(
        name=prop_name,
        size=3,
        default=prop_default,
        subtype=prop_subtype,
        options=set()
    )

def ImageProperty(prop_name = "Image"):
    return bp.PointerProperty(
        name=prop_name,
        type=bt.Image,
        options=set()
    )

def CameraProperty(prop_name = "Camera"):
    return bp.PointerProperty(
        name=prop_name,
        type=bt.Camera,
        options=set()
    )

def RGBAColorProperty(prop_name = "Color", prop_default=(1,1,1,1)):
    return bp.FloatVectorProperty(
        name=prop_name,
        size=4,
        default=prop_default,
        min=0,
        max=1,
        subtype="COLOR",
        options=set()
    )

def RGBColorProperty(prop_name = "Color", prop_default=(1,1,1)):
    return bp.FloatVectorProperty(
        name=prop_name,
        size=3,
        default=prop_default,
        min=0,
        max=1,
        subtype="COLOR",
        options=set()
    )

def RangeProperty(prop_name, prop_default=0):
    return FloatProperty(prop_name, prop_default, 0, 1)

def AlphaClipProperty(prop_name):
    return RangeProperty(prop_name + " Alpha Clip", 0.2)

def EnumProperty(prop_name, prop_values, prop_default=0, prop_update=None, prop_index_offset=0):
    prop_items = [(f"enum_{i}", prop_values[i], "", i+prop_index_offset) for i in range(len(prop_values))]
    return bp.EnumProperty(
        name=prop_name,
        items=prop_items,
        default=prop_default,
        update=prop_update,
        options=set()
    )

def ChannelProperty(prop_name, prop_default=0, composite_channel=False):
    if composite_channel:
        return EnumProperty(
            prop_name,
            ["RGB Average", "Red", "Green", "Blue", "Alpha"],
            prop_default,
            None,
            -1
        )
    return EnumProperty(
        prop_name,
        ["Red", "Green", "Blue", "Alpha"],
        prop_default,
        None,
        0
    )

blend_equations = [
    "Zero",
    "One",
    "Add",
    "Subtract",
    "Reverse Subtract",
    "Min",
    "Max"
]

blend_functions = [
    "Zero",
    "One",
    "SRC color",
    "One minus SRC Color",
    "DST Color",
    "One Minus DST Color",
    "SRC Alpha",
    "One Minus SRC Alpha",
    "DST Alpha",
    "One Minus DST Alpha",
    "Constant Color",
    "One Minus Constant color",
    "Constant Alpha",
    "One Minus Constant alpha",
    "SRC Alpha Saturate"
]

def BlendFunctionProperty(prop_name, prop_default, prop_update=None):
    return EnumProperty(prop_name, blend_functions, prop_default, prop_update)

def BlendEquationProperty(prop_name, prop_default, prop_update=None):
    return EnumProperty(prop_name, blend_equations, prop_default, prop_update)

def DirectoryPathProperty(prop_name, prop_default="", prop_options=set()):
    return bp.StringProperty(
        name=prop_name,
        subtype="DIR_PATH",
        default=prop_default,
        options=prop_options
    )

def FileNameProperty(prop_name, prop_default="", prop_options=set()):
    return bp.StringProperty(
        name=prop_name,
        subtype="FILE_NAME",
        default=prop_default,
        options=prop_options
    )

"""
    ------------

    Base Classes
    
    ------------
"""

class BaseProperties(bt.PropertyGroup):
    def render_child(self, target, child):
        layout = target.layout
        props = [x for x,_ in inspect.getmembers(self)]
        if f"{child}0_enabled" in props:
            layout.prop(self, f"{child}0_enabled")
            if self.get(f"{child}0_enabled", False):
                for name in props:
                    if child in name and name != f"{child}0_enabled":
                        layout.prop(self, name)
        else:
            for name, _ in inspect.getmembers(self):
                if child in name:
                    layout.prop(self, name)
        layout.column()

class PropertyPanel(bt.Panel):
    bl_space_type = "VIEW_3D"   
    bl_region_type = "UI"
    bl_context = "objectmode" 
    bl_options = {"DEFAULT_CLOSED", "HEADER_LAYOUT_EXPAND"}

    @classmethod
    def poll(self, context):
        return context.object is not None
 
"""
    ---------------

    Property Groups
    
    ---------------

"""
    
class ObjectMaterialProperties(BaseProperties):
    color: RGBAColorProperty("Color")
    shaded: BoolProperty("Shaded")
    illuminated: BoolProperty("Illuminated")
    h: FloatProperty("Hue", 0, (-3.1415) * 2, (3.1415) * 2)
    s: FloatProperty("Saturation", 1, 0, 2)
    l: FloatProperty("Luminosity", 1, 0, 2)
    b: FloatProperty("Brightness", 0, -1, 1)
    c: FloatProperty("Contrast", 1, 0, 2)
    
    # Please avert your eyes in the following lines, I needed getmembers() to be ordered in this way
    # Why? because lazy (render_child function)
    
    texture_0_enabled: BoolProperty("Enable Texture")
    texture_1_image: ImageProperty("Texture Image")
    texture_2_alpha_clip: AlphaClipProperty("Texture")
    
    emission_0_enabled: BoolProperty("Enable Emission")
    emission_1_image: ImageProperty("Emission Image")
    emission_3_strength: RangeProperty("Emission Strength")
    
    warp_0_enabled: BoolProperty("Enable Texture Warp")
    warp_1_image: ImageProperty("Warp Image")
    warp_2_position: Vector2Property("Warp Position")
    warp_3_rotation: FloatProperty("Warp Rotation")
    warp_4_scale: Vector2Property("Warp Scale", (0, 0))
    warp_5_channelX: ChannelProperty("Warp Channel for X", 0)
    warp_6_channelY: ChannelProperty("Warp Channel for Y", 1)
    
    negative_0_enabled: BoolProperty("Enable Negative")
    negative_1_strength: RangeProperty("Negative Strength")
    
    gradient_0_enabled: BoolProperty("Enable Gradient")
    gradient_1_channel: ChannelProperty("Gradient Channel", -1, True)
    gradient_2_begin: RGBAColorProperty("Gradient Begin", (0,0,0,1))
    gradient_3_end: RGBAColorProperty("Gradient End", (1,1,1,1))
    gradient_4_invert: BoolProperty("Gradient Invert")
    
    # Ugliness over
    # Hopefully that won't happen again (clueless)
    
    culling: EnumProperty("Culling", ["Front and Back", "Front", "Back"])
    
    fill: EnumProperty("Fill", ["Fill", "Line", "Point"])              
    
    def render(self, target):
        layout = target.layout
        layout.label(text="Albedo")
        layout.prop(self, "color")
        layout.label(text="Shading")
        layout.prop(self, "shaded")
        layout.prop(self, "illuminated")
        layout.label(text="HSL Adjustments")
        layout.prop(self, "h")
        layout.prop(self, "s")
        layout.prop(self, "l")
        layout.label(text="Brightness & Contrast")
        layout.prop(self, "b")
        layout.prop(self, "c")
        layout.column()
        layout.label(text="Texture Effect")
        self.render_child(target, "texture_")
        layout.label(text="Emission Effect")
        self.render_child(target, "emission_")
        layout.label(text="Texture Warping Effect")
        self.render_child(target, "warp_")
        layout.label(text="Negative Effect")
        self.render_child(target, "negative_")
        layout.label(text="Gradient Effect")
        self.render_child(target, "gradient_")
        layout.label(text="Culling & Fill")
        layout.prop(self, "culling")
        layout.prop(self, "fill")

class ObjectBlendProperties(BaseProperties):  
    def update_src_func(self, context):
        self.func_src_alpha = self.func_src
        self.func_src_rgb = self.func_src
        
    def update_dst_func(self, context):
        self.func_dst_alpha = self.func_dst
        self.func_dst_rgb = self.func_dst
        
    def update_eq(self, context):
        self.eq_alpha = self.eq
        self.eq_rgb = self.eq
    
    func_sep: BoolProperty("Separate Blend Functions")
    eq_sep: BoolProperty("Separate Blend Equations")
    
    func_src: BlendFunctionProperty("SRC Function", 6, update_src_func)
    func_src_rgb: BlendFunctionProperty("SRC Color Function", 6)
    func_src_alpha: BlendFunctionProperty("SRC Alpha Function", 7)
    
    func_dst: BlendFunctionProperty("DST Function", 7, update_dst_func)
    func_dst_rgb: BlendFunctionProperty("DST Color Function", 0)
    func_dst_alpha: BlendFunctionProperty("DST Alpha Function", 1)
    
    eq: BlendEquationProperty("Equation", 2, update_eq)
    eq_rgb: BlendEquationProperty("Color Equation", 2)
    eq_alpha: BlendEquationProperty("Alpha Equation", 2)
    
    def render(self, target):
        layout = target.layout
        layout.label(text="Blend Function")
        layout.prop(self, "func_sep")
        if self.func_sep:
            layout.prop(self, "func_src_rgb")
            layout.prop(self, "func_src_alpha")
            layout.prop(self, "func_dst_rgb")
            layout.prop(self, "func_dst_alpha")
        else:
            layout.prop(self, "func_src")
            layout.prop(self, "func_dst")
        layout.column()
        layout.label(text="Blend Equation")
        layout.prop(self, "eq_sep")
        if self.eq_sep:
            layout.prop(self, "eq_rgb")
            layout.prop(self, "eq_alpha")
        else:
            layout.prop(self, "eq")

class ObjectExportProperties(BaseProperties):
    dir_path: DirectoryPathProperty("Output Folder", "", set(["OUTPUT_PATH"])) 
    file_name: FileNameProperty("Object File Name")

    apply_mods: BoolProperty("Apply Modifiers", True)
    embed_meshes: BoolProperty("Embed Meshes", True)
    embed_textures: BoolProperty("Embed Textures", True)
    no_hex_color: BoolProperty("Don't use Hex Color", False)
    
    tx_dir_path: DirectoryPathProperty("Mesh Folder", "tx", set(["OUTPUT_PATH"])) 
    mesh_dir_path: DirectoryPathProperty("Textures Folder", "mesh", set(["OUTPUT_PATH"]))

    def render(self, target):
        layout = target.layout
        layout.label(text="Path")
        layout.prop(self, "dir_path")
        layout.prop(self, "file_name")
        layout.label(text="Settings")
        layout.prop(self, "apply_mods")
        layout.prop(self, "embed_meshes")
        if not self.embed_meshes:
            layout.prop(self, "mesh_dir_path")
        layout.prop(self, "embed_textures")
        if not self.embed_textures:
            layout.prop(self, "tx_dir_path")
        layout.prop(self, "no_hex_color")
        layout.column()
        pass

class SceneProperties(BaseProperties):
    camera: CameraProperty("Scene Camera")
    
    # It returns...
    
    near_fog_0_enabled: BoolProperty("Enable Near Fog")
    near_fog_1_start: FloatProperty("Near Fog Start", 0, 0)
    near_fog_2_stop: FloatProperty("Near Fog Stop", 10, 0)
    near_fog_3_color: RGBAColorProperty("Near Fog Color")
    near_fog_4_strength: RangeProperty("Near Fog Strength", 1)
    
    far_fog_0_enabled: BoolProperty("Enable Far Fog")
    far_fog_1_start: FloatProperty("Far Fog Start", 0, 0)
    far_fog_2_stop: FloatProperty("Far Fog Stop", 10, 0)
    far_fog_3_color: RGBAColorProperty("Far Fog Color")
    far_fog_4_strength: RangeProperty("Far Fog Strength", 1)
    
    ambient_0_color: RGBColorProperty("Ambient Light Color", (1, 1, 1))
    ambient_1_strength: FloatProperty("Ambient Light Strength", 1, 0)
    
    # This is why you should never be lazy
    
    def render(self, target):
        layout = target.layout
        layout.prop(self, "camera")
        layout.column()
        layout.label(text="Near Fog Effect")
        self.render_child(target, "near_fog_")
        layout.label(text="Far Fog Effect")
        self.render_child(target, "far_fog_")
        layout.label(text="Ambient Light")
        self.render_child(target, "ambient_")

class SceneExportProperties(BaseProperties):
    
    dir_path: DirectoryPathProperty("Output Folder", "", set(["OUTPUT_PATH"])) 
    file_name: FileNameProperty("Scene File Name") 
    
    apply_mods: BoolProperty("Apply Modifiers", True)
    cam_up_pos_y: BoolProperty("Camera 'UP' is Positive Y", True)
    over_obj_export: BoolProperty("Override Object Export Properties", True)
    embed_objects: BoolProperty("Embed Objects", True)
    embed_meshes: BoolProperty("Embed Meshes", True)
    embed_textures: BoolProperty("Embed Textures", True)
    no_hex_color: BoolProperty("Don't use Hex Color", False)
    
    tx_dir_path: DirectoryPathProperty("Mesh Folder", "tx", set(["OUTPUT_PATH"])) 
    mesh_dir_path: DirectoryPathProperty("Textures Folder", "mesh", set(["OUTPUT_PATH"])) 
    
    def render(self, target):
        layout = target.layout
        layout.label(text="Path")
        layout.prop(self, "dir_path")
        layout.prop(self, "file_name")
        layout.label(text="Settings")
        layout.prop(self, "cam_up_pos_y")
        layout.prop(self, "over_obj_export")
        if self.over_obj_export:
            layout.prop(self, "apply_mods")
            layout.prop(self, "embed_objects")
            layout.prop(self, "embed_meshes")
            if not self.embed_meshes:
                layout.prop(self, "mesh_dir_path")
            layout.prop(self, "embed_textures")
            if not self.embed_textures:
                layout.prop(self, "tx_dir_path")
            layout.prop(self, "no_hex_color")
        layout.column()
        pass
"""
    ---------

    Operators

    ---------
"""

class EXPORT_OT_ExportSceneObjectOperator(bt.Operator):
    bl_label = "Export Scene Object to File (.mrod)"
    bl_idname = "object.export_to_mrod"

    def execute(self, props):
        return {"FINISHED"}

class EXPORT_OT_ExportSceneOperator(bt.Operator):
    bl_label = "Export Scene to File (.msd)"
    bl_idname = "scene.export_to_msd"

    def execute(self, props):
        return {"FINISHED"}

"""
    ------

    Panels

    ------
"""

class SCENEOBJECT_PT_SceneObjectMaterialPanel(PropertyPanel):
    bl_label = "Material Properties"
    bl_idname = "0_OBJECT_PT_SceneObjectMaterialPanel"
    bl_category = "Scene Object"
    
    def draw(self, context):
        obj = context.object
        if obj.type != "MESH":
            return
        layout = self.layout
        props = context.object.material_props
        props.render(self)
        layout.column()

class SCENEOBJECT_PT_SceneObjectBlendPanel(PropertyPanel):
    bl_label = "Blend Properties"
    bl_idname = "1_SCENEOBJECT_PT_SceneObjectBlendPanel"
    bl_category = "Scene Object"
    
    def draw(self, context):
        obj = context.object
        if obj.type != "MESH":
            return
        layout = self.layout
        props = context.object.blend_props
        props.render(self)
        layout.column()

class SCENEOBJECT_PT_SceneObjectExportPanel(PropertyPanel):
    bl_label = "Export Properties"
    bl_idname = "2_SCENEOBJECT_PT_SceneObjectExportPanel"
    bl_category = "Scene Object"
    
    def draw(self, context):
        obj = context.object
        if obj.type != "MESH":
            return
        layout = self.layout
        props = context.object.object_export_props
        props.render(self)
        layout.label(text="Export")
        layout.operator("object.export_to_mrod", text="Export Object")
        layout.column()
        

class SCENE_PT_ScenePanel(PropertyPanel):
    bl_label = "Scene Properties"
    bl_idname = "TOOL_PT_ScenePanel"
    bl_category = "Scene"
    
    def draw(self, context):
        obj = context.object
        layout = self.layout
        props = context.scene.scene_props
        props.render(self)

class SCENE_PT_SceneExportPanel(PropertyPanel):
    bl_label = "Export Properties"
    bl_idname = "TOOL_PT_SceneExportPanel"
    bl_category = "Scene"
    
    def draw(self, context):
        obj = context.object
        layout = self.layout
        props = context.scene.scene_export_props
        props.render(self)
        layout.label(text="Export")
        layout.operator("scene.export_to_msd", text="Export Scene")
        layout.column()

"""
    ------------------

    Class Registration

    ------------------
"""

classes = (
    ObjectMaterialProperties,
    ObjectBlendProperties,
    ObjectExportProperties,
    SceneProperties,
    SceneExportProperties,
    EXPORT_OT_ExportSceneOperator,
    EXPORT_OT_ExportSceneObjectOperator,
    SCENEOBJECT_PT_SceneObjectMaterialPanel,
    SCENEOBJECT_PT_SceneObjectBlendPanel,
    SCENEOBJECT_PT_SceneObjectExportPanel,
    SCENE_PT_ScenePanel,
    SCENE_PT_SceneExportPanel
)
    
def register():
    from bpy.utils import register_class
    for cls in classes:
        register_class(cls)
    
    bt.Object.material_props = bp.PointerProperty(type=ObjectMaterialProperties)
    bt.Object.blend_props = bp.PointerProperty(type=ObjectBlendProperties)
    bt.Object.object_export_props = bp.PointerProperty(type=ObjectExportProperties)
    bt.Scene.scene_props = bp.PointerProperty(type=SceneProperties)
    bt.Scene.scene_export_props = bp.PointerProperty(type=SceneExportProperties)

def unregister():
    from bpy.utils import unregister_class
    for cls in reversed(classes):
        unregister_class(cls)
    del bt.Object.material_props
    del bt.Object.blend_props
    del bt.Object.object_export_props
    del bt.Scene.scene_props
    del bt.Scene.scene_export_props


if __name__ == "__main__":
    register()