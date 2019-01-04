/*
    gltf2importer.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_GLTF2IMPORTER_H
#define KUESA_GLTF2IMPORTER_H

#include <QUrl>
#include <Qt3DCore/QNode>
#include <Kuesa/kuesa_global.h>

QT_BEGIN_NAMESPACE

namespace Kuesa {
class SceneEntity;
class GLTF2Context;

class KUESASHARED_EXPORT GLTF2Importer : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Kuesa::GLTF2Importer::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(Kuesa::SceneEntity *sceneEntity READ sceneEntity WRITE setSceneEntity NOTIFY sceneEntityChanged)
    Q_PROPERTY(bool assignNames READ assignNames WRITE setAssignNames NOTIFY assignNamesChanged)
public:
    enum Status {
        None,
        Loading,
        Ready,
        Error
    };
    Q_ENUM(Status)

    GLTF2Importer(Qt3DCore::QNode *parent = nullptr);
    ~GLTF2Importer();

    QUrl source() const;
    GLTF2Importer::Status status() const;
    Kuesa::SceneEntity *sceneEntity() const;
    bool assignNames() const;

public Q_SLOTS:
    void setSource(const QUrl &source);
    void setSceneEntity(Kuesa::SceneEntity *sceneEntity);
    void setAssignNames(bool assignNames);

Q_SIGNALS:
    void sourceChanged(const QUrl &source);
    void statusChanged(const Kuesa::GLTF2Importer::Status status);
    void sceneEntityChanged(Kuesa::SceneEntity *sceneEntity);
    void assignNamesChanged(bool assignNames);

private Q_SLOTS:
    void load();

private:
    void clear();
    void setStatus(Status status);

    Kuesa::GLTF2Context *m_context;
    QUrl m_source;
    Qt3DCore::QEntity *m_root;
    Status m_status;
    Kuesa::SceneEntity *m_sceneEntity;
    QMetaObject::Connection m_sceneEntityDestructionConnection;
    bool m_assignNames;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORTER_H
