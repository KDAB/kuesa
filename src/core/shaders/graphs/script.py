#!/usr/bin/env python3

import copy
import json
import sys

INPUT = {
    'outputs': ['value'],
    'parameters': {
        'name': 'defaultName',
        'type': {
            'type': 'QShaderLanguage::VariableType',
            'value': 'QShaderLanguage::Vec3'
        },
        'location': '0'
    },
    'rules': [
        {
            'format': { 'api': 'OpenGLES', 'major': 2, 'minor': 0 },
            'headerSnippets': ['attribute highp $type $name;'],
            'substitution': 'highp $type $value = $name;'
        },
        {
            'format': { 'api': 'OpenGLES', 'major': 3, 'minor': 0 },
            'headerSnippets': ['in highp $type $name;'],
            'substitution': 'highp $type $value = $name;'
        },
        {
            'format': { 'api': 'OpenGLCoreProfile', 'major': 3, 'minor': 0 },
            'headerSnippets': ['in $type $name;'],
            'substitution': '$type $value = $name;'
        },
        {
            'format': { 'api': 'RHI', 'major': 1, 'minor': 0 },
            'headerSnippets': ['layout(location = $location) in $type $name;'],
            'substitution': '$type $value = $name;'
        }
    ]
}

BUILTIN_INPUT = {
    'outputs': ['value'],
    'parameters': {
        'name': 'defaultName',
        'type': {
            'type': 'QShaderLanguage::VariableType',
            'value': 'QShaderLanguage::Vec3'
        }
    },
    'rules': [
        {
            'format': { 'api': 'OpenGLES', 'major': 2, 'minor': 0 },
            'substitution': 'highp $type $value = $name;'
        },
        {
            'format': { 'api': 'OpenGLCoreProfile', 'major': 3, 'minor': 0 },
            'substitution': '$type $value = $name;'
        },
        {
            'format': { 'api': 'RHI', 'major': 1, 'minor': 0 },
            'substitution': '$type $value = $name;'
        }
    ]
}

UNIFORM_INPUT = {
    'outputs': ['value'],
    'parameters': {
        'name': 'defaultName',
        'type': {
            'type': 'QShaderLanguage::VariableType',
            'value': 'QShaderLanguage::Vec3'
        },
        'location': '0'
    },
    'rules': [
        {
            'format': { 'api': 'OpenGLES', 'major': 2, 'minor': 0 },
            'headerSnippets': ['uniform highp $type $name;'],
            'substitution': 'highp $type $value = $name;'
        },
        {
            'format': { 'api': 'OpenGLCoreProfile', 'major': 3, 'minor': 0 },
            'headerSnippets': ['uniform $type $name;'],
            'substitution': '$type $value = $name;'
        },
        {
            'format': { 'api': 'RHI', 'major': 1, 'minor': 0 },
            'headerSnippets': ['add-uniform $type $name'],
            'substitution': '$type $value = $name;'
        }
    ]
}

UNIFORM_ARRAY_INPUT = {
    'outputs': ['value'],
    'parameters': {
        'name': 'defaultName',
        'size': '1',
        'type': {
            'type': 'QShaderLanguage::VariableType',
            'value': 'QShaderLanguage::Vec3'
        },
        'location': '0'
    },
    'rules': [
        {
            'format': { 'api': 'OpenGLES', 'major': 2, 'minor': 0 },
            'headerSnippets': ['uniform highp $type $name[$size];'],
            'substitution': 'highp $type $value = $name;'
        },
        {
            'format': { 'api': 'OpenGLCoreProfile', 'major': 3, 'minor': 0 },
            'headerSnippets': ['uniform $type $name[$size];'],
            'substitution': '$type $value = $name;'
        },
        {
            'format': { 'api': 'RHI', 'major': 1, 'minor': 0 },
            'headerSnippets': ['add-uniform $type $name[$size]'],
            'substitution': '$type $value = $name;'
        }
    ]
}

TEXTURE_INPUT = {
    'outputs': ['value'],
    'parameters': {
        'name': 'defaultName',
        'type': {
            'type': 'QShaderLanguage::VariableType',
            'value': 'QShaderLanguage::Vec3'
        }
    },
    'rules': [
        {
            'format': { 'api': 'OpenGLES', 'major': 2, 'minor': 0 },
            'headerSnippets': ['uniform highp $type $name;'],
            'substitution': 'highp $type $value = $name;'
        },
        {
            'format': { 'api': 'OpenGLCoreProfile', 'major': 3, 'minor': 0 },
            'headerSnippets': ['uniform $type $name;'],
            'substitution': '$type $value = $name;'
        },
        {
            'format': { 'api': 'RHI', 'major': 1, 'minor': 0 },
            'headerSnippets': ['add-sampler $type $name'],
            'substitution': '$type $value = $name;'
        }
    ]
}

