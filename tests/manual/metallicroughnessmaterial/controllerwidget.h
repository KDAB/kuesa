/*
    controllerwidget.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Kevin Ottens <kevin.ottens@kdab.com>

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

#ifndef CONTROLLERWIDGET_H
#define CONTROLLERWIDGET_H

#include <QWidget>

class SceneController;

namespace Ui {
class ControllerWidget;
}

class ControllerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ControllerWidget(SceneController *sceneController,
                              QWidget *parent = nullptr);
    ~ControllerWidget();

signals:
    void baseColorChanged(const QColor &baseColor);
    void emissiveFactorChanged(const QColor &emissiveFactor);

private slots:
    void onBaseColorComponentChanged();
    void onEmissiveFactorComponentChanged();
    void onTextureTransformChanged();

private:
    Ui::ControllerWidget *ui;
    SceneController *m_controller;
};

#endif // CONTROLLERWIDGET_H
