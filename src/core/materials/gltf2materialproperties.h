/*
    gltf2materialproperties.h

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_GLTF2MATERIALPROPERTIES_H
#define KUESA_GLTF2MATERIALPROPERTIES_H

#include <Kuesa/kuesa_global.h>
#include <Qt3DCore/QNode>
#include <QVector>
#include <QColor>
#include <QtGui/QMatrix3x3>
#include <Qt3DRender/QAbstractTexture>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QMaterial;
class QShaderData;
} // namespace Qt3DRender

namespace Kuesa {

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
class DummyObserver;
#endif

class KUESASHARED_EXPORT GLTF2MaterialProperties : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QMatrix3x3 textureTransform READ textureTransform WRITE setTextureTransform NOTIFY textureTransformChanged)
    Q_PROPERTY(bool baseColorUsesTexCoord1 READ isBaseColorUsingTexCoord1 WRITE setBaseColorUsesTexCoord1 NOTIFY baseColorUsesTexCoord1Changed)
    Q_PROPERTY(QColor baseColorFactor READ baseColorFactor WRITE setBaseColorFactor NOTIFY baseColorFactorChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *baseColorMap READ baseColorMap WRITE setBaseColorMap NOTIFY baseColorMapChanged)
    Q_PROPERTY(float alphaCutoff READ alphaCutoff WRITE setAlphaCutoff NOTIFY alphaCutoffChanged)

public:
    explicit GLTF2MaterialProperties(Qt3DCore::QNode *parent = nullptr);
    ~GLTF2MaterialProperties();

    bool isBaseColorUsingTexCoord1() const;
    QColor baseColorFactor() const;
    Qt3DRender::QAbstractTexture *baseColorMap() const;
    float alphaCutoff() const;
    QMatrix3x3 textureTransform() const;

    void addClientMaterial(Qt3DRender::QMaterial *material);
    QVector<Qt3DRender::QMaterial *> clientMaterials() const;

    virtual Qt3DRender::QShaderData *shaderData() const = 0;

public Q_SLOTS:
    void setBaseColorUsesTexCoord1(bool baseColorUsesTexCoord1);
    void setBaseColorFactor(const QColor &baseColorFactor);
    void setBaseColorMap(Qt3DRender::QAbstractTexture *baseColorMap);
    void setAlphaCutoff(float alphaCutoff);
    void setTextureTransform(const QMatrix3x3 &textureTransform);

Q_SIGNALS:
    void baseColorUsesTexCoord1Changed(bool);
    void baseColorFactorChanged(const QColor &baseColorFactor);
    void baseColorMapChanged(Qt3DRender::QAbstractTexture *baseColorMap);
    void alphaCutoffChanged(float alphaCutoff);
    void textureTransformChanged(const QMatrix3x3 &textureTransform);

private:
    QVector<Qt3DRender::QMaterial *> m_clientMaterials;
    QVector<QMetaObject::Connection> m_connections;

    float m_alphaCutOff;
    bool m_usesTexCoord1;
    Qt3DRender::QAbstractTexture *m_baseColorTexture;
    QColor m_baseColorFactor;
    QMatrix3x3 m_textureTransform;

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    mutable DummyObserver *m_dummyObserver;
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
#endif
};
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2MATERIALPROPERTIES_H
