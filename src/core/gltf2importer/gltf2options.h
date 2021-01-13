/*
    gltf2options.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#ifndef KUESA_GLTF2IMPORT_GLTF2OPTIONS_H
#define KUESA_GLTF2IMPORT_GLTF2OPTIONS_H

#include <QObject>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {
namespace GLTF2Import {

class KUESASHARED_EXPORT GLTF2Options : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool generateTangents READ generateTangents WRITE setGenerateTangents NOTIFY generateTangentsChanged)
    Q_PROPERTY(bool generateNormals READ generateNormals WRITE setGenerateNormals NOTIFY generateNormalsChanged)
public:
    GLTF2Options();

    bool generateTangents() const;
    bool generateNormals() const;

public Q_SLOTS:
    void setGenerateTangents(bool generateTangents);
    void setGenerateNormals(bool generateNormals);

Q_SIGNALS:
    void generateTangentsChanged(bool generateTangents);
    void generateNormalsChanged(bool generateNormals);

private:
    bool m_generateTangents;
    bool m_generateNormals;
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_GLTF2OPTIONS_H
