/*
    assetinspector.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef ASSETINSPECTOR_H
#define ASSETINSPECTOR_H

#include <QObject>

namespace Kuesa {
class AbstractAssetCollection;
}

class TextureInspector;
class MeshInspector;
class MaterialInspector;

class AssetInspector : public QObject
{
    Q_OBJECT
public:
    enum AssetType {
        Material,
        Mesh,
        Texture,
        Unknown
    };
    Q_ENUM(AssetType)

    AssetInspector(QObject *parent = nullptr);
    virtual ~AssetInspector() {}

    void setAsset(const QString &assetName, Kuesa::AbstractAssetCollection *collection);
    void clear();

    AssetType assetType() const;
    QString assetName() const;

    MaterialInspector *materialInspector() const;
    MeshInspector *meshInspector() const;
    TextureInspector *textureInspector() const;

Q_SIGNALS:
    void assetTypeChanged(AssetInspector::AssetType assetType);
    void assetChanged(const QString &assetName);

private:
    QString m_currentAssetName;
    AssetType m_assetType;
    MaterialInspector *m_materialInspector;
    MeshInspector *m_meshInspector;
    TextureInspector *m_textureInspector;
};

#endif // ASSETINSPECTOR_H
