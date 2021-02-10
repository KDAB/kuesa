/*
    usermanualscreencontroller.h

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

#ifndef USERMANUALSCREENCONTROLLER_H
#define USERMANUALSCREENCONTROLLER_H

#include "abstractscreencontroller.h"
#include <QHash>

class UserManualScreenController : public AbstractScreenController
{
    Q_OBJECT
    Q_PROPERTY(SelectablePart selectedPart READ selectedPart WRITE setSelectedPart NOTIFY selectedPartChanged)
public:
    enum SelectablePart {
        Trigger,
        Clutch,
        Chuck,
        DirectionSwitch,
        SpeedSwitch,
        BatteryPack,
        NoPartSelected
    };
    Q_ENUM(SelectablePart)

    explicit UserManualScreenController(Qt3DCore::QNode *parent = nullptr);

    void setSelectedPart(SelectablePart selectedPart);
    SelectablePart selectedPart() const;

signals:
    void selectedPartChanged();

private:
    void updateSceneConfiguration();

    SelectablePart m_selectedPart = NoPartSelected;
    QHash<SelectablePart, KuesaUtils::SceneConfiguration *> m_sceneConfigurationsTable;
};

#endif // USERMANUALSCREENCONTROLLER_H
