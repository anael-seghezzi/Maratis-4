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


import bpy
import sys, os, os.path, struct, math, string, copy
from bpy.props import *
from mathutils import *


# Material class
class Material:
    def __init__(self, material, mode, blend):
        self.bMaterial = material
        self.mode = mode
        self.blend = blend
        self.id = 0
        self.opacityKeys = []
        self.shininessKeys = []
        self.diffuseKeys = []
        self.specularKeys = []
        self.emitKeys = []

# Texture class
class Texture:
    def __init__(self, texture):
        self.bTexture = texture
        self.id = 0
        self.translateKeys = []
        self.scaleKeys = []
        
# Sub Vertex class
class SubVertex:
    def __init__(self, bVert):
        self.uvs = {}
        self.bVert = bVert
        
    def compare(self, vertex):
        for uvLayer in self.uvs:
            if not (self.uvs[uvLayer][0] == vertex.uvs[uvLayer][0] and self.uvs[uvLayer][1] == vertex.uvs[uvLayer][1]):
                return 0
        return 1
    
# BVertex class
class BVertex:
    def __init__(self):
        self.subIndices = []
        
# Display class
class Display:
    def __init__(self, begin, size, material, cullFace):
        self.begin = begin
        self.size = size
        self.material = material
        self.cullFace = cullFace
        
# Bone
class Bone:
    def __init__(self, id, bone):
        self.id = id
        self.bBone = bone
        self.posKeys = []
        self.rotKeys = []
        self.scaleKeys = []
        
# Bone link class
class BoneLink:
    def __init__(self, bone, weight):
        self.bone = bone   
        self.weight = weight
        
# Skin class
class Skin:
    def __init__(self, vertIndex):
        self.vertIndex = vertIndex
        self.bones = []          



