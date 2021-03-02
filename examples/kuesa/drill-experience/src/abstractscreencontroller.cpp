/*
    abstractscreencontroller.cpp

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

#include "abstractscreencontroller.h"
#include <Kuesa/View>

/*
    Base class for the various screen controllers of the Drill-Experience
    application.
 */
AbstractScreenController::AbstractScreenController(QObject *parent)
    : QObject(parent)
{
}

KuesaUtils::SceneConfiguration *AbstractScreenController::sceneConfiguration() const
{
    return m_sceneConfiguration;
}

AbstractScreenController::~AbstractScreenController()
{
    if (!m_sceneConfiguration->parent())
        delete m_sceneConfiguration;
}

bool AbstractScreenController::isActive() const
{
    return m_active;
}

void AbstractScreenController::setActive(bool active)
{
    if (active == m_active)
        return;
    m_active = active;
    emit isActiveChanged();
}

const std::vector<Kuesa::View *> &AbstractScreenController::views()
{
    return m_views;
}

void AbstractScreenController::setSceneConfiguration(KuesaUtils::SceneConfiguration *sceneConfiguration)
{
    if (sceneConfiguration == m_sceneConfiguration)
        return;
    m_sceneConfiguration = sceneConfiguration;
    emit sceneConfigurationChanged();
}

void AbstractScreenController::addView(Kuesa::View *v)
{
    auto it = std::find(m_views.begin(), m_views.end(), v);
    if (it == m_views.end()) {
        m_views.push_back(v);
        emit viewsChanged();
    }
}

void AbstractScreenController::removeView(Kuesa::View *v)
{
    auto it = std::remove(m_views.begin(), m_views.end(), v);
    if (it != m_views.end()) {
        m_views.erase(it);
        emit viewsChanged();
    }
}
