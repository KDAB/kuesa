#!/usr/bin/env python3
#
# This file is part of Kuesa.
#
# Copyright (C) 2019-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Paul Lemire <paul.lemire@kdab.com>
#
# Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
# accordance with the Kuesa Enterprise License Agreement provided with the Software in the
# LICENSE.KUESA.ENTERPRISE file.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import sys
import re
import os
import json
import shutil
import pathlib

CURRENT_FILE_DIR = pathlib.Path(__file__).parent.absolute()

def read_template(name):
    with open(f'{CURRENT_FILE_DIR}/custom_material_templates/{name}.in', 'r') as f:
        return f.read()

class CustomMaterialGenerator:

    propertyTypesToQtType = { 'float': 'float',
                              'int'  : 'int',
                              'bool' : 'bool',
                              'color': 'QColor',
                              'vec2' : 'QVector2D',
                              'vec3' : 'QVector3D',
                              'vec4' : 'QVector4D',
                              'texture2d' : 'Qt3DRender::QAbstractTexture *' }

    includesForPropertyType = {
                              'vec2' : '#include <QVector2D>',
                              'vec3' : '#include <QVector3D>',
                              'vec4' : '#include <QVector4D>',
                              'color' : '#include <QColor>',
                              'texture2d' : '#include <Qt3DRender/QAbstractTexture>' }

    headerPrivateWarning = read_template("private_header_warning")

    headerContent = read_template("header")
    cppContent = read_template("cpp")

    propertiesClassHeaderContent = read_template("properties_class_header")
    propertiesClassCppContent = read_template("properties_class_cpp")

    shaderDataClassHeaderContent = read_template("shader_data_class_header")
    shaderDataClassCppContent = read_template("shader_data_class_cpp")

    effectClassHeaderContent = read_template("effect_class_header")
    effectClassCppContent = read_template("effect_class_cpp")

    techniqueOpaqueAndTransparent = read_template("technique_opaque_and_transparent")
    techniqueOpaqueOnly = read_template("technique_opaque_only")
    techniqueBackgroundOnly = read_template("technique_background_only")
    techniqueTransparentOnly = read_template("technique_transparent_only")
    techniqueMultiTransparent = read_template("technique_multi_transparent")
    techniqueMultiTransparentInnerPass = read_template("technique_multi_transparent_inner_pass")

    materialClassHeaderContent = read_template("material_class_header")
    materialClassCppContent = read_template("material_class_cpp")

    priContent = read_template("pri")

    def __init__(self, jsonDescriptionFilePath):
        self.jsonDescriptionFilePath = jsonDescriptionFilePath

    def parseDescription(self):
        try:
            with open(self.jsonDescriptionFilePath) as f:
                fileContent = f.read()
                self.rawJson = json.loads(fileContent)
                print(json.dumps(self.rawJson))
        except IOError as e:
            print("Couldn't open file (%s)." % e)

    def propertySetter(self, prop, declarationOnly = True, className = ""):
        doNotConvertToRefTypes = ["float", "int", "bool", "texture2d"]
        content = ""
        rawPropType = prop.get("type", "")
        propType = CustomMaterialGenerator.propertyTypesToQtType[rawPropType]
        paramType = ("%s " if rawPropType in doNotConvertToRefTypes else "const %s &") % (propType)
        propName = prop.get("name", "")
        if declarationOnly:
            content += " " * 4 + "void set%s(%s%s);\n" % (propName[0].upper() + propName[1:],
                                                          paramType,
                                                          propName)
        else:
            setterName = "%s::set%s" % (className, propName[0].upper() + propName[1:])
            contentStr = "void %s(%s%s)\n{\n    if (m_%s == %s)\n        return;\n"
            content += contentStr % (setterName,
                                        paramType,
                                        propName,
                                        propName,
                                        propName)
            if rawPropType.startswith("texture"):
                contentStr = """
    Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
    if (m_%s != nullptr)
        d->unregisterDestructionHelper(m_%s);
    m_%s = %s;
    if (m_%s != nullptr) {
        if (m_%s->parent() == nullptr)
            m_%s->setParent(this);
        d->registerDestructionHelper(m_%s, &%s, m_%s);
    }
    emit %sChanged(m_%s);
}

"""
                content += contentStr % (propName,
                                            propName,
                                            propName,
                                            propName,
                                            propName,
                                            propName,
                                            propName,
                                            propName,
                                            setterName,
                                            propName,
                                            propName,
                                            propName)

            else:
                contentStr = " " * 4 + "m_%s = %s;\n    emit %sChanged(%s);\n}\n\n"
                content += contentStr % (propName, propName, propName, propName)
        return content

    def propertySetters(self, properties, declarationOnly = True, className = ""):
        content = ""

        for prop in properties:
            content += self.propertySetter(prop, declarationOnly, className)
        return content

    def propertySettersShaderDataForwarding(self, properties, className = ""):
            content = ""

            doNotConvertToRefTypes = ["float", "int", "bool", "texture2d"]

            for prop in properties:
                rawPropType = prop.get("type", "")
                propType = CustomMaterialGenerator.propertyTypesToQtType[rawPropType]
                paramType = ("%s " if rawPropType in doNotConvertToRefTypes else "const %s &") % (propType)
                propName = prop.get("name", "")
                setterSig = "set%s" % (propName[0].upper() + propName[1:])
                content += "void %s::%s(%s%s)\n{\n    m_shaderData->%s(%s);\n}\n\n" % (className,
                                                                                    setterSig,
                                                                                    paramType,
                                                                                    propName,
                                                                                    setterSig,
                                                                                    propName)
            return content

    def propertyGetter(self, prop, declarationOnly = True, className = ""):
        content = ""
        propName = prop.get("name", "")
        propType = CustomMaterialGenerator.propertyTypesToQtType.get(prop.get("type", ""), "")
        if declarationOnly:
            content += " " * 4 + "%s %s() const;\n" % (propType, propName)
        else:
            contentStr = "%s %s::%s() const\n{\n    return m_%s;\n}\n\n"
            content += contentStr % (propType, className, propName, propName)
        return content

    def propertyGetters(self, properties, declarationOnly = True, className = ""):
        content = ""
        for prop in properties:
            content += self.propertyGetter(prop, declarationOnly, className)
        return content

    def propertyGettersShaderDataForwarding(self, properties, className):
        content = ""
        for prop in properties:
            propName = prop.get("name", "")
            rawPropType = prop.get("type", "")
            propType = CustomMaterialGenerator.propertyTypesToQtType.get(rawPropType, "")
            doc = prop.get("doc", "")
            qmlDocStr = "/*!\n    \\qmlproperty %s %s::%s\n    %s\n*/\n" % (propType,
                                                                            className,
                                                                            propName,
                                                                            doc)
            cppDocStr = "/*!\n    \\property %s::%s\n    %s\n*/\n" % (className,
                                                                      propName,
                                                                      doc)

            # We don't forward texture properties to the shader data
            # as UBO can't have sampler based members
            if rawPropType.startswith("texture"):
                content += self.propertyGetter(prop, False, className)
            else:
                contentStr = "%s%s%s %s::%s() const\n{\n    return m_shaderData->%s();\n}\n\n"
                content += contentStr % (qmlDocStr,
                                        cppDocStr,
                                        propType,
                                        className,
                                        propName,
                                        propName)
        return content


    def memberInitializations(self, properties):
        content = ""
        for prop in properties:
            propName = prop.get("name", "")
            propType = prop.get("type", "")
            content += "\n    , m_%s(%s)" % (propName,
                                            "nullptr" if propType.startswith("texture") else "")
        return content

    def propertySignals(self, properties):
        content = ""
        for prop in properties:
            propName = prop.get("name", "")
            propType = CustomMaterialGenerator.propertyTypesToQtType.get(prop.get("type", ""), "")
            content += " " * 4 + "void %sChanged(%s);\n" % (propName, propType)
        return content

    def propertyMembers(self, properties):
        content = ""
        for prop in properties:
            propName = prop.get("name", "")
            propType = CustomMaterialGenerator.propertyTypesToQtType.get(prop.get("type", ""), "")
            content += " " * 4 + "%s m_%s;\n" % (propType, propName)
        return content

    def propertyDeclarations(self, properties):
        content = ""
        for prop in properties:
            propName = prop.get("name", "")
            propType = CustomMaterialGenerator.propertyTypesToQtType.get(prop.get("type", ""), "")
            content += " " * 4 + "Q_PROPERTY(%s %s READ %s WRITE %s NOTIFY %s)\n" % (propType,
                                                                                     propName,
                                                                                     propName,
                                                                                     "set" + propName[0].upper() + propName[1:],
                                                                                     propName + "Changed")
        return content

    def includesForProperties(self, properties):
        uniqueIncludes = set()
        content = ""
        for prop in properties:
            jsonPropType = prop.get("type", "")
            incl = CustomMaterialGenerator.includesForPropertyType[jsonPropType] if jsonPropType in CustomMaterialGenerator.includesForPropertyType else ""
            if len(incl) > 0:
                uniqueIncludes.add(incl)
        for inc in sorted(uniqueIncludes):
            content += "%s\n" % (inc)
        return content

    def docForClass(self, className, inheritedClassName, docContent):
        docStr = """
/*!
    \class Kuesa::%s
    \inheaderfile Kuesa/%s
    \inherits %s
    \inmodule Kuesa
    \since Kuesa 1.3

    \\brief Kuesa::%s %s
*/

/*!
    \qmltype %s
    \instantiates Kuesa::%s
    \inqmlmodule Kuesa
    \since Kuesa 1.3

    \\brief Kuesa::%s %s
*/
"""
        return docStr % (className,
                         className,
                         inheritedClassName,
                         className,
                         docContent,
                         className,
                         className,
                         className,
                         docContent)

    def generateHeaderFile(self, content, className, includes = "", privateHeader = False, declare_metatype = False):
        headerFileName = className.lower() + ("_p" if privateHeader else "")
        includeGuardName = className.upper() + ("_P" if privateHeader else "")
        declareMetaType = "Q_DECLARE_METATYPE(Kuesa::%s*)" % (className)
        headerFileContent = CustomMaterialGenerator.headerContent % (headerFileName,
                                                                     includeGuardName,
                                                                     includeGuardName,
                                                                     CustomMaterialGenerator.headerPrivateWarning if privateHeader else "",
                                                                     includes,
                                                                     content,
                                                                     declareMetaType if declare_metatype else "",
                                                                     includeGuardName)
        with open(headerFileName + ".h", 'w') as f:
            f.write(headerFileContent)

    def generateCppFile(self, content, className, includes = ""):
        cppFileName = className.lower()
        headerFileContent = CustomMaterialGenerator.cppContent % (cppFileName,
                                                                  includes,
                                                                  content)
        with open(cppFileName + ".cpp", 'w') as f:
            f.write(headerFileContent)


    def generateShaderData(self):
        props = self.rawJson.get("properties", [])
        matName = self.rawJson.get("type", "")
        className = matName + "ShaderData"

        # Remove texture from props as ShaderData should only contain the scalar properties
        props = [p for p in props if not p.get("type", "").startswith("texture")]

        def generateHeader():
            setters = self.propertySetters(props, True)
            getters = self.propertyGetters(props, True)
            signals = self.propertySignals(props)
            members = self.propertyMembers(props)
            propDeclarations = self.propertyDeclarations(props)
            content = CustomMaterialGenerator.shaderDataClassHeaderContent % (matName,
                                                                              propDeclarations,
                                                                              matName,
                                                                              matName,
                                                                              getters,
                                                                              setters,
                                                                              signals,
                                                                              members)
            includes = self.includesForProperties(props)
            includes += "#include <Qt3DRender/QShaderData>"
            self.generateHeaderFile(content,
                                    className,
                                    includes,
                                    True)
        generateHeader()

        def generateCpp():
            initializations = self.memberInitializations(props)
            setters = self.propertySetters(props, False, className)
            getters = self.propertyGetters(props, False, className)
            includes = "#include \"%s\"\n" % (className.lower() + "_p.h")
            content = CustomMaterialGenerator.shaderDataClassCppContent % (matName,
                                                                           matName,
                                                                           initializations,
                                                                           matName,
                                                                           matName,
                                                                           getters,
                                                                           setters)
            self.generateCppFile(content,
                                 className,
                                 includes)
        generateCpp()

    def generateMaterialProperties(self):
        props = self.rawJson.get("properties", [])
        matName = self.rawJson.get("type", "")
        className = matName + "Properties"

        # Texture props are not stored in the shaderData
        texture_props = [p for p in props if p.get("type", "").startswith("texture")]

        def generateHeader():
            setters = self.propertySetters(props, True)
            getters = self.propertyGetters(props, True)
            signals = self.propertySignals(props)
            propDeclarations = self.propertyDeclarations(props)
            texture_members = self.propertyMembers(texture_props)
            content = CustomMaterialGenerator.propertiesClassHeaderContent % (matName,
                                                                              matName,
                                                                              propDeclarations,
                                                                              matName,
                                                                              matName,
                                                                              getters,
                                                                              setters,
                                                                              signals,
                                                                              matName,
                                                                              texture_members)
            includes = self.includesForProperties(props)
            includes += "#include <Kuesa/GLTF2MaterialProperties>\n"
            includes += "#include <Kuesa/kuesa_global.h>\n"

            self.generateHeaderFile(content,
                                    className,
                                    includes,
                                    declare_metatype=True)
        generateHeader()

        def generateCpp():
            # We don't forward texture properties to the shader data
            # as UBO can't have sampler based members

            texture_props = [p for p in props if p.get("type", "").startswith("texture")]
            shaderdata_props = [p for p in props if not p.get("type", "").startswith("texture")]

            setters = self.propertySettersShaderDataForwarding(shaderdata_props, className) + self.propertySetters(texture_props, False, className)
            getters = self.propertyGettersShaderDataForwarding(shaderdata_props, className) + self.propertyGetters(texture_props, False, className)
            initializations = self.memberInitializations(texture_props)

            def setDefaultEmptyTextures(props):
                if len(props) == 0:
                    return ""
                content = "#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)"
                for texture_prop in props:
                    propName = texture_prop.get("name", "")
                    content += "\n    set%s(new Empty2DTexture());" % (propName[0].upper() + propName[1:])
                content += "\n#endif"
                return content

            default_textures = setDefaultEmptyTextures(texture_props)

            connectionStatements = ""
            connectionStatementsStr = "    QObject::connect(m_shaderData, &%sShaderData::%sChanged, this, &%sProperties::%sChanged);\n"
            for p in shaderdata_props:
                propName = p.get("name", "")
                connectionStatements += connectionStatementsStr % (matName,
                                                                   propName,
                                                                   matName,
                                                                   propName)

            doc = self.docForClass(className,
                                   "Kuesa::GLTF2MaterialProperties",
                                   "holds the properties controlling the visual appearance of a %sMaterial instance." % (matName))

            content = CustomMaterialGenerator.propertiesClassCppContent % (doc,
                                                                           matName,
                                                                           matName,
                                                                           matName,
                                                                           initializations,
                                                                           connectionStatements + default_textures,
                                                                           matName,
                                                                           matName,
                                                                           matName,
                                                                           setters,
                                                                           getters)
            includes = "#include \"%sproperties.h\"\n" % (matName.lower())
            includes += "#include \"%sshaderdata_p.h\"\n" % (matName.lower())
            includes += "#include <Qt3DCore/private/qnode_p.h>\n"
            if len(texture_props) > 0:
                includes += "#include <Kuesa/private/empty2dtexture_p.h>\n"

            self.generateCppFile(content,
                                 className,
                                 includes)
        generateCpp()

    def generateTechnique(self):
        matName = self.rawJson.get("type", "")
        className = matName + "Technique"

        def generateHeader():
            passes = self.rawJson.get("passes", [])
            passes_info = []

            class PassInfo:
                gl3FragCode = ""
                es3FragCode = ""
                es2FragCode = ""
                rhiFragCode = ""
                gl3VertCode = ""
                es3VertCode = ""
                es2VertCode = ""
                rhiVertCode = ""
                gl3GeometryCode = ""
                es3GeometryCode = ""
                es2GeometryCode = ""
                rhiGeometryCode = ""
                es2VertexShaderGraph = ""
                es2FragmentShaderGraph = ""
                es3VertexShaderGraph = ""
                rhiVertexShaderGraph = ""
                es3FragmentShaderGraph = ""
                gl3VertexShaderGraph = ""
                gl3FragmentShaderGraph = ""
                rhiFragmentShaderGraph = ""
                blendFunction = ""
                blendSourceRGB = ""
                blendSourceAlpha = ""
                blendDestinationRGB = ""
                blendDestinationAlpha = ""
                pass_type = ""

            # Retrieve Pass information
            for render_pass in passes:
                pass_info = PassInfo()

                # Blending
                blending_obj = render_pass.get("blending", {})
                pass_info.blendFunction = blending_obj.get("function", "Add")
                pass_info.blendSourceRGB = blending_obj.get("sourceRGB", "SourceAlpha")
                pass_info.blendSourceAlpha = blending_obj.get("sourceAlpha", "SourceAlpha")
                pass_info.blendDestinationRGB = blending_obj.get("destinationRGB", "OneMinusSourceAlpha")
                pass_info.blendDestinationAlpha = blending_obj.get("destinationAlpha", "One")

                # Pass Type
                pass_info.pass_type = render_pass.get("type", "TransparentAndOpaque")

                # Shaders
                shaders =render_pass.get("shaders", [])
                for shader in shaders:
                    shaderType = shader.get("type", "")
                    shaderFormat = shader.get("format", {})
                    majorVersion = shaderFormat.get("major", 0)
                    api = shaderFormat.get("api", "")
                    code = shader.get("code", "")
                    graph = shader.get("graph", "")
                    if shaderType == "Fragment":
                        if api == "OpenGLES":
                            if majorVersion == 3:
                                pass_info.es3FragCode = code
                                pass_info.es3FragmentShaderGraph = graph
                            elif majorVersion == 2:
                                pass_info.es2FragCode = code
                                pass_info.es2FragmentShaderGraph = graph
                        elif api == "OpenGL":
                            if majorVersion == 3:
                                pass_info.gl3FragCode = code
                                pass_info.gl3FragmentShaderGraph = graph
                        elif api == "RHI":
                            if majorVersion == 1:
                                pass_info.rhiFragCode = code
                                pass_info.rhiFragmentShaderGraph = graph
                    elif shaderType == "Vertex":
                        if api == "OpenGLES":
                            if majorVersion == 3:
                                pass_info.es3VertCode = code
                                pass_info.es3VertexShaderGraph = graph
                            elif majorVersion == 2:
                                pass_info.es2VertCode = code
                                pass_info.es2VertexShaderGraph = graph
                        elif api == "OpenGL":
                            if majorVersion == 3:
                                pass_info.gl3VertCode = code
                                pass_info.gl3VertexShaderGraph = graph
                        elif api == "RHI":
                            if majorVersion == 1:
                                pass_info.rhiVertCode = code
                                pass_info.rhiVertexShaderGraph = graph
                    elif shaderType == "Geometry":
                        if api == "OpenGLES":
                            if majorVersion == 3:
                                pass_info.es3GeometryCode = code
                            # ES2 has no geometry shader support
                        elif api == "OpenGL":
                            if majorVersion == 3:
                                pass_info.gl3GeometryCode = code
                        elif api == "RHI":
                            if majorVersion == 1:
                                pass_info.rhiGeometryCode = code
                passes_info.append(pass_info)


            # Find out what type of technique we need to create based on our passes info

            if len(passes_info) == 0:
                print("No passes found in Material description")
                return
            if len(passes_info) != 1:
                for pass_info in passes_info:
                    if pass_info.pass_type != "MultiPassTransparent":
                        print("Generator only handle Multiple Passes of type MultiPassTransparent")
                        return

            def transparentAndOpaqueTechnique(passes_info):
                pass_info = passes_info[0]
                return CustomMaterialGenerator.techniqueOpaqueAndTransparent % (matName,
                                                                                matName,
                                                                                pass_info.gl3VertexShaderGraph,
                                                                                pass_info.es3VertexShaderGraph,
                                                                                pass_info.es2VertexShaderGraph,
                                                                                pass_info.rhiVertexShaderGraph,
                                                                                pass_info.gl3FragmentShaderGraph,
                                                                                pass_info.es3FragmentShaderGraph,
                                                                                pass_info.es2FragmentShaderGraph,
                                                                                pass_info.rhiFragmentShaderGraph,
                                                                                pass_info.gl3VertCode,
                                                                                pass_info.es3VertCode,
                                                                                pass_info.es2VertCode,
                                                                                pass_info.rhiVertCode,
                                                                                pass_info.gl3FragCode,
                                                                                pass_info.es3FragCode,
                                                                                pass_info.es2FragCode,
                                                                                pass_info.rhiFragCode,
                                                                                pass_info.gl3GeometryCode,
                                                                                pass_info.es3GeometryCode,
                                                                                pass_info.es2GeometryCode,
                                                                                pass_info.rhiGeometryCode,
                                                                                pass_info.blendFunction,
                                                                                pass_info.blendSourceRGB,
                                                                                pass_info.blendSourceAlpha,
                                                                                pass_info.blendDestinationRGB,
                                                                                pass_info.blendDestinationAlpha)

            def transparentOnlyTechnique(passes_info):
                pass_info = passes_info[0]
                return CustomMaterialGenerator.techniqueTransparentOnly % (matName,
                                                                           matName,
                                                                           pass_info.gl3VertexShaderGraph,
                                                                           pass_info.es3VertexShaderGraph,
                                                                           pass_info.es2VertexShaderGraph,
                                                                           pass_info.rhiVertexShaderGraph,
                                                                           pass_info.gl3FragmentShaderGraph,
                                                                           pass_info.es3FragmentShaderGraph,
                                                                           pass_info.es2FragmentShaderGraph,
                                                                           pass_info.rhiFragmentShaderGraph,
                                                                           pass_info.gl3VertCode,
                                                                           pass_info.es3VertCode,
                                                                           pass_info.es2VertCode,
                                                                           pass_info.rhiVertCode,
                                                                           pass_info.gl3FragCode,
                                                                           pass_info.es3FragCode,
                                                                           pass_info.es2FragCode,
                                                                           pass_info.rhiFragCode,
                                                                           pass_info.gl3GeometryCode,
                                                                           pass_info.es3GeometryCode,
                                                                           pass_info.es2GeometryCode,
                                                                           pass_info.rhiGeometryCode,
                                                                           pass_info.blendFunction,
                                                                           pass_info.blendSourceRGB,
                                                                           pass_info.blendSourceAlpha,
                                                                           pass_info.blendDestinationRGB,
                                                                           pass_info.blendDestinationAlpha)


            def opaqueOnlyTechnique(passes_info):
                pass_info = passes_info[0]
                return CustomMaterialGenerator.techniqueOpaqueOnly % (matName,
                                                                      matName,
                                                                      pass_info.gl3VertexShaderGraph,
                                                                      pass_info.es3VertexShaderGraph,
                                                                      pass_info.es2VertexShaderGraph,
                                                                      pass_info.rhiVertexShaderGraph,
                                                                      pass_info.gl3FragmentShaderGraph,
                                                                      pass_info.es3FragmentShaderGraph,
                                                                      pass_info.es2FragmentShaderGraph,
                                                                      pass_info.rhiFragmentShaderGraph,
                                                                      pass_info.gl3VertCode,
                                                                      pass_info.es3VertCode,
                                                                      pass_info.es2VertCode,
                                                                      pass_info.rhiVertCode,
                                                                      pass_info.gl3FragCode,
                                                                      pass_info.es3FragCode,
                                                                      pass_info.es2FragCode,
                                                                      pass_info.rhiFragCode,
                                                                      pass_info.gl3GeometryCode,
                                                                      pass_info.es3GeometryCode,
                                                                      pass_info.es2GeometryCode,
                                                                      pass_info.rhiGeometryCode)


            def multiTransparentTechnique(passes_info):
                innerPassContent = ""

                for idx, pass_info in enumerate(passes_info):
                    innerPassContent +=  CustomMaterialGenerator.techniqueMultiTransparentInnerPass % (pass_info.gl3VertexShaderGraph,
                                                                                                       pass_info.es3VertexShaderGraph,
                                                                                                       pass_info.es2VertexShaderGraph,
                                                                                                       pass_info.rhiVertexShaderGraph,
                                                                                                       pass_info.gl3FragmentShaderGraph,
                                                                                                       pass_info.es3FragmentShaderGraph,
                                                                                                       pass_info.es2FragmentShaderGraph,
                                                                                                       pass_info.rhiFragmentShaderGraph,
                                                                                                       pass_info.gl3VertCode,
                                                                                                       pass_info.es3VertCode,
                                                                                                       pass_info.es2VertCode,
                                                                                                       pass_info.rhiVertCode,
                                                                                                       pass_info.gl3FragCode,
                                                                                                       pass_info.es3FragCode,
                                                                                                       pass_info.es2FragCode,
                                                                                                       pass_info.rhiFragCode,
                                                                                                       pass_info.gl3GeometryCode,
                                                                                                       pass_info.es3GeometryCode,
                                                                                                       pass_info.es2GeometryCode,
                                                                                                       pass_info.rhiGeometryCode,
                                                                                                       pass_info.blendFunction,
                                                                                                       pass_info.blendSourceRGB,
                                                                                                       pass_info.blendSourceAlpha,
                                                                                                       pass_info.blendDestinationRGB,
                                                                                                       pass_info.blendDestinationAlpha)
                return CustomMaterialGenerator.techniqueMultiTransparent % (matName,
                                                                            matName,
                                                                            innerPassContent)

            def backgroundTechnique(passes_info):
                pass_info = passes_info[0]
                return CustomMaterialGenerator.techniqueBackgroundOnly % (matName,
                                                                          matName,
                                                                          pass_info.gl3VertexShaderGraph,
                                                                          pass_info.es3VertexShaderGraph,
                                                                          pass_info.es2VertexShaderGraph,
                                                                          pass_info.rhiVertexShaderGraph,
                                                                          pass_info.gl3FragmentShaderGraph,
                                                                          pass_info.es3FragmentShaderGraph,
                                                                          pass_info.es2FragmentShaderGraph,
                                                                          pass_info.rhiFragmentShaderGraph,
                                                                          pass_info.gl3VertCode,
                                                                          pass_info.es3VertCode,
                                                                          pass_info.es2VertCode,
                                                                          pass_info.rhiVertCode,
                                                                          pass_info.gl3FragCode,
                                                                          pass_info.es3FragCode,
                                                                          pass_info.es2FragCode,
                                                                          pass_info.rhiFragCode,
                                                                          pass_info.gl3GeometryCode,
                                                                          pass_info.es3GeometryCode,
                                                                          pass_info.es2GeometryCode,
                                                                          pass_info.rhiGeometryCode)


            def generateTechnique(passes_info):
                switcher = {
                    "Background": backgroundTechnique,
                    "Opaque": opaqueOnlyTechnique,
                    "Transparent": transparentOnlyTechnique,
                    "TransparentAndOpaque": transparentAndOpaqueTechnique,
                    "MultiPassTransparent": multiTransparentTechnique
                }
                pass_type = passes_info[0].pass_type
                if pass_type not in switcher:
                    print("Unhandled pass type")
                    return
                return switcher[pass_type](passes_info), pass_type

            content, technique_name = generateTechnique(passes_info)

            includes = "#include <Qt3DRender/QTechnique>\n"
            includes += "#include <Qt3DRender/QCullFace>\n"
            includes += "#include <Qt3DRender/QFilterKey>\n"
            includes += "#include <Qt3DRender/QRenderPass>\n"
            includes += "#include <Qt3DRender/QShaderProgram>\n"
            includes += "#include <Qt3DRender/QShaderProgramBuilder>\n"
            includes += "#include <Qt3DRender/QGraphicsApiFilter>"

            if technique_name in ["Transparent", "TransparentAndOpaque", "MultiPassTransparent"]:
                includes += "\n"
                includes += "#include <Qt3DRender/QBlendEquation>\n"
                includes += "#include <Qt3DRender/QBlendEquationArguments>"

            if technique_name == "Background":
                includes += "\n"
                includes += "#include <Qt3DRender/QNoDepthMask>\n"
                includes += "#include <Qt3DRender/QDepthTest>"

            self.generateHeaderFile(content,
                                    className,
                                    includes,
                                    True)
        generateHeader()

    def generateEffect(self):
        matName = self.rawJson.get("type", "")
        className = matName + "Effect"

        def generateHeader():
            content = CustomMaterialGenerator.effectClassHeaderContent % (matName,
                                                                          matName,
                                                                          matName,
                                                                          matName,
                                                                          matName,
                                                                          matName,
                                                                          matName,
                                                                          matName)
            self.generateHeaderFile(content,
                                    className,
                                    "#include <Kuesa/gltf2materialeffect.h>\n#include <Kuesa/kuesa_global.h>\n")
        generateHeader()


        def generateCpp():
            doc = self.docForClass(className,
                                   "Qt3DRender::QEffect",
                                   "is the effect for the %sMaterial class." % (matName))

            content = CustomMaterialGenerator.effectClassCppContent.format(doc, matName)

            includes = "#include \"%s.h\"\n" % (className.lower())
            includes += "\n"
            includes += "#include <Qt3DRender/QCullFace>\n"
            includes += "\n"
            includes += "#include \"%stechnique_p.h\"" % (matName.lower())
            includes += "\n"

            self.generateCppFile(content,
                                 className,
                                 includes)
        generateCpp()

    def generateMaterial(self):
        matName = self.rawJson.get("type", "")
        className = matName + "Material"
        props = self.rawJson.get("properties", [])
        texture_props = [p for p in props if p.get("type", "").startswith("texture")]

        def generateTextureParameters(properties):
            content = ""
            param_str = 4 * ' ' + "Qt3DRender::QParameter *m_%sParameter;\n"
            for p in properties:
                content += param_str % p.get("name", "")
            return content

        def generateInitializeParameters(properties):
            content = ""
            param_str = '\n' + 4 * ' ' + ", m_%sParameter(new Qt3DRender::QParameter(QStringLiteral(\"%s\"), {}))"
            for p in properties:
                name =  p.get("name", "")
                content += param_str % (name, name)
            return content

        def generateAddParameters(properties):
            content = ""
            param_str = '\n' + 4 * ' ' + "addParameter(m_%sParameter);"
            for p in properties:
                content += param_str % p.get("name", "")
            return content

        def generateParametersConnection(properties, propertiesClassName):
            content = ""
            connect_str = '\n' + 12 * ' ' + "QObject::connect(m_materialProperties, &%s::%sChanged, this, [this] (%s t) { m_%sParameter->setValue(QVariant::fromValue(t)); });"
            for p in properties:
                name =  p.get("name", "")
                propType = CustomMaterialGenerator.propertyTypesToQtType.get(p.get("type", ""), "")
                content += connect_str % (propertiesClassName, name, propType, name)
            return content

        def generateParameterSetValues(properties):
            content = ""
            set_str = '\n' + 12 * ' ' + "m_%sParameter->setValue(QVariant::fromValue(m_materialProperties->%s()));"
            for p in properties:
                name =  p.get("name", "")
                content += set_str % (name, name)
            return content


        def generateHeader():
            texture_parameters = generateTextureParameters(texture_props)

            content = CustomMaterialGenerator.materialClassHeaderContent % (matName,
                                                                            matName,
                                                                            matName,
                                                                            matName,
                                                                            matName,
                                                                            matName,
                                                                            matName,
                                                                            matName,
                                                                            texture_parameters)
            self.generateHeaderFile(content,
                                    className,
                                    "#include <Kuesa/GLTF2Material>\n#include <Kuesa/kuesa_global.h>\n#include <Kuesa/%sProperties>\n" % matName)
        generateHeader()

        def generateCpp():
            matName = self.rawJson.get("type", "")
            doc = self.docForClass(className, "Kuesa::GLTF2Material", self.rawJson.get("doc", ""))
            parameters_init = generateInitializeParameters(texture_props)
            add_parameters = generateAddParameters(texture_props)
            param_connections = generateParametersConnection(texture_props, matName + "Properties")
            param_setValues = generateParameterSetValues(texture_props)
            content = CustomMaterialGenerator.materialClassCppContent % (doc,
                                                                         matName,
                                                                         matName,
                                                                         parameters_init,
                                                                         add_parameters,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         matName,
                                                                         param_connections,
                                                                         param_setValues,
                                                                         matName,
                                                                         matName)
            includes = "#include \"%smaterial.h\"\n" % (matName.lower())
            includes += "#include \"%sproperties.h\"\n" % (matName.lower())
            includes += "#include \"%sshaderdata_p.h\"\n" % (matName.lower())
            includes += "#include <Qt3DRender/qparameter.h>\n"
            self.generateCppFile(content,
                                 className,
                                 includes)
        generateCpp()

    def generatePri(self):
        matName = self.rawJson.get("type", "").lower()
        content = CustomMaterialGenerator.priContent % (matName,
                                                        matName,
                                                        matName,
                                                        matName,
                                                        matName,
                                                        matName,
                                                        matName,
                                                        matName,
                                                        matName,
                                                        matName)
        with open(matName + ".pri", 'w') as f:
            f.write(content)

    def generate(self):
        # Parse JSON description file
        self.parseDescription()

        # Generate a new directory to contain all classes
        # and a .pri file
        matName = self.rawJson.get("type", "").lower()
        if os.path.isdir(matName):
            shutil.rmtree(matName)
        os.mkdir(matName)
        os.chdir(matName)

        # Generate and write the different classes and files
        self.generateMaterial()
        self.generateTechnique()
        self.generateEffect()
        self.generateMaterialProperties()
        self.generateShaderData()
        self.generatePri()

        # Return to previous path
        os.chdir("..")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage " + sys.argv[0] + " material description file")
        sys.exit()

    for f in sys.argv[1:]:
        generator = CustomMaterialGenerator(f)
        generator.generate()
