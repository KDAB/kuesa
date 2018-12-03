/*
    assetinspectorwidget.h

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

#ifndef ASSETINSPECTORWIDGET_H
#define ASSETINSPECTORWIDGET_H

#include <QWidget>

class MaterialWidget;
class MeshWidget;
class TextureWidget;
class AssetInspector;
class QLabel;
class QQmlContext;

class AssetInspectorWidget : public QWidget
{
    Q_OBJECT
public:
    AssetInspectorWidget(AssetInspector *inspector, QWidget *parent = nullptr);
    virtual ~AssetInspectorWidget();

    void updateAssetType();

    void setPreviewRenderContext(QQmlContext *context);

Q_SIGNALS:
    void showAsset(const QString &assetName);

private:
    void setAssetName(const QString &name);

    MaterialWidget *m_materialWidget;
    MeshWidget *m_meshWidget;
    TextureWidget *m_textureWidget;
    AssetInspector *m_assetInspector;

    QLabel *m_assetNameLabel;
    QLabel *m_noAssetLabel;

    QWidget *m_contentsContainer;
    QWidget *m_currentWidget;
};

#endif // ASSETINSPECTORWIDGET_H
