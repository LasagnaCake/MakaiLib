import bpy
import struct
import os
import sys
import inspect
from bpy import types as bt
from bpy import props as bp

bl_info = {
    "name": "Makai SceneMaker",
    "author": "LasagnaCake",
    "version": (0, 0, 1),
    "blender": (3, 4, 0),
    "location": "3D Viewport > Sidebar > [Item, Scene]",
    "description": "Custom scene builder & exporter for MakaiLib's custom scene format",
    "category": "Development"
}

def BoolProperty(prop_name, prop_default=False):
    return bp.BoolProperty(
        name=prop_name,
        default = prop_default
    )

def IntProperty(prop_name, prop_default=0, prop_min=sys.float_info.min, prop_max=sys.float_info.max):
    return bp.IntProperty(
        name=prop_name,
        default=prop_default,
        min=prop_min,
        max=prop_max
    )

def FloatProperty(prop_name, prop_default=0, prop_min=sys.float_info.min, prop_max=sys.float_info.max):
    return bp.FloatProperty(
        name=prop_name,
        default=prop_default,
        min=prop_min,
        max=prop_max
    )

def Vector2Property(prop_name, prop_default=(0, 0), prop_subtype="XYZ"):
    return bp.FloatVectorProperty(
        name=prop_name,
        size=2,
        default=prop_default,
        subtype=prop_subtype
    )

def Vector3Property(prop_name, prop_default=(0, 0, 0), prop_subtype="XYZ"):
    return bp.FloatVectorProperty(
        name=prop_name,
        size=3,
        default=prop_default,
        subtype=prop_subtype
    )

def ImageProperty(prop_name = "Image"):
    return bp.PointerProperty(
        name=prop_name,
        type=bt.Image
    )

def CameraProperty(prop_name = "Camera"):
    return bp.PointerProperty(
        name=prop_name,
        type=bt.Camera
    )

def RGBAColorProperty(prop_name = "Color", prop_default=(1,1,1,1)):
    return bp.FloatVectorProperty(
        name=prop_name,
        size=4,
        default=prop_default,
        min=0,
        max=1,
        subtype="COLOR"
    )

