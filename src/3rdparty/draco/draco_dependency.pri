QT_FOR_CONFIG += kuesa-private
qtConfig(system-draco):!if(cross_compile:host_build) {
    QMAKE_USE_PRIVATE += draco
} else {
    qtConfig(draco) {
        include(draco.pri)
    }
}

