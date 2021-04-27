#ifndef STENCILHELPER_H
#define STENCILHELPER_H

#include <QObject>
#include <Qt3DCore/QEntity>

class StencilHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QEntity *entity READ entity WRITE setEntity NOTIFY entityChanged)
public:
    explicit StencilHelper(QObject *parent = nullptr);

    Qt3DCore::QEntity *entity() const;

public Q_SLOTS:
    void setEntity(Qt3DCore::QEntity *entity);

Q_SIGNALS:
    void entityChanged();

private:
    Qt3DCore::QEntity *m_entity;
};

#endif // STENCILHELPER_H
