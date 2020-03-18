/*
    metallicroughnessmaterial.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

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

#ifndef KUESA_METALLICROUGHNESSMATERIAL_H
#define KUESA_METALLICROUGHNESSMATERIAL_H

#include <QtGui/qcolor.h>
#include <Kuesa/gltf2material.h>
#include <Kuesa/kuesa_global.h>
#include <Kuesa/metallicroughnesseffect.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QFilterKey;
class QAbstractTexture;
class QTechnique;
class QParameter;
class QShaderProgram;
class QShaderProgramBuilder;
class QRenderPass;
class QCullFace;
} // namespace Qt3DRender

namespace Kuesa {

class MetallicRoughnessProperties;

class KUESASHARED_EXPORT MetallicRoughnessMaterial : public GLTF2Material
{
    Q_OBJECT

    Q_PROPERTY(Kuesa::MetallicRoughnessProperties *metallicRoughnessProperties READ metallicRoughnessProperties WRITE setMetallicRoughnessProperties NOTIFY metallicRoughnessPropertiesChanged)

public:
    explicit MetallicRoughnessMaterial(Qt3DCore::QNode *parent = nullptr);
    ~MetallicRoughnessMaterial();

    MetallicRoughnessProperties *metallicRoughnessProperties() const;

public Q_SLOTS:
    void setMetallicRoughnessProperties(Kuesa::MetallicRoughnessProperties *metallicRoughnessProperties);

Q_SIGNALS:
    void metallicRoughnessPropertiesChanged(Kuesa::MetallicRoughnessProperties *metallicRoughnessProperties);

private:
    MetallicRoughnessProperties *m_metallicRoughnessProperties;
    Qt3DRender::QParameter *m_metallicRoughnessShaderDataParameter;
    QMetaObject::Connection m_textureTransformChangedConnection;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_METALLICROUGHNESSMATERIAL_H
