# iro-materials.pri
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Nicolas Guichard <paul.lemire@kdab.com>
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


INCLUDEPATH += $$PWD

include(iro2diffusesem/iro2diffusesem.pri)
include(iro2diffuseequirect/iro2diffuseequirect.pri)

include(iro2alphasem/iro2alphasem.pri)
include(iro2alphaequirect/iro2alphaequirect.pri)

include(iro2glasssem/iro2glasssem.pri)
include(iro2glassequirect/iro2glassequirect.pri)

include(iro2mattealpha/iro2mattealpha.pri)

include(iro2planarreflectionsem/iro2planarreflectionsem.pri)
include(iro2planarreflectionequirect/iro2planarreflectionequirect.pri)

SOURCES += $$PWD/iro2-materials.cpp
HEADERS += $$PWD/iro2-materials_p.h

OTHER_FILES += $$PWD/*.json