# Export class
class Export:
    def __init__(self, filename, export_maa, export_mta, export_mma):
        self.meshFilename = bpy.path.ensure_ext(filename, ".mesh")
        self.maaFilename = self.meshFilename.replace(".mesh", ".maa", 1)
        self.mmaFilename = self.meshFilename.replace(".mesh", ".mma", 1)
        self.mtaFilename = self.meshFilename.replace(".mesh", ".mta", 1)
        
        self.export_maa = export_maa
        self.export_mta = export_mta
        self.export_mma = export_mma
        
        self.dir = os.path.dirname(self.meshFilename)
        self.materialsList = {}
        self.texturesList = {}
        self.bonesList = {}
        self.texSlotsList = {}
                
    def getLocalPath(self, path):
        if path[0] != self.dir[0]:
            return path
        else:
            filename = bpy.path.relpath(path, self.dir)
            if filename[:2] == "//":
                filename = filename[2:]
            return filename
            
    def findMin(self, min, point):
        _min = copy.copy(min)
        if point[0] < _min[0]:
            _min[0] = point[0]
        if point[1] < min[1]:
            _min[1] = point[1]
        if point[2] < min[2]:
            _min[2] = point[2]
        return _min	
            
    def findMax(self, max, point):
        _max = copy.copy(max)
        if point[0] > _max[0]:
            _max[0] = point[0]
        if point[1] > _max[1]:
            _max[1] = point[1]
        if point[2] > _max[2]:
            _max[2] = point[2]
        return _max	

    def exportMaratis(self):
    
        self.getMaterials()
        self.getTextures()
        self.getBones()
        
        self.exportMesh()
        if self.export_maa:
            self.exportMAA()
        if self.export_mta:
            self.exportMTA()
        if self.export_mma:
            self.exportMMA()

    def exportMesh(self):
        self.file = open(self.meshFilename, "w")
        
        self.file.write("<Maratis>\n\n")
        self.file.write("<Mesh>\n\n")	
        
        self.exportAnims()
        self.exportTextures()
        self.exportMaterials()
        self.exportBones()
        self.exportMeshs()
        
        self.file.write("</Mesh>\n\n")
        self.file.write("</Maratis>")
        
    def exportMAA(self):
        num_bones = len(self.bonesList)
        
        obj_list = bpy.context.selected_objects
        
        if num_bones > 0:
            startFrame = bpy.context.scene.frame_start
            endFrame = bpy.context.scene.frame_end
            
            # add keys
            for frame in range(startFrame, endFrame+1):
                bpy.context.scene.frame_set(frame)
            
                for obj in obj_list:
                    if obj.type == "ARMATURE":
                    
                        bones = obj.pose.bones
                    
                        for bone in bones:
                            mbone = self.bonesList[bone.name]
	                            
                            matrix = obj.matrix_world * bone.matrix
		
                            if bone.parent:
                                parentMatrix = obj.matrix_world * bone.parent.matrix
                                matrix = parentMatrix.inverted() * matrix
		
                            pos = matrix.to_translation()
                            rot = matrix.to_euler('XYZ')
                            scale = matrix.to_scale()
		                    
                            rot[0] = math.degrees(rot[0])
                            rot[1] = math.degrees(rot[1])
                            rot[2] = math.degrees(rot[2])
		                    
                            self.addKey(mbone.posKeys, (frame, pos))
                            self.addKey(mbone.rotKeys, (frame, rot))
                            self.addKey(mbone.scaleKeys, (frame, scale))
            
            # print keys
            self.maaFile = open(self.maaFilename, "w")
        
            self.maaFile.write("<Maratis>\n\n")
            self.maaFile.write("<ArmatureAnim num=\"%d\">\n\n" % num_bones)	
            
            for obj in obj_list:
                if obj.type == "ARMATURE":
                    bones = obj.pose.bones
                    for bone in bones:
                        mbone = self.bonesList[bone.name]
                        self.maaFile.write("\t<Bone id=\"%d\">\n" % mbone.id)     
                        self.printKeys(self.maaFile, "position", mbone.posKeys)
                        self.printKeys(self.maaFile, "rotation", mbone.rotKeys)
                        self.printKeys(self.maaFile, "scale", mbone.scaleKeys)
                        self.maaFile.write("\t</Bone>\n")
        
            self.maaFile.write("</ArmatureAnim>\n\n")
            self.maaFile.write("</Maratis>")        

    def exportMTA(self):
        num_textures = len(self.texturesList)
        
        if num_textures > 0:
            startFrame = bpy.context.scene.frame_start
            endFrame = bpy.context.scene.frame_end
            
            # add keys
            for frame in range(startFrame-1, endFrame+1):
                bpy.context.scene.frame_set(frame)
                
                for textureName in self.texturesList:
                    texture = self.texturesList[textureName]
                    bTexture = texture.bTexture
                
                    translate = bTexture.offset[0:2]
                    scale = bTexture.scale[0:2]
                    self.addKey(texture.translateKeys, (frame, translate))
                    self.addKey(texture.scaleKeys, (frame, scale))
                    
            # print keys
            mtaFile = open(self.mtaFilename, "w")
        
            mtaFile.write("<Maratis>\n\n")
            mtaFile.write("<TextureAnim num=\"%d\">\n\n" % num_textures)	
            
            id = 0
            for textureName in self.texturesList:
                texture = self.texturesList[textureName]
                
                mtaFile.write("\t<Texture id=\"%d\">\n" % id)
                self.printKeys(mtaFile, "translate", texture.translateKeys)
                self.printKeys(mtaFile, "scale", texture.scaleKeys)
                mtaFile.write("\t</Texture>\n")
                id=id+1
        
            mtaFile.write("</TextureAnim>\n\n")
            mtaFile.write("</Maratis>")        
            
    def exportMMA(self):
        num_materials = len(self.materialsList)
        
        if num_materials > 0:
            startFrame = bpy.context.scene.frame_start
            endFrame = bpy.context.scene.frame_end
            
            # add keys
            for frame in range(startFrame-1, endFrame+1):
                bpy.context.scene.frame_set(frame)
                
                for materialName in self.materialsList:
                    material = self.materialsList[materialName]
                    bMaterial = material.bMaterial
                
                    alpha = [bMaterial.alpha]
                    hardness = [bMaterial.specular_hardness]
                    diffuse = bMaterial.diffuse_color[0:3]
                    specular = copy.copy(bMaterial.specular_color)
                    specular[0] = specular[0] * bMaterial.specular_intensity
                    specular[1] = specular[1] * bMaterial.specular_intensity
                    specular[2] = specular[2] * bMaterial.specular_intensity
                    emit = copy.copy(bMaterial.diffuse_color)
                    emit[0] = emit[0] * bMaterial.emit
                    emit[1] = emit[1] * bMaterial.emit
                    emit[2] = emit[2] * bMaterial.emit
                
                    self.addKey(material.opacityKeys, (frame, alpha))
                    self.addKey(material.shininessKeys, (frame, hardness))
                    self.addKey(material.diffuseKeys, (frame, diffuse))
                    self.addKey(material.specularKeys, (frame, specular))
                    self.addKey(material.emitKeys, (frame, emit))
                    
            # print keys
            mmaFile = open(self.mmaFilename, "w")
        
            mmaFile.write("<Maratis>\n\n")
            mmaFile.write("<MaterialAnim num=\"%d\">\n\n" % num_materials)	
            
            id = 0
            for materialName in self.materialsList:
                material = self.materialsList[materialName]
                
                mmaFile.write("\t<Material id=\"%d\">\n" % id)
                self.printKeys(mmaFile, "opacity", material.opacityKeys)
                self.printKeys(mmaFile, "shininess", material.shininessKeys)
                self.printKeys(mmaFile, "diffuseColor", material.diffuseKeys)
                self.printKeys(mmaFile, "specularColor", material.specularKeys)
                self.printKeys(mmaFile, "emitColor", material.emitKeys)
                mmaFile.write("\t</Material>\n")
                        
                id=id+1
        
            mmaFile.write("</MaterialAnim>\n\n")
            mmaFile.write("</Maratis>")            
            
    def addKey(self, keys, key):
        num_keys = len(keys)
        num_comp = len(key[1])
            
        if num_keys == 0:
            keys.append(key)
            return
            
        for i in range(num_comp):
            if key[1][i] != keys[num_keys-1][1][i]:
                keys.append(key)
                return
                    
        if num_keys > 1:
            for i in range(num_comp):
                if key[1][i] != keys[num_keys-2][1][i]:
                    keys.append(key)
                    return
        
        keys[num_keys-1] = key
            
    def printKeys(self, file, name, keys):
        num_keys = len(keys)
        
        if num_keys > 0:
            num_comp = len(keys[0][1])
            comp_names = []
            
            if num_comp == 1:
                comp_names.append("value")
            else:
                comp_names.append("x")
                comp_names.append("y")
                comp_names.append("z")
                
            file.write("\t\t<%s num=\"%d\">\n" % (name, num_keys))
            
            for key in keys:
                file.write("\t\t\t<k t=\"%d\" " % key[0])
                for i in range(num_comp):
                    file.write("%s=\"%f\" " % (comp_names[i], key[1][i]))
                file.write("/>\n")
            
            file.write("\t\t</%s>\n" % name)

    def getMaterials(self):
        obj_list = bpy.context.selected_objects
        
        for obj in obj_list:
            if obj.type == "MESH":

                mesh = obj.data
                mesh.update(calc_tessface=1)
                if len(mesh.materials) > 0:
                
                    faces = mesh.tessfaces
                    for face in faces:
                        mat = face.material_index

                        mode = int(mesh.materials[mat].mar_mode)
                        blend = int(mesh.materials[mat].mar_blend_mode)
                            
                        material = mesh.materials[mat]
                        materialName = material.name
                        if not materialName in self.materialsList:
                            self.materialsList[materialName] = Material(material, mode, blend)
                else:
                    print("WARNING Maratis Export : Mesh needs a Material !\n")

    def getTextures(self):
        for materialName in self.materialsList:
            material = self.materialsList[materialName]
            bMaterial = material.bMaterial
            mtextures = bMaterial.texture_slots
                
            id = 0
            for mtex in mtextures:
                if mtex:
                    tex = mtex.texture
                    if mtex.use and tex.type == "IMAGE":
                        if tex.image:
                            name = bMaterial.name + "." + tex.name + (".%d" % id)
                            self.texturesList[name] = Texture(mtex)
                id = id+1

    def getBones(self):
        obj_list = bpy.context.selected_objects

        id = 0
        for obj in obj_list:
            if obj.type == "ARMATURE":
                
                bones = obj.pose.bones
                for bone in bones:
                    self.bonesList[bone.name] = Bone(id, bone)
                    id=id+1

    def exportAnims(self):
        sce = bpy.context.scene
        mar_anim_list = sce.mar_anim_list
		
        num_anims = len(mar_anim_list)
        if num_anims > 0 and (self.export_maa or self.export_mta or self.export_mma):
            self.file.write("<Animation>\n")
            
            if self.export_maa:
                self.file.write("\t<ArmatureAnim file=\"%s\" />\n" % self.getLocalPath(self.maaFilename))
            
            if self.export_mma:
                self.file.write("\t<MaterialAnim file=\"%s\" />\n" % self.getLocalPath(self.mmaFilename))
                
            if self.export_mta:
                self.file.write("\t<TextureAnim file=\"%s\" />\n" % self.getLocalPath(self.mtaFilename))
                
            self.file.write("\t<Anims num=\"%d\">\n" % num_anims)
            
            for i in range(num_anims):
                self.file.write("\t\t<anim id=\"%d\" name=\"%s\" start=\"%d\" end=\"%d\" loops=\"%d\" />\n" % (0, "", mar_anim_list[i].my_item.start, mar_anim_list[i].my_item.end, mar_anim_list[i].my_item.loops))
            
            self.file.write("\t</Anims>\n")
            
            self.file.write("</Animation>\n\n")

    def exportTextures(self):
        texturesSize = len(self.texturesList)
        if texturesSize > 0:
            self.file.write("<Textures num=\"%d\">\n" % texturesSize)
            
            id = 0
            for textureName in self.texturesList:
                texture = self.texturesList[textureName]
                bTexture = texture.bTexture
                texture.id = id
                
                path = bpy.path.abspath(bTexture.texture.image.filepath)
                filename = self.getLocalPath(path)

                mipmap = bTexture.texture.use_mipmap
                mapMode = 1
                tile = ("repeat", "repeat")
                if bTexture.texture.extension != "REPEAT":
                    tile = ("clamp", "clamp")
                translate = bTexture.offset
                scale = bTexture.scale
                rotate = 0.0
                
                self.file.write("\t<Texture id=\"%d\">\n" % id)
                
                self.file.write("\t\t<image filename=\"%s\" mipmap=\"%d\" />\n" % (filename, mipmap))
                self.file.write("\t\t<mapMode value=\"%d\" />\n" % mapMode)
                self.file.write("\t\t<tile u=\"%s\" v=\"%s\" />\n" % (tile[0], tile[1]))
                self.file.write("\t\t<translate x=\"%f\" y=\"%f\" />\n" % (translate[0], translate[1]))
                self.file.write("\t\t<scale x=\"%f\" y=\"%f\" />\n" % (scale[0], scale[1]))
                self.file.write("\t\t<rotate angle=\"%f\" />\n" % rotate)
                    
                self.file.write("\t</Texture>\n")
                
                id = id+1
                
            self.file.write("</Textures>\n\n")

    def exportMaterials(self):
        materialsSize = len(self.materialsList)
        if materialsSize > 0:
            self.file.write("<Materials num=\"%d\">\n" % materialsSize)
                
            id = 0
            for materialName in self.materialsList:
                material = self.materialsList[materialName]
                bMaterial = material.bMaterial
                material.id = id
                
                mode = material.mode
                blend = material.blend
                alpha = bMaterial.alpha
                hardness = bMaterial.specular_hardness
                diffuse = bMaterial.diffuse_color
                specular = copy.copy(bMaterial.specular_color)
                specular[0] = specular[0] * bMaterial.specular_intensity
                specular[1] = specular[1] * bMaterial.specular_intensity
                specular[2] = specular[2] * bMaterial.specular_intensity
                emit = copy.copy(bMaterial.diffuse_color)
                emit[0] = emit[0] * bMaterial.emit
                emit[1] = emit[1] * bMaterial.emit
                emit[2] = emit[2] * bMaterial.emit
                
                self.file.write("\t<Material id=\"%d\" type=\"%d\">\n" % (id, mode))
                
                self.file.write("\t\t<blend type=\"%d\" />\n" % blend)
                self.file.write("\t\t<opacity value=\"%f\" />\n" % alpha)
                self.file.write("\t\t<shininess value=\"%f\" />\n" % hardness)
                self.file.write("\t\t<customValue value=\"%f\" />\n" % (0.0))
                self.file.write("\t\t<diffuseColor r=\"%f\" g=\"%f\" b=\"%f\" />\n" % (diffuse[0], diffuse[1], diffuse[2]))
                self.file.write("\t\t<specularColor r=\"%f\" g=\"%f\" b=\"%f\" />\n" % (specular[0], specular[1], specular[2]))
                self.file.write("\t\t<emitColor r=\"%f\" g=\"%f\" b=\"%f\" />\n" % (emit[0], emit[1], emit[2]))
                self.file.write("\t\t<customColor r=\"%f\" g=\"%f\" b=\"%f\" />\n" % (0.0, 0.0, 0.0))
                
                if(mode == 2):
                    if(len(bMaterial.mar_vert_shad) > 0 and len(bMaterial.mar_frag_shad)):
                        vert = filename = self.getLocalPath(bpy.path.abspath(bMaterial.mar_vert_shad))
                        frag = filename = self.getLocalPath(bpy.path.abspath(bMaterial.mar_frag_shad))
                        self.file.write("\t\t<vertexShader file=\"%s\" />\n" % (vert))
                        self.file.write("\t\t<fragmentShader file=\"%s\" />\n" % (frag))
                    if(len(bMaterial.mar_zvert_shad) > 0 and len(bMaterial.mar_zfrag_shad)):
                        vert = filename = self.getLocalPath(bpy.path.abspath(bMaterial.mar_zvert_shad))
                        frag = filename = self.getLocalPath(bpy.path.abspath(bMaterial.mar_zfrag_shad))
                        self.file.write("\t\t<ZVertexShader file=\"%s\" />\n" % (vert))
                        self.file.write("\t\t<ZFragmentShader file=\"%s\" />\n" % (frag))

                # textures pass
                mtextures = bMaterial.texture_slots
                numPass = 0
                tPass = 0
                for mtex in mtextures:
                    if mtex:
                        tex = mtex.texture
                        if mtex.use and tex.type == "IMAGE":
                            if tex.image:
                                numPass = tPass+1
                    tPass = tPass+1
                            
                if numPass > 0:
                    self.file.write("\t\t<TexturesPass num=\"%d\">\n" % numPass)
                    
                    tPass = 0
                    for mtex in mtextures:
                        enabledTexture = 0
                        if mtex:
                            tex = mtex.texture
                            if mtex.use and tex.type == "IMAGE":
                                if tex.image:
                                    name = bMaterial.name + "." + tex.name + (".%d" % tPass)
                                    textureId = self.texturesList[name].id
                                
                                    mode = "modulate"
                                    if mtex.texture.use_normal_map:
                                        mode = "dot"
                                    elif mtex.blend_type == "SCREEN":
                                        mode = "replace"
                                    elif mtex.blend_type == "ADD":
                                        mode = "add"
                                    elif mtex.blend_type == "SUBTRACT":
                                        mode = "sub"
                                
                                    mapChannel = 0
                                    uvlayer = mtex.uv_layer
                                    if uvlayer:
                                        if uvlayer in self.texSlotsList:
                                            mapChannel = self.texSlotsList[uvlayer]
                                        else:
                                            mapChannel = len(self.texSlotsList)
                                            self.texSlotsList[uvlayer] = mapChannel
                                
                                    self.file.write("\t\t\t<texturePass id=\"%d\" texture=\"%d\" mode=\"%s\" mapChannel=\"%d\" />\n" % (tPass, textureId, mode, mapChannel))
                                    enabledTexture = 1
                                
                        if enabledTexture == 0:
                            self.file.write("\t\t\t<texturePass id=\"%d\" texture=\"%d\" />\n" % (tPass, -1))
                        tPass = tPass+1
                        if tPass >= numPass:
                            break
                                
                    self.file.write("\t\t</TexturesPass>\n")	
                            
                self.file.write("\t</Material>\n")
                id = id+1
            
            self.file.write("</Materials>\n\n")

    def exportBones(self):
        num_bones = len(self.bonesList)
                
        if num_bones > 0:
            self.file.write("<Bones num=\"%d\">\n" % num_bones)
            
            id = 0
            obj_list = bpy.context.selected_objects

            meshObjs = []
            for obj in obj_list:
                if obj.type == "ARMATURE":
                    
                    bones = obj.pose.bones
                    
                    for bone in bones:
                        self.file.write("\t<Bone id=\"%d\" name=\"%s\">\n" % (id, bone.name))
                        
                        matrix = obj.matrix_world * bone.matrix

                        if bone.parent:
                            pBone = self.bonesList[bone.parent.name]
                            parentMatrix = obj.matrix_world * bone.parent.matrix
                            matrix =  parentMatrix.inverted() * matrix
                            self.file.write("\t\t<parent id=\"%d\" />\n" % pBone.id)
    
                        position = matrix.to_translation()
                        rotation = matrix.to_euler('XYZ')
                        scale = matrix.to_scale()
                        
                        rotation[0] = math.degrees(rotation[0])
                        rotation[1] = math.degrees(rotation[1])
                        rotation[2] = math.degrees(rotation[2])
                            
                        self.file.write("\t\t<position x=\"%f\" y=\"%f\" z=\"%f\" />\n" % (position[0], position[1], position[2]))
                        self.file.write("\t\t<rotation x=\"%f\" y=\"%f\" z=\"%f\" />\n" % (rotation[0], rotation[1], rotation[2]))
                        self.file.write("\t\t<scale x=\"%f\" y=\"%f\" z=\"%f\" />\n" % (scale[0], scale[1], scale[2]))
                        
                        self.file.write("\t</Bone>\n")
                        id=id+1
                    
            self.file.write("</Bones>\n\n")
                    
    def computeTangent(self, v1, v2, v3, uv1, uv2, uv3, n):
        edge1 = v2 - v1
        edge2 = v3 - v1
        edge1uv = uv2 - uv1
        edge2uv = uv3 - uv1
		
        cp = edge1uv[1] * edge2uv[0] - edge1uv[0] * edge2uv[1]
		
        if (cp != 0.0):
            mul = 1.0 / cp
            tangent = (edge1 * -edge2uv[1] + edge2 * edge1uv[1]) * mul
            tangent = tangent.normalize()
            tangent -= n * tangent.dot(n)
            return tangent.normalize()
      		
        return Vector(0, 0, 0)
      	
    def exportMeshs(self):
        obj_list = bpy.context.selected_objects

        meshObjs = []
        for obj in obj_list:
            if obj.type == "MESH":
                    
                deformed_mesh = obj.to_mesh(bpy.context.scene, True, "PREVIEW")
                deformed_mesh.transform(obj.matrix_world)
                deformed_mesh.calc_normals()
                deformed_mesh.update(calc_tessface=1)

                points = deformed_mesh.vertices
                if len(points) > 0:
                    min = Vector((points[0].co[0], points[0].co[1], points[0].co[2]))
                    max = copy.copy(min)
                    for point in points:
                        transPt = Vector((point.co[0], point.co[1], point.co[2]))
                        min = self.findMin(min, transPt)
                        max = self.findMax(max, transPt)
                        
                    meshObjs.append((obj, (min, max), deformed_mesh))
    
        # sub meshes
        num_submeshs = len(meshObjs)
        if num_submeshs > 0:
            box = meshObjs[0][1]
            min = box[0]
            max = box[1]
            for meshObj in meshObjs:
                box = meshObj[1]
                min = self.findMin(min, box[0])
                max = self.findMax(max, box[1])				
                                                
            # global box
            self.file.write("<BoundingBox minx=\"%f\" miny=\"%f\" minz=\"%f\" maxx=\"%f\" maxy=\"%f\" maxz=\"%f\" />\n\n" % (min[0], min[1], min[2], max[0], max[1], max[2]))							
                                                
            self.file.write("<SubMeshs num=\"%d\">\n" % num_submeshs)
            
            subMeshId = 0
            for meshObj in meshObjs:

                obj = meshObj[0]
                box = meshObj[1]
                deformed_mesh = meshObj[2]
                
                min = box[0]
                max = box[1]
            
                bvertices = {}
                
                displays = []
                indices = []
                skins = []
                subVertices = []

                mesh = obj.data
                
                uvlayers = deformed_mesh.tessface_uv_textures
                num_uvlayers = len(uvlayers)
                
                # scan in order to Displays and split/sort vertices
                # (we split vertices according to uvs to construct unified buffers)
                for materialName in self.materialsList:
                    material = self.materialsList[materialName]
                    
                    # display start
                    start = len(indices)
                        
                    faces = deformed_mesh.tessfaces
                    numFaces = len(faces)
                    for face in faces:
                         
                        # check face material and two-side
                        matName = mesh.materials[face.material_index].name

                        if matName == materialName:
                            
                            verts = face.vertices
                            numVerts = len(verts)
                            subIndices = []
                            
                            vertOrder = (0, 1, 2)
                            if numVerts == 4:
                                vertOrder = (0, 1, 2, 3)
                                
                            # scan face points
                            for id in vertOrder:
                                index = verts[id]
                                vert = deformed_mesh.vertices[index]
                                subVert = SubVertex(vert)
                                
                                # add uv layers
                                if num_uvlayers > 0:
                                    for uvlayer in uvlayers:
                                        if len(uvlayer.data) == numFaces:
                                            subVert.uvs[uvlayer.name] = uvlayer.data[face.index].uv[id]
                                
                                # scan existing point for vertex spliting
                                # (we compare uvs according to layers)
                                same = 0
                                if index in bvertices:
                                    for subIndex in bvertices[index].subIndices:
                                        if subVertices[subIndex].compare(subVert):
                                            same = 1
                                            subIndices.append(subIndex)
                                            break
                                        
                                # add new vertex (also called sub vertex)
                                if not same:		
                                    subIndex = len(subVertices)
                                    subIndices.append(subIndex)
                                    subVertices.append(subVert)
                                    # add BVertex (BVertex are used to fast scanning)
                                    if not index in bvertices:
                                        bvertices[index] = BVertex()
                                    bvertices[index].subIndices.append(subIndex)
                                
                            # add indices
                            indices.append(subIndices[0])
                            indices.append(subIndices[1])
                            indices.append(subIndices[2])
                                
                            if numVerts == 4:
                                indices.append(subIndices[2])
                                indices.append(subIndices[3])
                                indices.append(subIndices[0])
    
                    # display end
                    size = len(indices) - start
                    if size > 0:
                        cullFace = 0
                        if mesh.show_double_sided:
                            cullFace = 2
                        displays.append(Display(start, size, material.id, cullFace))
            
                # skinning
                num_bones = len(self.bonesList)
                if num_bones > 0:
                    id = 0
                    for subVertex in subVertices:
                        vert = subVertex.bVert
                        
                        totalWeight = 0.0
                        skin = Skin(id)
                        for group in vert.groups:
                            if group.group < len(obj.vertex_groups):
                                groupName = obj.vertex_groups[group.group].name
                                if groupName in self.bonesList:
                                    if group.weight >= 0.0:
                                        skin.bones.append(BoneLink(self.bonesList[groupName].id, group.weight))
                                        totalWeight += group.weight
                            
                        if totalWeight > 0.0:
                            for bone in skin.bones:
                                bone.weight /= totalWeight # normalize
                            skins.append(skin)
                            
                        id=id+1
            
                # export
                num_vertices = len(subVertices)
                num_indices = len(indices)
                num_displays = len(displays)
                num_skins = len(skins)
                
                if num_indices > 0 and num_displays > 0:
                    self.file.write("\t<SubMesh id=\"%d\">\n" % subMeshId)
                    
                    # bounding box
                    self.file.write("\t\t<BoundingBox minx=\"%f\" miny=\"%f\" minz=\"%f\" maxx=\"%f\" maxy=\"%f\" maxz=\"%f\" />\n" % (min[0], min[1], min[2], max[0], max[1], max[2]))
                    
                    # vertices
                    self.file.write("\t\t<Vertices num=\"%d\">\n" % num_vertices)
                    for subVertex in subVertices:
                        self.file.write("\t\t\t<vertex x=\"%f\" y=\"%f\" z=\"%f\" />\n" % (subVertex.bVert.co[0], subVertex.bVert.co[1], subVertex.bVert.co[2]))
                    self.file.write("\t\t</Vertices>\n")
                    
                    # normals
                    self.file.write("\t\t<Normals num=\"%d\">\n" % num_vertices)
                    for subVertex in subVertices:
                        self.file.write("\t\t\t<normal x=\"%f\" y=\"%f\" z=\"%f\" />\n" % (subVertex.bVert.normal[0], subVertex.bVert.normal[1], subVertex.bVert.normal[2]))
                    self.file.write("\t\t</Normals>\n")
                
                    # texCoords
                    num_uvs = num_vertices*num_uvlayers
                    if num_uvs > 0:

                        # write channels infos
                        num_pass = 0
                        self.file.write("\t\t<TexCoords num=\"%d\" mapChannels=\"" % num_uvs)
                        for uvlayer in uvlayers:
                            slot = 0
                            if uvlayer.name in self.texSlotsList:
                                slot = self.texSlotsList[uvlayer.name]
                            else:
                                slot = len(self.texSlotsList)
                                self.texSlotsList[uvlayer.name] = slot
                            
                            if num_pass > 0:
                                self.file.write(" ")    
                            self.file.write("%d" % slot)
                            num_pass = num_pass + 1
                        self.file.write("\">\n")
                        
                        # write coords
                        for uvlayer in uvlayers:
                            for subVertex in subVertices:
                                self.file.write("\t\t\t<texCoord x=\"%f\" y=\"%f\" />\n" % (subVertex.uvs[uvlayer.name][0], 1.0 - subVertex.uvs[uvlayer.name][1]))
                        self.file.write("\t\t</TexCoords>\n")
                                                        
                    # indices
                    self.file.write("\t\t<Indices num=\"%d\">\n" % num_indices)
                    for index in indices:
                        self.file.write("\t\t\t<index value=\"%d\" />\n" % index)
                    self.file.write("\t\t</Indices>\n")	
                    
                    # skins
                    if num_skins > 0:
                        self.file.write("\t\t<Skins num=\"%d\">\n" % num_skins)
                        for skin in skins:
                            self.file.write("\t\t\t<skin vertex=\"%d\" numBones=\"%d\">\n" % (skin.vertIndex, len(skin.bones)))
                            for bone in skin.bones:
                                self.file.write("\t\t\t\t<bone id=\"%d\" weight=\"%f\" />\n" % (bone.bone, bone.weight))
                            self.file.write("\t\t\t</skin>\n")
                        self.file.write("\t\t</Skins>\n")	

                    # displays
                    self.file.write("\t\t<Displays num=\"%d\">\n" % num_displays)
                    for display in displays:
                        self.file.write("\t\t\t<display primType=\"triangles\" begin=\"%d\" size=\"%d\" material=\"%d\" cullFace=\"%d\" />\n" % (display.begin, display.size, display.material, display.cullFace))
                    self.file.write("\t\t</Displays>\n")	
                        
                    self.file.write("\t</SubMesh>\n")
                    subMeshId = subMeshId+1
                    
            self.file.write("</SubMeshs>\n\n")
            
            
            

