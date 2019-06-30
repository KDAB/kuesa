/*
    materialinspector.h

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#ifndef MATERIALINSPECTOR_H
#define MATERIALINSPECTOR_H

#include <QObject>
#include <QMatrix3x3>

namespace Qt3DRender {
class QAbstractTexture;
}

namespace Kuesa {
class AbstractAssetCollection;
class MetallicRoughnessProperties;
class MetallicRoughnessMaterial;
} // namespace Kuesa

class MaterialInspector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Kuesa::MetallicRoughnessProperties *materialProperties READ materialProperties NOTIFY materialPropertiesChanged)
    Q_PROPERTY(bool hasTextures READ hasTextures NOTIFY materialPropertiesChanged)
    Q_PROPERTY(Kuesa::MetallicRoughnessMaterial *material READ material NOTIFY materialPropertiesChanged)

public:
    explicit MaterialInspector(QObject *parent = nullptr);
    virtual ~MaterialInspector();

    void setMaterialProperties(Kuesa::MetallicRoughnessProperties *materialProperties);
    Kuesa::MetallicRoughnessProperties *materialProperties() const;
    Kuesa::MetallicRoughnessMaterial *material() const;

    QString assetName() const;

    QColor baseColor() const;
    Qt3DRender::QAbstractTexture *baseColorMap() const;
    float metallicFactor() const;
    float roughnessFactor() const;
    Qt3DRender::QAbstractTexture *metalRoughMap() const;

    Qt3DRender::QAbstractTexture *normalMap() const;
    float normalScale() const;

    Qt3DRender::QAbstractTexture *ambientOcclusionMap() const;

    QColor emissiveFactor() const;
    Qt3DRender::QAbstractTexture *emissiveMap() const;

    QMatrix3x3 textureTransform() const;
    bool doubleSided() const;
    bool opaque() const;

    bool hasTextures() const;

Q_SIGNALS:
    void materialPropertiesChanged();

private:
    Kuesa::MetallicRoughnessProperties *m_materialProperties;
    Kuesa::MetallicRoughnessMaterial *m_material;
    QMetaObject::Connection m_materialConnection;
};

#endif // MATERIALINSPECTOR_H
