PACKAGE_PATH = $$OUT_PWD/install/$$[QT_INSTALL_PREFIX]
PACKAGE_PREFIX = $$basename(PACKAGE_PATH)
PACKAGE_PATH = $$clean_path($$PACKAGE_PATH/..)

PACKAGE_LIB_FILE=$$OUT_PWD/$$shell_path($${PACKAGE_NAME}-lib-$${QT_VERSION}-$${PACKAGE_PREFIX}.7z)
lib_package.target = lib_package
lib_package.depends = sub-src
lib_package.commands = $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install); \
                       $$QMAKE_DEL_FILE $${PACKAGE_LIB_FILE}; \
                       $$QMAKE_CD $$OUT_PWD && \
                       $(MAKE) -f $(MAKEFILE) INSTALL_ROOT=$$OUT_PWD/install install && \
                       7z a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on $${PACKAGE_LIB_FILE} $$PACKAGE_PATH && \
                       $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install); \
                       echo "Generated package file: $${PACKAGE_LIB_FILE}"

DOCS_PATH = $$OUT_PWD/install_docs/$$[QT_INSTALL_DOCS]
DOCS_PATH = $$clean_path($$DOCS_PATH/..)
PACKAGE_DOC_FILE=$$shell_path($$OUT_PWD/$${PACKAGE_NAME}-doc-$${QT_VERSION}-all.7z)
doc_package.target = doc_package
doc_package.depends = docs
doc_package.commands = $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install_docs); \
                       $$QMAKE_DEL_FILE $${PACKAGE_DOC_FILE}; \
                       $(MAKE) -f $(MAKEFILE) INSTALL_ROOT=$$OUT_PWD/install_docs install_docs && \
                       7z a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on $${PACKAGE_DOC_FILE} $$DOCS_PATH && \
                       $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install_docs); \
                       echo "Generated package file: $${PACKAGE_DOC_FILE}"

EXAMPLES_PATH = $$OUT_PWD/install_examples/$$[QT_INSTALL_EXAMPLES]
EXAMPLES_PATH = $$clean_path($$EXAMPLES_PATH/..)
PACKAGE_EXAMPLES_FILE=$$shell_path($$OUT_PWD/$${PACKAGE_NAME}-examples-$${QT_VERSION}-all.7z)
examples_package.target = examples_package
examples_package.depends = sub-examples
examples_package.commands = $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install_examples); \
                       $$QMAKE_DEL_FILE $${PACKAGE_EXAMPLES_FILE}; \
                       $(MAKE) -f $(MAKEFILE) INSTALL_ROOT=$$OUT_PWD/install_examples sub-examples-install_subtargets && \
                       7z a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on $${PACKAGE_EXAMPLES_FILE} $$EXAMPLES_PATH && \
                       $$QMAKE_DEL_TREE $$shell_path($$OUT_PWD/install_examples); \
                       echo "Generated package file: $${PACKAGE_EXAMPLES_FILE}"

packages.target = packages
packages.depends = lib_package doc_package examples_package

QMAKE_EXTRA_TARGETS *= lib_package doc_package examples_package packages
