# src.pro
#
# This file is part of Kuesa.
#
# Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
# Author: Kevin Ottens <kevin.ottens@kdab.com>
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

TEMPLATE = subdirs

src_core.subdir = $$PWD/core
src_core.target = sub-core

src_utils.subdir = $$PWD/utils
src_utils.target = sub-utils
src_utils.depends = src_core

SUBDIRS += src_core doc src_utils

qtHaveModule(quick) {
    src_quick_imports.file = $$PWD/quick/imports/core/imports-core.pro
    src_quick_imports.target = sub-quick
    src_quick_imports.depends = src_core

    src_quick_utils_imports.file = $$PWD/quick/imports/utils/imports-utils.pro
    src_quick_utils_imports.target = sub-quick-utils
    src_quick_utils_imports.depends = src_core src_utils

    SUBDIRS += src_quick_imports src_quick_utils_imports
}
