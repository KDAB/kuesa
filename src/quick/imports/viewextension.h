/*
    viewextension.h

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

#ifndef KUESA_VIEW_EXTENSION_H
#define KUESA_VIEW_EXTENSION_H

#include <QObject>
#include <QtQml/QQmlListProperty>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QLayer;
}

namespace Kuesa {

class AbstractPostProcessingEffect;
class ReflectionPlane;

class ViewExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Kuesa::AbstractPostProcessingEffect> postProcessingEffects READ postProcessingEffects CONSTANT)
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QLayer> layers READ layers CONSTANT)
    Q_PROPERTY(QQmlListProperty<Kuesa::ReflectionPlane> reflectionPlanes READ reflectionPlanes CONSTANT)
public:
    explicit ViewExtension(QObject *parent = nullptr);

    QQmlListProperty<Kuesa::AbstractPostProcessingEffect> postProcessingEffects();
    QQmlListProperty<Qt3DRender::QLayer> layers();
    QQmlListProperty<Kuesa::ReflectionPlane> reflectionPlanes();

private:
    static void appendFX(QQmlListProperty<AbstractPostProcessingEffect> *list, AbstractPostProcessingEffect *fx);
    static AbstractPostProcessingEffect *fxAt(QQmlListProperty<AbstractPostProcessingEffect> *list, int index);
    static int fxCount(QQmlListProperty<AbstractPostProcessingEffect> *list);
    static void clearFx(QQmlListProperty<AbstractPostProcessingEffect> *list);

    static void appendLayer(QQmlListProperty<Qt3DRender::QLayer> *list, Qt3DRender::QLayer *layer);
    static Qt3DRender::QLayer *layerAt(QQmlListProperty<Qt3DRender::QLayer> *list, int index);
    static int layersCount(QQmlListProperty<Qt3DRender::QLayer> *list);
    static void clearLayers(QQmlListProperty<Qt3DRender::QLayer> *list);

    static void appendReflectionPlane(QQmlListProperty<Kuesa::ReflectionPlane> *list, Kuesa::ReflectionPlane *plane);
    static Kuesa::ReflectionPlane *reflectionPlaneAt(QQmlListProperty<Kuesa::ReflectionPlane> *list, int index);
    static int reflectionPlaneCount(QQmlListProperty<Kuesa::ReflectionPlane> *list);
    static void clearReflectionPlanes(QQmlListProperty<Kuesa::ReflectionPlane> *list);
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_VIEW_EXTENSION_H
