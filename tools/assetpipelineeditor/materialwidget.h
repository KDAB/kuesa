/*
    materialwidget.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

#ifndef MATERIALWIDGET_H
#define MATERIALWIDGET_H

#include <QWidget>
#include <QMatrix3x3>
class QQmlContext;

class MaterialInspector;
class QLabel;
class QGridLayout;

namespace Qt3DRender {
class QAbstractTexture;
}

namespace Ui {
class MaterialWidget;
}

class MaterialWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MaterialWidget(QWidget *parent = nullptr);
    virtual ~MaterialWidget();

    void setMaterialInspector(MaterialInspector *inspector);
    void setPreviewRenderContext(QQmlContext *context);

Q_SIGNALS:
    void showAsset(const QString &assetName);

private Q_SLOTS:
    void updateData();

private:
    QString boolTextValue(bool value) const;
    void setLabelColor(QLabel *label, const QColor &color);
    void setTextureLabelState(QLabel *nameLabel, QLabel *valueLabel, Qt3DRender::QAbstractTexture *texture) const;

    MaterialInspector *m_inspector;
    Ui::MaterialWidget *m_ui;
};

class MatrixLabel : public QWidget
{
    Q_OBJECT
public:
    MatrixLabel(QWidget *parent = nullptr);
    virtual ~MatrixLabel() {}

    void setMatrix(const QMatrix3x3 &matrix);
private:
    QGridLayout *m_gridLayout;
};

#endif // MATERIALWIDGET_H