class RawExporter(bpy.types.Operator):
    '''Save Maratis mesh'''
    bl_idname = "export_mesh.mesh"
    bl_label = "Export Maratis mesh"

    filepath = StringProperty(name="File Path", description="Filepath used for exporting the mesh file", maxlen= 1024, default= "")
    check_existing = BoolProperty(name="Check Existing", description="Check and warn on overwriting existing files", default=True, options={'HIDDEN'})

    export_maa = BoolProperty(name="Export Armature Anim", description="", default=True)
    export_mta = BoolProperty(name="Export Texture Anim", description="", default=True)
    export_mma = BoolProperty(name="Export Material Anim", description="", default=True)
    
    def execute(self, context):
        mesh = Export(self.filepath, self.export_maa, self.export_mta, self.export_mma)
        mesh.exportMaratis()
        return {'FINISHED'}

    def invoke(self, context, event):
        wm = context.window_manager
        wm.fileselect_add(self)
        return {'RUNNING_MODAL'}
               


        
###########################################################################################    
# UI / Maratis "RenderEngine"     
        
class PovrayRender(bpy.types.RenderEngine):
    bl_idname = 'MARATIS_RENDER'
    bl_label = "Maratis"


# render
from bl_ui import properties_render
properties_render.RENDER_PT_dimensions.COMPAT_ENGINES.add('MARATIS_RENDER')
del properties_render

