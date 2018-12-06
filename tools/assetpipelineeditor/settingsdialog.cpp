/*
    settingsdialog.cpp

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "mainwindow.h"

#include <QPainter>
#include <QColorDialog>

SettingsDialog::SettingsDialog(MainWindow *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    connect(ui->selectionColorButton, &QToolButton::clicked, this, &SettingsDialog::pickColor);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

QColor SettingsDialog::selectionColor() const
{
    return m_selectionColor;
}

void SettingsDialog::setSelectionColor(QColor selectionColor)
{
    if (m_selectionColor == selectionColor)
        return;

    m_selectionColor = selectionColor;
    emit selectionColorChanged(m_selectionColor);

    QPixmap px(16, 16);
    QPainter p(&px);
    p.fillRect(0, 0, 16, 16, m_selectionColor);
    ui->selectionColorButton->setIcon(QIcon(px));
}

void SettingsDialog::pickColor()
{
    QColor c = QColorDialog::getColor(m_selectionColor, this, tr("Pick Selection Color"));
    setSelectionColor(c);
}
