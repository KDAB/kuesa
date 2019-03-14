import os
import glob
import sys
import subprocess
import fnmatch

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
            split = os.path.split(pattern)
            path = self.path
            extension = None
            if (len(split) == 1):
                extension = split[0]
            else:
                path = os.path.join(path, split[0])
                extension = split[1]
            path = os.path.abspath(path)
            matches = []
            if recursive:
                print("globbing recursively {}...".format(os.path.join(path, extension)))
                for root, dirnames, filenames in os.walk(path): # Python 2
                    for filename in fnmatch.filter(filenames, extension):
                        matches.append(os.path.join(root, filename))
            else:
                print("globbing {}...".format(os.path.join(path, extension)))
                matches = glob.glob(os.path.join(path, extension))

            count_textures = 0
            count_unknown = 0
            for path in matches:
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
            print("{} textures added, {} unknown files skipped".format(count_textures, count_unknown))

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
        total_assets = len(self.get_dds_paths()) + len(self.get_png_paths()) + len(self.get_jpg_paths())
        count_assets = 0
        for i in self.get_dds_paths():
            self.lint_dds(i)
            count_assets += 1
            self.print_progress(count_assets, total_assets)
        for i in self.get_png_paths():
            self.lint_png(i)
            count_assets += 1
            self.print_progress(count_assets, total_assets)
        for i in self.get_jpg_paths():
            self.lint_jpg(i)
            count_assets += 1
            self.print_progress(count_assets, total_assets)
        print("")

    def print_progress(self, count, total):
        p = int(100 * count / total)
        sys.stdout.write("\r{} / {} assets: {}%".format(count, total, p))
        sys.stdout.flush()

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
