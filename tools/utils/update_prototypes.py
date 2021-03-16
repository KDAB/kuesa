#!/usr/bin/env python3
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Nicolas Guichard <nicolas.guichard@kdab.com>
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
