/*
    kuesa_global.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

    Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
    accordance with the Kuesa Enterprise License Agreement provided with the Software in the
    LICENSE.KUESA.ENTERPRISE file.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef KUESA_KUESA_GLOBAL_H
#define KUESA_KUESA_GLOBAL_H

#include <QtCore/qglobal.h>

#define KUESA_VERSION_MAJOR 1
#define KUESA_VERSION_MINOR 3
#define KUESA_VERSION_PATCH 0

//    KUESA_VERSION is (major << 16) + (minor << 8) + patch.
#define KUESA_VERSION      QT_VERSION_CHECK(KUESA_VERSION_MAJOR, KUESA_VERSION_MINOR, KUESA_VERSION_PATCH)
//    can be used like #if (KUESA_VERSION >= KUESA_VERSION_CHECK(1, 3, 0))
#define KUESA_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#define KUESA_TO_STR2(x) #x
#define KUESA_TO_STR(x) KUESA_TO_STR2(x)
#define KUESA_VERSION_STRING (KUESA_TO_STR(KUESA_VERSION_MAJOR) "." KUESA_TO_STR(KUESA_VERSION_MINOR) "." KUESA_TO_STR(KUESA_VERSION_PATCH))

QT_BEGIN_NAMESPACE

#if defined(QT_SHARED) || !defined(QT_STATIC)
#if defined(QT_BUILD_KUESA_LIB)
#define KUESASHARED_EXPORT Q_DECL_EXPORT
#else
#define KUESASHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#define KUESASHARED_EXPORT
#endif

QT_END_NAMESPACE

#endif // KUESA_KUESA_GLOBAL_H
