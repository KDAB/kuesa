import os
import glob
import sys
import subprocess

class Module (object):

    path = None
    argv = None

    texture_asset_paths = None

    errors = None

    def __init__(self, path, argv):
        self.path = path
        self.argv = argv
        self.texture_asset_paths = []
        self.errors = {}

    def gather_asset_files(self):
        recursive = ("-r" in self.argv)
        patterns = self.argv[1].split(",")
        unknown_file_types = []
        for pattern in patterns:
            globpath = None
            if recursive:
                globpath = os.path.join(self.path, "**", pattern)
            else:
                globpath = os.path.join(self.path, pattern)
            print("globbing '{}', recursive = {}...".format(globpath, recursive))
            filelist = glob.glob(globpath, recursive=recursive)
            count_textures = 0
            count_unknown = 0
            for path in filelist:
                absolute_path = os.path.abspath(path)
                if not os.path.isfile(absolute_path):
                    continue
                if absolute_path.endswith(".dds") or absolute_path.endswith(".png") or absolute_path.endswith(".jpg"):
                    if absolute_path not in self.texture_asset_paths:
                        self.texture_asset_paths.append(absolute_path)
                        count_textures += 1
                else:
                    unknown_file_type = absolute_path[-4:]
                    if unknown_file_type not in unknown_file_types:
                        unknown_file_types.append(unknown_file_type)
                        print("Unknown file type '{}'".format(absolute_path[-4:]))
                    count_unknown += 1
            print("{} textures added, {} unknown files".format(count_textures, count_unknown))

    def get_texture_asset_paths(self):
        return self.texture_asset_paths

    def get_texture_asset_paths_by_extension(self, extension):
        r = []
        for path in self.get_texture_asset_paths():
            if path.endswith(extension):
                r.append(path)
        return r

    def get_dds_paths(self):
        return self.get_texture_asset_paths_by_extension(".dds")

    def get_png_paths(self):
        return self.get_texture_asset_paths_by_extension(".png")

    def get_jpg_paths(self):
        return self.get_texture_asset_paths_by_extension(".jpg")

    def lint(self):
        print("Linting...")
        for i in self.get_dds_paths():
            self.lint_dds(i)
        for i in self.get_png_paths():
            self.lint_png(i)
        for i in self.get_jpg_paths():
            self.lint_jpg(i)

    def lint_dds(self, path):
        print("lint_dds() not implemented for linting module '{}'!".format(self.__class__.__name__))
        sys.exit(1)

    def lint_png(self, path):
        print("lint_png() not implemented for linting module '{}'!".format(self.__class__.__name__))
        sys.exit(1)

    def lint_jpg(self, path):
        print("lint_jpg() not implemented for linting module '{}'!".format(self.__class__.__name__))
        sys.exit(1)

    def magick_identify_verbose(self, path):
        result = subprocess.run(["identify", "-verbose", path], capture_output=True)
        if result.returncode != 0:
            self.add_error("ImageMagick failed on '{}'".format(path))
            return ""
        return result.stdout

    def add_warning(self, path, warning):
        if not path in self.errors.keys():
            self.errors[path] = []
        self.errors[path].append("WARNING: {}".format(warning))

    def add_error(self, path, error):
        if not path in self.errors.keys():
            self.errors[path] = []
        self.errors[path].append("ERROR: {}".format(error))

    def print_errors(self):
        for path in self.errors.keys():
            print(path)
            for message in self.errors[path]:
                print("  {}".format(message))