SAMPLE_TEXTURE = {
    'inputs': ['coord'],
    'outputs': ['color'],
    'parameters': {
        'name': 'defaultName'
    },
    'rules': [
        {
            'format': { 'api': 'OpenGLES', 'major': 2, 'minor': 0 },
            'headerSnippets': ['uniform sampler2D $name;'],
            'substitution': 'highp vec4 $color = texture2D($name, $coord);'
        },
        {
            'format': { 'api': 'OpenGLES', 'major': 3, 'minor': 0 },
            'headerSnippets': ['uniform sampler2D $name;'],
            'substitution': 'highp vec4 $color = texture($name, $coord);'
        },
        {
            'format': { 'api': 'OpenGLCoreProfile', 'major': 3, 'minor': 0 },
            'headerSnippets': ['uniform sampler2D $name;'],
            'substitution': 'vec4 $color = texture($name, $coord);'
        },
        {
            'format': { 'api': 'RHI', 'major': 1, 'minor': 0 },
            'headerSnippets': ['add-sampler sampler2D $name'],
            'substitution': 'vec4 $color = texture($name, $coord);'
        }
    ]
}

OUTPUT = {
    'inputs': ['value'],
    'parameters': {
        'name': 'defaultName',
        'type': {
            'type': 'QShaderLanguage::VariableType',
            'value': 'QShaderLanguage::Vec3'
        }
    },
    'rules': [
        {
            'format': { 'api': 'OpenGLES', 'major': 2, 'minor': 0 },
            'headerSnippets': ['varying highp $type $name;'],
            'substitution': '$name = $value;'
        },
        {
            'format': { 'api': 'OpenGLES', 'major': 3, 'minor': 0 },
            'headerSnippets': ['out highp $type $name;'],
            'substitution': '$name = $value;'
        },
        {
            'format': { 'api': 'OpenGLCoreProfile', 'major': 3, 'minor': 0 },
            'headerSnippets': ['out $type $name;'],
            'substitution': '$name = $value;'
        },
        {
            'format': { 'api': 'RHI', 'major': 1, 'minor': 0 },
            'headerSnippets': ['add-input out $type $name'],
            'substitution': '$name = $value;'
        }
    ]
}


def main():
    filename = sys.argv[1]
    with open(filename) as f:
        graph = json.load(f)

    for prototype in graph['prototypes'].values():
         # Check if the RHI rule is already there
        rhi_rule = next((rule for rule in prototype['rules'] if rule['format']['api'] == 'RHI'), None)
        if rhi_rule is not None:
            continue

        # Otherwise add it
        gl3_rule = next((rule for rule in prototype['rules'] if rule['format']['api'] == 'OpenGLCoreProfile'))
        rhi_rule = copy.deepcopy(gl3_rule)
        rhi_rule['format'] = {
            'api': 'RHI',
            'major': 1,
            'minor': 0,
        }
        prototype['rules'].append(rhi_rule)

    graph['prototypes']['input'] = INPUT
    graph['prototypes']['builtinInput'] = BUILTIN_INPUT
    graph['prototypes']['uniformInput'] = UNIFORM_INPUT
    graph['prototypes']['uniformArrayInput'] = UNIFORM_ARRAY_INPUT
    graph['prototypes']['textureInput'] = TEXTURE_INPUT
    graph['prototypes']['sampleTexture'] = SAMPLE_TEXTURE
    graph['prototypes']['output'] = OUTPUT

    currentInput = 0
    currentOutput = 0

    for node in graph['nodes']:
        if node['type'] == 'input':
            if node['parameters']['qualifier']['value'] == 'QShaderLanguage::Uniform':
                if node['parameters']['type']['value'] == 'QShaderLanguage::Sampler2D':
                    node['type'] = 'textureInput'
                else:
                    node['type'] = 'uniformInput'
            elif node['parameters']['qualifier']['value'] == 'QShaderLanguage::BuiltIn':
                node['type'] = 'builtinInput'
            else:
                node['parameters']['location'] = str(currentInput)
                currentInput += 1

            del node['parameters']['qualifier']

        elif node['type'] == 'arrayInput':
            node['type'] = 'uniformArrayInput'
            del node['parameters']['qualifier']

        elif node['type'] == 'output':
            node['parameters']['location'] = str(currentOutput)
            currentOutput += 1


    with open(filename, 'w') as f:
        json.dump(graph, f, indent=4)


if __name__ == '__main__':
    main()
