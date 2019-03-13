from .module_es2 import ModuleES2
from .module_es3 import ModuleES3

class GlesLint (object):

    lint_module = None

    def __init__(self, path, argv):
        lint_module = ModuleES2
        if "-es3" in argv:
            lint_module = ModuleES3
        self.lint_module = lint_module(path, argv)
        self.lint_module.gather_asset_files()
        self.lint_module.lint()
        self.lint_module.print_errors()
