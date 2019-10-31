defineReplace(fixedPath) {
	windows: return ($$str_member($$1, 2, -1))
	return ($$1)
}

windows: CMD_SEP="&"
   else: CMD_SEP=";"

PACKAGE_PATH = $$OUT_PWD/install/$$fixedPath($$[QT_INSTALL_PREFIX])
PACKAGE_PREFIX = $$basename(PACKAGE_PATH)
PACKAGE_PATH = $$clean_path($$PACKAGE_PATH/..)

PACKAGE_LIB_FILE=$$shell_path($$OUT_PWD/$${PACKAGE_NAME}-lib-$${QT_VERSION}-$${PACKAGE_PREFIX}.7z)
lib_package.target = lib_package
lib_package.depends = sub-src
lib_package.commands = $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install) $$CMD_SEP \
                       $$QMAKE_DEL_FILE $$shell_path($${PACKAGE_LIB_FILE}) $$CMD_SEP \
                       $(MAKE) -f $(MAKEFILE) INSTALL_ROOT=$$shell_path($$fixedPath($$OUT_PWD/install)) install && \
                       7z a -bb3 -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on $${PACKAGE_LIB_FILE} $$shell_path($$PACKAGE_PATH) && \
                       $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install) $$CMD_SEP \
                       echo "Generated package file: $${PACKAGE_LIB_FILE}" $$CMD_SEP \
                       echo "Generated package file: $${PACKAGE_QTC_TEMPLATES_FILE}"

DOCS_PATH = $$OUT_PWD/install_docs/$$fixedPath($$[QT_INSTALL_DOCS])
DOCS_PATH = $$clean_path($$DOCS_PATH/..)
PACKAGE_DOC_FILE=$$shell_path($$OUT_PWD/$${PACKAGE_NAME}-doc-$${QT_VERSION}-all.7z)
doc_package.target = doc_package
doc_package.depends = docs
doc_package.commands = $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install_docs) $$CMD_SEP \
                       $$QMAKE_DEL_FILE $$shell_path($${PACKAGE_DOC_FILE}) $$CMD_SEP \
                       $(MAKE) -f $(MAKEFILE) INSTALL_ROOT=$$shell_path($$fixedPath($$OUT_PWD/install_docs)) install_docs && \
                       7z a -bb3 -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on $${PACKAGE_DOC_FILE} $$shell_path($$DOCS_PATH) && \
                       $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install_docs) $$CMD_SEP \
                       echo "Generated package file: $${PACKAGE_DOC_FILE}"

EXAMPLES_PATH = $$OUT_PWD/install_examples/$$fixedPath($$[QT_INSTALL_EXAMPLES])
EXAMPLES_PATH = $$clean_path($$EXAMPLES_PATH/..)
PACKAGE_EXAMPLES_FILE=$$shell_path($$OUT_PWD/$${PACKAGE_NAME}-examples-$${QT_VERSION}-all.7z)
examples_package.target = examples_package
examples_package.depends = sub-examples
examples_package.commands = $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install_examples) $$CMD_SEP \
                       $$QMAKE_DEL_FILE $$shell_path($${PACKAGE_EXAMPLES_FILE}) $$CMD_SEP \
                       $(MAKE) -f $(MAKEFILE) INSTALL_ROOT=$$shell_path($$fixedPath($$OUT_PWD/install_examples)) sub-examples-install_subtargets && \
                       7z a -bb3 -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on $$shell_path($${PACKAGE_EXAMPLES_FILE}) $$shell_path($$EXAMPLES_PATH) && \
                       $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install_examples) $$CMD_SEP \
                       echo "Generated package file: $${PACKAGE_EXAMPLES_FILE}"

PACKAGE_TEMPLATES_FILE=$$shell_path($$OUT_PWD/$${PACKAGE_NAME}-qtc_templates-1.0.0-all.7z)
templates_package.target = templates_package
templates_package.depends = sub-examples
templates_package.commands = $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install_templates) $$CMD_SEP \
                       $$QMAKE_DEL_FILE $$shell_path($${PACKAGE_TEMPLATES_FILE}) $$CMD_SEP \
                       $(MAKE) -f $(MAKEFILE) INSTALL_ROOT=$$shell_path($$fixedPath($$OUT_PWD/install_templates)) install && \
                       7z a -bb3 -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on $${PACKAGE_TEMPLATES_FILE} $$shell_path($$OUT_PWD/install_templates/Tools) && \
                       $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install_templates) $$CMD_SEP \
                       echo "Generated package file: $${PACKAGE_TEMPLATES_FILE}"

packages.target = packages
packages.depends = lib_package doc_package examples_package templates_package

QMAKE_EXTRA_TARGETS *= lib_package doc_package examples_package templates_package packages
