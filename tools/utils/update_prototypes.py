#!/usr/bin/env python3

import json
import copy
import argparse

def to_dotjson_def(prototype):
    proto

def update_prototype(prototypes, name, qt3d_definition, editor_definition):
    if type(prototypes) is list:
        found_at = -1
        for idx, prototype in enumerate(prototypes):
            if prototype["name"] == name:
                found_at = idx
                break
        if found_at >= 0:
            prototypes[found_at] = editor_definition
    elif type(prototypes) is dict and name in prototypes:
        prototypes[name] = qt3d_definition

    return prototypes

def update_file(filename, name, qt3d_definition, editor_definition):
    try:
        with open(filename, 'r') as f:
            content = json.load(f)
    except json.decoder.JSONDecodeError as e:
        print(f"ERROR: Error deserializing {filename}: {e}")
        return

    before = copy.deepcopy(content)

    if type(content) is list:
        content = update_prototype(content, name, qt3d_definition, editor_definition)
    elif type(content) is dict and "prototypes" in content:
        content["prototypes"] = update_prototype(content["prototypes"], name, qt3d_definition, editor_definition)
    else:
        print(f"WARNING: Ignored file {filename}")
        return

    if before == content:
        print(f"INFO: {filename} unchanged")
        return

    with open(filename, 'w') as f:
        print(f"INFO: {filename} updated")
        json.dump(content, f, indent=4)

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("name")
    parser.add_argument("qt3d_definition")
    parser.add_argument("editor_definition")
    parser.add_argument('files', nargs='*')

    args = parser.parse_args()

    name = args.name
    qt3d_definition = json.loads(args.qt3d_definition)
    editor_definition = json.loads(args.editor_definition)
    files_to_update = args.files

    for file_to_update in files_to_update:
        update_file(file_to_update, name, qt3d_definition, editor_definition)

if __name__ == '__main__':
    main()
