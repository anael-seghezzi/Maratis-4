# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

bl_info = {
    "name": "Maratis mesh",
    "author": "Anael Seghezzi",
    "version": (0,2),
    "blender": (2, 6, 6),
    "api": 31667,
    "location": "File > Import-Export > Maratis mesh ",
    "description": "Export Maratis mesh (.mesh format)",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Import-Export"}

import bpy
from bpy.props import *


try:
    imp.reload(export_maratis)
    
except:
    from maratis import export_maratis

#if "bpy" in locals():
#    import imp
#    if "export_maratis" in locals():
#        imp.reload(export_maratis)
        
#    from maratis import export_maratis  


class AnimList(bpy.types.PropertyGroup):
	pass

class AnimListItem(bpy.types.PropertyGroup):
	pass


def menu_export(self, context):
    from maratis import export_maratis
    import os
    default_path = os.path.splitext(bpy.data.filepath)[0] + ".mesh"
    self.layout.operator(export_maratis.RawExporter.bl_idname, text="Maratis mesh (.mesh)").filepath = default_path


def register():

    bpy.utils.register_module(__name__)
    #bpy.utils.register_class(AnimList)
	 
    Scene = bpy.types.Scene
    Mat = bpy.types.Material
	
	# Maratis material
    Mat.mar_mode = EnumProperty(
            items=[("2","CustomShader","use custom shader material"),
                   ("1","Standard","use standard material"),
                   ("0","Fixed","use fixed material")
                   ],
            name="Mode",
            description="select Maratis material mode",
            default="1")
            
    Mat.mar_blend_mode = EnumProperty(
            items=[("4","Product","product blending"),
                   ("3","Add","additional blending"),
                   ("2","Alpha","alpha blending"),
                   ("0","None","no blending")
                   ],
            name="BlendMode",
            description="select Maratis material blend mode",
            default="0")
            
    Mat.mar_vert_shad = StringProperty(
            name="VertShader", description="define vertex shader file",
            default="", subtype='FILE_PATH')
            
    Mat.mar_frag_shad = StringProperty(
            name="FragShader", description="define fragment shader file",
            default="", subtype='FILE_PATH')
            
    Mat.mar_zvert_shad = StringProperty(
            name="Z VertShader", description="define Z vertex shader file (optional optim)",
            default="", subtype='FILE_PATH')
            
    Mat.mar_zfrag_shad = StringProperty(
            name="Z FragShader", description="define Z fragment shader file (optional optim)",
            default="", subtype='FILE_PATH')           

    # Maratis render
    Scene.mar_anim_list = CollectionProperty(
        type=AnimList,
        name="Anim List",
        description="")
        
    Scene.mar_anim_list_index = IntProperty(
        name= "Anim List Index",
        default= -1,
        min= -1
        )

    AnimList.my_item = PointerProperty(
        type=  AnimListItem,
        name= "AnimListItem",
        description= ""
        )

    AnimListItem.start = IntProperty(
        name= "start",
        description= "start frame",
        default=0,
        min=0
       )

    AnimListItem.end = IntProperty(
        name= "end",
        description= "end frame",
        default=0,
        min=0
        )

    AnimListItem.loops = IntProperty(
        name= "loops",
        description= "loops",
        default=-1,
        min=-1
        )
         
    # export   
    bpy.types.INFO_MT_file_export.append(menu_export)

def unregister():

    bpy.utils.unregister_module(__name__)
    #bpy.utils.unregister_class(AnimList)
 
    Scene = bpy.types.Scene
    Mat = bpy.types.Material
    
    del Mat.mar_mode
    del Mat.mar_blend_mode
    del Mat.mar_vert_shad
    del Mat.mar_frag_shad
    del Mat.mar_zvert_shad
    del Mat.mar_zfrag_shad    
    del Scene.mar_anim_list
    del Scene.mar_anim_list_index
	 
    bpy.types.INFO_MT_file_export.remove(menu_export)

if __name__ == "__main__":
    register()
