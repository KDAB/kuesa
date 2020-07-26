/*
    entitytransformwatcher.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

    Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
    accordance with the Kuesa Enterprise License Agreement provided with the Software in the
    LICENSE.KUESA.ENTERPRISE file.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef ENTITYTRANSFORMWATCHER_P_H
#define ENTITYTRANSFORMWATCHER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Kuesa API.  It exists for the convenience
// of other Kuesa classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QMatrix4x4>
#include <QObject>
#include <Kuesa/private/kuesa_global_p.h>

namespace Qt3DCore {
class QEntity;
}

QT_BEGIN_NAMESPACE

namespace Kuesa {

class KUESA_PRIVATE_EXPORT EntityTransformWatcher : public QObject
{
    Q_OBJECT
public:
    explicit EntityTransformWatcher(QObject *parent = nullptr);

    void setTarget(Qt3DCore::QEntity *target);
    Qt3DCore::QEntity *target() const;

    QMatrix4x4 worldMatrix() const;

Q_SIGNALS:
    void worldMatrixChanged(const QMatrix4x4 &worldMatrix);

private:
    void updateWorldMatrix();

    Qt3DCore::QEntity *m_target = nullptr;
    QMatrix4x4 m_worldMatrix;
    bool m_updateRequested = false;
    std::vector<QMetaObject::Connection> m_connections;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // ENTITYTRANSFORMWATCHER_H
