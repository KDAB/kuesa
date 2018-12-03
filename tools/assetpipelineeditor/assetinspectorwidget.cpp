/*
    assetinspectorwidget.cpp

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

#include "assetinspectorwidget.h"
#include "assetinspector.h"
#include "materialinspector.h"
#include "textureinspector.h"
#include "meshinspector.h"
#include "texturewidget.h"
#include "materialwidget.h"
#include "meshwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>

AssetInspectorWidget::AssetInspectorWidget(AssetInspector *inspector, QWidget *parent)
    : QWidget(parent)
    , m_assetInspector(inspector)
    , m_currentWidget(nullptr)
{
    connect(m_assetInspector, &AssetInspector::assetTypeChanged, this, &AssetInspectorWidget::updateAssetType);
    connect(m_assetInspector, &AssetInspector::assetChanged, this, &AssetInspectorWidget::setAssetName);

    m_assetNameLabel = new QLabel;
    auto nameFont = m_assetNameLabel->font();
    nameFont.setBold(true);
    m_assetNameLabel->setFont(nameFont);

    m_noAssetLabel = new QLabel;
    m_noAssetLabel->setWordWrap(true);
    m_noAssetLabel->setIndent(10);

    m_materialWidget = new MaterialWidget;
    m_materialWidget->setMaterialInspector(inspector->materialInspector());
    connect(m_materialWidget, &MaterialWidget::showAsset, this, &AssetInspectorWidget::showAsset);

    m_meshWidget = new MeshWidget;
    m_meshWidget->setMeshInspector(inspector->meshInspector());

    m_textureWidget = new TextureWidget;
    m_textureWidget->setTextureInspector(inspector->textureInspector());

    m_contentsContainer = new QWidget;
    auto contentsLayout = new QVBoxLayout(m_contentsContainer);
    contentsLayout->setContentsMargins(0,0,0,0);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(m_assetNameLabel, 0, Qt::AlignHCenter);
    mainLayout->addWidget(m_noAssetLabel);
    mainLayout->addWidget(m_contentsContainer, 1);

    updateAssetType();
    setAssetName(QString());
}

AssetInspectorWidget::~AssetInspectorWidget()
{
}

void AssetInspectorWidget::updateAssetType()
{
    static QHash<int, QWidget*> widgetHash = {
        { AssetInspector::Unknown, nullptr },
        { AssetInspector::Mesh, m_meshWidget },
        { AssetInspector::Texture, m_textureWidget} ,
        { AssetInspector::Material, m_materialWidget }
    };

    auto assetType = m_assetInspector->assetType();
    if (m_currentWidget) {
        m_contentsContainer->layout()->removeWidget(m_currentWidget);
        m_currentWidget->hide();
    }
    m_currentWidget = widgetHash.value(assetType, nullptr);
    if (m_currentWidget) {
        m_contentsContainer->layout()->addWidget(m_currentWidget);
        m_currentWidget->show();
    }
    m_noAssetLabel->setVisible(assetType == AssetInspector::Unknown);
    m_assetNameLabel->setVisible(assetType != AssetInspector::Unknown);
}

void AssetInspectorWidget::setPreviewRenderContext(QQmlContext *context)
{
    m_materialWidget->setPreviewRenderContext(context);
    m_textureWidget->setPreviewRenderContext(context);
}

void AssetInspectorWidget::setAssetName(const QString &assetName)
{
    m_assetNameLabel->setText(assetName);
    if (assetName.isEmpty())
        m_noAssetLabel->setText(tr("Select an asset in Collection Browser for details"));
    else
    {
        m_noAssetLabel->setText(tr("No information available for %1").arg(assetName));
    }
}