# texture
from bl_ui import properties_texture
properties_texture.TEXTURE_PT_image.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_texture.TEXTURE_PT_image_mapping.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_texture.TEXTURE_PT_image_sampling.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_texture.TEXTURE_PT_mapping.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_texture.TEXTURE_PT_context_texture.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_texture.TEXTURE_PT_image.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_texture.TEXTURE_PT_influence.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_texture.TEXTURE_PT_preview.COMPAT_ENGINES.add('MARATIS_RENDER')
del properties_texture

# material
from bl_ui import properties_material
properties_material.MATERIAL_PT_context_material.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_material.MATERIAL_PT_diffuse.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_material.MATERIAL_PT_shading.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_material.MATERIAL_PT_specular.COMPAT_ENGINES.add('MARATIS_RENDER')
properties_material.MATERIAL_PT_transp_game.COMPAT_ENGINES.add('MARATIS_RENDER')
del properties_material

# data mesh
from bl_ui import properties_data_mesh
for member in dir(properties_data_mesh):
    subclass = getattr(properties_data_mesh, member)
    try:
        subclass.COMPAT_ENGINES.add('MARATIS_RENDER')
    except:
        pass
del properties_data_mesh


# Maratis render panel
class mar_anim_list_OT_add(bpy.types.Operator):
	bl_idname      = 'mar_anim_list.add'
	bl_label       = "Add anim"
	bl_description = "Add anim"

	def invoke(self, context, event):
		sce = context.scene
		mar_anim_list = sce.mar_anim_list
		mar_anim_list.add()
		mar_anim_list[-1].name= "Anim%d" % (len(mar_anim_list) - 1)
		return{'FINISHED'}