def RGBColorProperty(prop_name = "Color", prop_default=(1,1,1)):
    return bp.FloatVectorProperty(
        name=prop_name,
        size=3,
        default=prop_default,
        min=0,
        max=1,
        subtype="COLOR"
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
        update=prop_update
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

# Class stuff
    
class ObjectMaterialProperties(bt.PropertyGroup):
    color: RGBAColorProperty("Color")
    shaded: BoolProperty("Shaded")
    illuminated: BoolProperty("Illuminated")
    h: FloatProperty("Hue", 0, (-3.1415) * 2, (3.1415) * 2)
    s: FloatProperty("Saturation", 0, 0, 2)
    l: FloatProperty("Luminosity", 0, -1, 1)
    
    # Please avert your eyes in the following lines, I needed getmembers() to be ordered in this way
    # Why? because lazy (render_child function)
    
    texture_0_enabled: BoolProperty("Enable Texture")
    texture_1_image: ImageProperty("Texture Image")
    texture_2_alpha_clip: AlphaClipProperty("Texture")
    
    emission_0_enabled: BoolProperty("Enable Emission")
    emission_1_image: ImageProperty("Emission Image")
    emission_2_alpha_clip: AlphaClipProperty("Emission")
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
    # Hopefully that won't happen again
    
    culling: EnumProperty("Culling", ["Front and Back", "Front", "Back"])
    
    fill: EnumProperty("Fill", ["Fill", "Line", "Point"])
    
    def render_child(self, target, child):
        layout = target.layout
        for name, _ in inspect.getmembers(self):
            if child in name:
                layout.prop(self, name)
        layout.column()
                    
    
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

class ObjectBlendProperties(bt.PropertyGroup):  
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

class SceneProperties(bt.PropertyGroup):
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
    
    def render_child(self, target, child):
        layout = target.layout
        for name, _ in inspect.getmembers(self):
            if child in name:
                layout.prop(self, name)
        layout.column()
    
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

class SceneExportProperties(bt.PropertyGroup):
    
    embed_objects: BoolProperty("Embed Objects", True)
    embed_meshes: BoolProperty("Embed Meshes", True)
    embed_textures: BoolProperty("Embed Textures", True)
    
    def render(self, target):
        layout = target.layout
        layout.prop(self, "embed_objects")
        layout.prop(self, "embed_meshes")
        layout.prop(self, "embed_textures")
        layout.column()
        pass

class SCENE_OT_ExportSceneOperator(bt.Operator):
    bl_label = "Export Scene to File (.msd)"
    bl_idname = "scene.export_to_msd"

    def execute(self, props):
        return {"FINISHED"}

class ITEM_PT_SceneObjectMaterialPanel(bt.Panel):
    bl_label = "Scene Object Material Properties"
    bl_idname = "OBJECT_PT_SceneObjectMaterialPanel"
    bl_space_type = "VIEW_3D"   
    bl_region_type = "UI"
    bl_category = "Item"
    bl_context = "objectmode" 
    
    @classmethod
    def poll(self, context):
        return context.object is not None
    
    def draw(self, context):
        obj = context.object
        if obj.type != "MESH":
            return
        layout = self.layout
        material = context.object.material_props
        material.render(self)
        layout.column()

class ITEM_PT_SceneObjectBlendPanel(bt.Panel):
    bl_label = "Scene Object Blend Properties"
    bl_idname = "OBJECT_PT_SceneObjectBlendPanel"
    bl_space_type = "VIEW_3D"   
    bl_region_type = "UI"
    bl_category = "Item"
    bl_context = "objectmode"
    
    @classmethod
    def poll(self, context):
        return context.object is not None
    
    def draw(self, context):
        obj = context.object
        if obj.type != "MESH":
            return
        layout = self.layout
        blend = context.object.blend_props
        blend.render(self)
        layout.column()
        

class SCENE_PT_ScenePanel(bt.Panel):
    bl_label = "Scene Properties"
    bl_idname = "TOOL_PT_ScenePanel"
    bl_space_type = "VIEW_3D"   
    bl_region_type = "UI"
    bl_category = "Scene"
    bl_context = "objectmode"
    
    @classmethod
    def poll(self, context):
        return context.object is not None
    
    def draw(self, context):
        obj = context.object
        layout = self.layout
        blend = context.scene.scene_props
        blend.render(self)

class SCENE_PT_SceneExportPanel(bt.Panel):
    bl_label = "Scene Export Properties"
    bl_idname = "TOOL_PT_SceneExportPanel"
    bl_space_type = "VIEW_3D"   
    bl_region_type = "UI"
    bl_category = "Scene"
    bl_context = "objectmode"
    
    @classmethod
    def poll(self, context):
        return context.object is not None
    
    def draw(self, context):
        obj = context.object
        layout = self.layout
        blend = context.scene.scene_export_props
        blend.render(self)
        layout.operator("scene.export_to_msd", text="Export Scene")
        layout.column()

classes = (
    ObjectMaterialProperties,
    ObjectBlendProperties,
    SceneProperties,
    SceneExportProperties,
    SCENE_OT_ExportSceneOperator,
    ITEM_PT_SceneObjectMaterialPanel,
    ITEM_PT_SceneObjectBlendPanel,
    SCENE_PT_ScenePanel,
    SCENE_PT_SceneExportPanel
)
    

def register():
    from bpy.utils import register_class
    for cls in classes:
        register_class(cls)
    
    bt.Object.material_props = bp.PointerProperty(type=ObjectMaterialProperties)
    bt.Object.blend_props = bp.PointerProperty(type=ObjectBlendProperties)
    bt.Scene.scene_props = bp.PointerProperty(type=SceneProperties)
    bt.Scene.scene_export_props = bp.PointerProperty(type=SceneExportProperties)

def unregister():
    from bpy.utils import unregister_class
    for cls in reversed(classes):
        unregister_class(cls)
    del bt.Object.material_props
    del bt.Object.blend_props
    del bt.Scene.scene_props
    del bt.Scene.scene_export_props

if __name__ == "__main__":
    register()