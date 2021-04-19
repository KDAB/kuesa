/*
    kuesaentity.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "kuesaentity_p.h"
#include <private/qmetaobjectbuilder_p.h>
#include <private/qobject_p.h>
#include <cstring>

QT_BEGIN_NAMESPACE

namespace Kuesa {

// Dynamic Meta Object wrapper around another meta object that contains
// dynamic properties
class KuesaEntityDynamicMetaObject : public QAbstractDynamicMetaObject
{
public:
    explicit KuesaEntityDynamicMetaObject(KuesaEntity *e, QMetaObject *meta)
        : QAbstractDynamicMetaObject()
    {
        QObjectPrivate *op = QObjectPrivate::get(e);

        // Replace or set MetaObject on e with us
        // Ensure proper ownership and cleanup
        op->metaObject = this;
        // Use actual data content from the meta object
        this->d.superdata = meta->d.superdata;
        this->d.data = meta->d.data;
        this->d.stringdata = meta->d.stringdata;
        this->d.extradata = meta->d.extradata;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        this->d.metaTypes = meta->d.metaTypes;
#endif
    }

    ~KuesaEntityDynamicMetaObject()
    {
        // Since we have copied the content of d
        // If we get released, meta also gets released
    }

    int metaCall(QObject *o, QMetaObject::Call c, int _id, void **a) override
    {
        // Calls KuesaEntity::qt_metacall
        return o->qt_metacall(c, _id, a);
    }

    QAbstractDynamicMetaObject *toDynamicMetaObject(QObject *) override
    {
        return this;
    }
};

KuesaEntity::KuesaEntity(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
{
}

KuesaEntity::~KuesaEntity()
{
    // KuesaEntityDynamicMetaObject will be destroyed by QObjectPrivate dtor
    // which calls objectDestroyed() on the QDynamicMetaObjectData (which
    // KuesaEntityDynamicMetaObject is a subclass of) which performs a delete
    // this
}

void KuesaEntity::addExtraProperty(const QString &name, const QVariant &value)
{
    assert(m_metaObject == nullptr);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    m_extraProperties.push_back({ name, value, value.type() });
#else
    m_extraProperties.push_back({ name, value, value.metaType() });
#endif
}

void KuesaEntity::finalize()
{
    assert(m_metaObject == nullptr);

    // Build a runtime metaobject
    QMetaObjectBuilder builder;
    builder.setClassName("KuesaEntity");
    // Qt3D relies on knowing whether a metaObject is dynamic or not
    // so that it knows whether it is safe to use it or not for the construction/destruction changes
    // as dynamic objects are released when the object is, static meta objects aren't
    // see QNodePrivate::findStaticMetaObject
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    builder.setFlags(QMetaObjectBuilder::DynamicMetaObject);
#else
    builder.setFlags(DynamicMetaObject);
#endif
    builder.setSuperClass(&Qt3DCore::QEntity::staticMetaObject);

    for (const DynamicProperty &p : m_extraProperties) {
        const QByteArray pName = std::get<0>(p).toUtf8();
        QMetaPropertyBuilder propertyBuilder = builder.addProperty(pName,
                                                                   std::get<1>(p).typeName());
        propertyBuilder.setWritable(true);
        // Add propertyChanged Signal
        QMetaMethodBuilder signalBuilder = builder.addSignal(pName + "Changed()");
        propertyBuilder.setNotifySignal(signalBuilder);
    }

    QMetaObject *m = builder.toMetaObject();

    // Qt3D expects that non static QMetaObject be marked with the DynamicMetaObject flag
    // That is so that when it records the creation / destroyed changes, it selects only
    // static meta objects to ensure they are always valid

    // QMetaObjectBuilder allows us to set the DynamicMetaObject and create a
    // QMetaObject essentially a malloc of a struct with char *. The problem is
    // that QMetaObject is not a subclass of QDynamicMetaObject but Qt in
    // QMetaObject::indexOfProperty does a cast to QDynamicMetaObject if the
    // DynamicMetaObject is set.

    // This means to have that cast working properly, we need to create a QDynamicMetaObject
    // subclass and copy the content of the QMetaObject built with the builder into it
    m_metaObject = new KuesaEntityDynamicMetaObject(this, m);
}

QStringList KuesaEntity::extraPropertyNames() const
{
    QStringList l;
    l.reserve(m_extraProperties.size());
    for (const DynamicProperty &p : m_extraProperties)
        l.push_back(std::get<0>(p));
    return l;
}

const QMetaObject *KuesaEntity::metaObject() const
{
    return m_metaObject;
}

int KuesaEntity::qt_metacall(QMetaObject::Call call, int id, void **argv)
{
    const int realId = id - m_metaObject->propertyOffset();
    if (realId < 0 || size_t(realId) >= m_extraProperties.size())
        return QObject::qt_metacall(call, id, argv);

    if (call == QMetaObject::ReadProperty) {
        const DynamicProperty &property = m_extraProperties.at(static_cast<size_t>(realId));
        const QVariant &v = std::get<1>(property);
        // return data from type id and pointer to QVariant value
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        const int typeId = std::get<2>(property);
        QMetaType::construct(typeId, argv[0], v.data());
#else
        const QMetaType typeId = std::get<2>(property);
        typeId.construct(argv[0], v.data());
#endif
        return -1;
    } else if (call == QMetaObject::WriteProperty) {
        DynamicProperty &property = m_extraProperties.at(static_cast<size_t>(realId));
        QVariant &v = std::get<1>(property);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        const int typeId = std::get<2>(property);
        // set QVariant from type id and pointer to data
        v = QVariant(typeId, argv[0]);
#else
        const QMetaType typeId = std::get<2>(property);
        v = QVariant(typeId, argv[0]);
#endif
        *reinterpret_cast<int *>(argv[2]) = 1; // setProperty return value
        QMetaObject::activate(this, m_metaObject, realId, nullptr);
        return -1;
    }

    return QObject::qt_metacall(call, id, argv);
}

void *KuesaEntity::qt_metacast(const char *name)
{
    if (std::strcmp(name, m_metaObject->className()) == 0)
        return this;
    return QObject::qt_metacast(name);
}

// Empty Meta Object
const QMetaObject KuesaEntity::staticMetaObject = {};

} // namespace Kuesa

QT_END_NAMESPACE
