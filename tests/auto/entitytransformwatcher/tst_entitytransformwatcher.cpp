/*
    tst_entitytransformwatcher.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

    Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
    accordance with the Kuesa Enterprise License Agreement provided with the Software in the
    LICENSE.KUESA.ENTERPRISE file.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Kuesa/private/kuesa_utils_p.h>
#include <Kuesa/private/entitytransformwatcher_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qtransform_p.h>

using namespace Kuesa;

class StubTransform : public Qt3DCore::QTransform
{
    Q_OBJECT
public:
    explicit StubTransform(Qt3DCore::QNode *parent = nullptr)
        : Qt3DCore::QTransform(parent)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QObject::connect(this, &StubTransform::matrixChanged,
                         this, [this] {
                             // Compute worldTransform manually (as that's done by the Qt3D backend)
                             QMatrix4x4 worldMatrix = QMatrix4x4();
                             Qt3DCore::QEntity *e = qobject_cast<Qt3DCore::QEntity *>(parentNode());
                             while (e != nullptr) {
                                 const Qt3DCore::QTransform *transform = Kuesa::componentFromEntity<Qt3DCore::QTransform>(e);
                                 if (transform != nullptr)
                                     worldMatrix = transform->matrix() * worldMatrix;
                                 e = e->parentEntity();
                             }
                             Qt3DCore::QTransformPrivate *dNode =
                                     static_cast<Qt3DCore::QTransformPrivate *>(Qt3DCore::QNodePrivate::get(this));
                             dNode->setWorldMatrix(worldMatrix);

                             // Call matrixChanged on any child transform to force them to update their worldTransform
                             e = qobject_cast<Qt3DCore::QEntity *>(parentNode());
                             if (!e)
                                 return;
                             for (Qt3DCore::QNode *c : e->childNodes()) {
                                 Qt3DCore::QEntity *ce = qobject_cast<Qt3DCore::QEntity *>(c);
                                 if (!ce)
                                     continue;
                                 StubTransform *transform = Kuesa::componentFromEntity<StubTransform>(ce);
                                 if (transform != nullptr)
                                     transform->matrixChanged();
                             }
                         });
#endif
    }
};

class tst_TargetTransformWatcher : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkWatchesTransformChangeOnTarget()
    {
        // GIVEN
        Qt3DCore::QEntity target;
        StubTransform *transform = new StubTransform();
        target.addComponent(transform);

        EntityTransformWatcher watcher;
        QSignalSpy worldMatrixSpy(&watcher, &EntityTransformWatcher::worldMatrixChanged);

        // THEN
        QVERIFY(worldMatrixSpy.isValid());

        // WHEN
        watcher.setTarget(&target);

        // THEN
        QCOMPARE(watcher.target(), &target);
        QCOMPARE(worldMatrixSpy.count(), 1);
        worldMatrixSpy.clear();

        // WHEN
        transform->setTranslation(QVector3D(1.0f, 0.0f, 0.0f));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(worldMatrixSpy.count(), 1);
        const QMatrix4x4 m = watcher.worldMatrix();
        QCOMPARE(m.column(3)[0], 1.0f);
    }

    void checkWatchesTransformChangeOnTargetHierarchy()
    {
        // GIVEN
        Qt3DCore::QEntity parent;
        StubTransform *parentTransform = new StubTransform();
        parent.addComponent(parentTransform);

        Qt3DCore::QEntity *intermediateEntity = new Qt3DCore::QEntity(&parent);
        StubTransform *intermediateTransform = new StubTransform();
        intermediateEntity->addComponent(intermediateTransform);

        Qt3DCore::QEntity *target = new Qt3DCore::QEntity(intermediateEntity);
        StubTransform *targetTransform = new StubTransform();
        target->addComponent(targetTransform);

        EntityTransformWatcher watcher;
        QSignalSpy worldMatrixSpy(&watcher, &EntityTransformWatcher::worldMatrixChanged);

        // THEN
        QVERIFY(worldMatrixSpy.isValid());

        // WHEN
        watcher.setTarget(target);

        // THEN
        QCOMPARE(watcher.target(), target);
        QCOMPARE(worldMatrixSpy.count(), 1);
        worldMatrixSpy.clear();

        // WHEN
        parentTransform->setTranslation(QVector3D(1.0f, 0.0f, 0.0f));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(worldMatrixSpy.count(), 1);
        const QMatrix4x4 m = watcher.worldMatrix();
        QCOMPARE(m.column(3)[0], 1.0f);
        worldMatrixSpy.clear();

        // WHEN
        intermediateTransform->setTranslation(QVector3D(2.0f, 0.0f, 0.0f));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(worldMatrixSpy.count(), 1);
        const QMatrix4x4 m2 = watcher.worldMatrix();
        QCOMPARE(m2.column(3)[0], 3.0f);
        worldMatrixSpy.clear();
    }

    void checkWatchingUnsetOnTargetChange()
    {
        // GIVEN
        Qt3DCore::QEntity target1;
        StubTransform *transform1 = new StubTransform();
        target1.addComponent(transform1);

        Qt3DCore::QEntity target2;
        StubTransform *transform2 = new StubTransform();
        target2.addComponent(transform2);

        EntityTransformWatcher watcher;
        QSignalSpy worldMatrixSpy(&watcher, &EntityTransformWatcher::worldMatrixChanged);

        transform1->setTranslation(QVector3D(-5.0f, 0.0f, 0.0f));
        transform2->setTranslation(QVector3D(5.0f, 0.0f, 0.0f));

        // THEN
        QVERIFY(worldMatrixSpy.isValid());

        // WHEN
        watcher.setTarget(&target1);

        // THEN
        QCOMPARE(watcher.target(), &target1);
        QCOMPARE(worldMatrixSpy.count(), 1);
        worldMatrixSpy.clear();
        const QMatrix4x4 m = watcher.worldMatrix();
        QCOMPARE(m.column(3)[0], -5.0f);

        // WHEN
        watcher.setTarget(&target2);

        // THEN
        QCOMPARE(watcher.target(), &target2);
        QCOMPARE(worldMatrixSpy.count(), 1);
        worldMatrixSpy.clear();
        const QMatrix4x4 m2 = watcher.worldMatrix();
        QCOMPARE(m2.column(3)[0], 5.0f);

        // WHEN
        transform1->setTranslation(QVector3D(1.0f, 1.0f, 1.0f));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(worldMatrixSpy.count(), 0);
        const QMatrix4x4 m3 = watcher.worldMatrix();
        QCOMPARE(m3.column(3)[0], 5.0f);
    }
};

QTEST_MAIN(tst_TargetTransformWatcher)

#include "tst_entitytransformwatcher.moc"