class mar_anim_list_OT_del(bpy.types.Operator):
	bl_idname      = 'mar_anim_list.remove'
	bl_label       = "Remove anim"
	bl_description = "Remove anim"

	def invoke(self, context, event):
		sce = context.scene
		mar_anim_list = sce.mar_anim_list
		if sce.mar_anim_list_index >= 0:
		   mar_anim_list.remove(sce.mar_anim_list_index)
		   sce.mar_anim_list_index-= 1
		   for i in range(len(mar_anim_list)):
		       mar_anim_list[i].name= "Anim%d" % (i)
		return{'FINISHED'}
		

class RenderButtonsPanel():
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "render"

    @classmethod
    def poll(cls, context):
        rd = context.scene.render
        return (rd.use_game_engine == False) and (rd.engine in cls.COMPAT_ENGINES)
        

class RENDER_PT_maratis(RenderButtonsPanel, bpy.types.Panel):
    bl_label = "Maratis Anims"
    COMPAT_ENGINES = {'MARATIS_RENDER'}

    def draw(self, context):
        layout= self.layout

        sce= context.scene

        split= layout.split()
        row= split.row()
        row.template_list("UI_UL_list", "20184", sce, 'mar_anim_list', sce, 'mar_anim_list_index', rows= 3)

        col= row.column(align=True)
        col.operator('mar_anim_list.add', text="", icon="ZOOMIN")
        col.operator('mar_anim_list.remove', text="", icon="ZOOMOUT")

        if sce.mar_anim_list_index >= 0 and len(sce.mar_anim_list) > 0:
            list_item = sce.mar_anim_list[sce.mar_anim_list_index]
	
            layout.separator()

            layout.prop(list_item.my_item, 'start')
            layout.prop(list_item.my_item, 'end')
            layout.prop(list_item.my_item, 'loops')
	
			





# Maratis material panel
class MaterialButtonsPanel():
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "material"

    @classmethod
    def poll(cls, context):
        mat = context.material
        rd = context.scene.render
        return mat and (rd.use_game_engine == False) and (rd.engine in cls.COMPAT_ENGINES)

class MATERIAL_PT_maratis(MaterialButtonsPanel, bpy.types.Panel):
    bl_label = "Maratis"
    COMPAT_ENGINES = {'MARATIS_RENDER'}

    def draw(self, context):
        
        layout = self.layout
        mat = context.material
        split = layout.split()
        col = split.column()
        col.prop(mat, "mar_mode")
        col.prop(mat, "mar_blend_mode")
        if mat.mar_mode == "2":
            col.prop(mat, "mar_vert_shad")
            col.prop(mat, "mar_frag_shad")
            col.prop(mat, "mar_zvert_shad")
            col.prop(mat, "mar_zfrag_shad")
                      
