defineTest(qtConfLibrary_draco) {
    libs = $$eval($${1}.libs)
    includedir =

    libs_override = $$getenv(DRACOSDK_LIBS)
    !isEmpty(libs_override) {
        libs = $${libs_override}
    }

    prefix = $$getenv(DRACOSDK)

    unix:isEmpty(prefix) {
        libs += "-L/usr/local/lib"
        libs += "-L/usr/lib"
    }

    !isEmpty(prefix) {
        includedir += $${prefix}/include
        !win32:libs += -L$${prefix}/lib
    }

    $${1}.libs = $$val_escape(libs)
    $${1}.includedir = $$val_escape(includedir)

    export($${1}.libs)
    export($${1}.includedir)

    return(true)
}

