/*
    abstractscreencontroller.h

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

#ifndef ABSTRACTSCREENCONTROLLER_H
#define ABSTRACTSCREENCONTROLLER_H

#include <KuesaUtils/sceneconfiguration.h>
#include <Kuesa/KuesaNode>

namespace Kuesa {
class View;
} // namespace Kuesa

class AbstractScreenController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KuesaUtils::SceneConfiguration *sceneConfiguration READ sceneConfiguration NOTIFY sceneConfigurationChanged)
    Q_PROPERTY(bool isActive READ isActive WRITE setActive NOTIFY isActiveChanged)

public:
    KuesaUtils::SceneConfiguration *sceneConfiguration() const;
    virtual ~AbstractScreenController();

    bool isActive() const;
    void setActive(bool active);

    const std::vector<Kuesa::View *> &views();

protected:
    explicit AbstractScreenController(QObject *parent = nullptr);
    void setSceneConfiguration(KuesaUtils::SceneConfiguration *sceneConfiguration);

signals:
    void sceneConfigurationChanged();
    void isActiveChanged();

private:
    KuesaUtils::SceneConfiguration *m_sceneConfiguration = nullptr;
    bool m_active = false;
};

#endif // ABSTRACTSCREENCONTROLLER_H
