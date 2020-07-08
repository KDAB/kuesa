# entitytransformwatcher.pro
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Paul Lemire <paul.lemire@kdab.com>
#
# Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
# accordance with the Kuesa Enterprise License Agreement provided with the Software in the
# LICENSE.KUESA.ENTERPRISE file.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.

TEMPLATE = app

TARGET = test

QT += testlib kuesa kuesa-private 3dcore 3drender

CONFIG += testcase

SOURCES += \
    tst_entitytransformwatcher.cpp
