/*
    texturewidget.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "texturewidget.h"
#include "ui_texturewidget.h"
#include "textureinspector.h"
#include "renderedpreviewwidget.h"
#include <QCheckBox>
#include <QDataWidgetMapper>
#include <QLineEdit>
#include <QToolButton>
#include <QtWidgets/QAction>
#include <QDesktopServices>
#include <QResizeEvent>
#include <QScrollBar>
#include <QQmlContext>

TextureWidget::TextureWidget(QWidget *parent)
    : QWidget(parent)
    , m_inspector(nullptr)
    , ui(new Ui::TextureWidget)
    , m_previewWidget(new RenderedPreviewWidget)
{
    ui->setupUi(this);

    m_previewWidget->setAlignment(Qt::AlignHCenter);
    auto hBoxLayout = new QHBoxLayout(ui->texturePreviewGroupBox);
    hBoxLayout->addWidget(m_previewWidget);
    ui->texturePreviewGroupBox->setLayout(hBoxLayout);
}

TextureWidget::~TextureWidget()
{
    delete ui;
}

void TextureWidget::setTextureInspector(TextureInspector *inspector)
{
    m_inspector = inspector;
    QObject::connect(m_inspector, &TextureInspector::textureParamsChanged,
                     this, &TextureWidget::updateData);
    TextureImageViewer *imageViewer = new TextureImageViewer(m_inspector->textureImagesModel());
    imageViewer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    auto layout = new QVBoxLayout(ui->imagesGroupBox);
    layout->addWidget(imageViewer);
}

void TextureWidget::setPreviewRenderContext(QQmlContext *context)
{
    // Set context properties for TexturePreview rendering
    context->setContextProperty("_textureCaptureReceiver", m_previewWidget);
    context->setContextProperty("_textureInspector", m_inspector);
}

void TextureWidget::updateData()
{
    ui->nameValue->setText(m_inspector->assetName());
    ui->formatValue->setText(m_inspector->format());
    ui->targetValue->setText(m_inspector->target());
    ui->widthValue->setText(QString::number(m_inspector->width()));
    ui->heightValue->setText(QString::number(m_inspector->height()));
    ui->depthValue->setText(QString::number(m_inspector->depth()));
    ui->layersValue->setText(QString::number(m_inspector->layers()));
    ui->samplesValue->setText(QString::number(m_inspector->samples()));
    ui->minificationValue->setText(m_inspector->minificationFilter());
    ui->magnificationValue->setText(m_inspector->magnificationFilter());
    ui->wrapXValue->setText(m_inspector->wrapXMode());
    ui->wrapYValue->setText(m_inspector->wrapYMode());
    ui->wrapZValue->setText(m_inspector->wrapZMode());
}

TextureImageViewer::TextureImageViewer(QAbstractItemModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    connect(model, &QAbstractItemModel::layoutChanged, this, &TextureImageViewer::updateDelegates);
    connect(model, &QAbstractItemModel::rowsInserted, this, &TextureImageViewer::updateDelegates);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &TextureImageViewer::updateDelegates);
    connect(model, &QAbstractItemModel::rowsMoved, this, &TextureImageViewer::updateDelegates);
    connect(model, &QAbstractItemModel::modelReset, this, &TextureImageViewer::updateDelegates);
}

TextureImageViewer::~TextureImageViewer()
{
}

void TextureImageViewer::updateDelegates()
{
    const int numDelegatesRequired = m_model->rowCount();

    // delete any extra delegates from the end, leaving the others in place
    // so their row # stays same
    while (m_delegates.count() > numDelegatesRequired)
        delete m_delegates.takeLast();

    // create more delegates if needed at the end
    int nextDelegateNo = m_delegates.count();
    while (m_delegates.count() < numDelegatesRequired) {
        auto delegate = new TextureImageDelegateWidget(m_model);
        delegate->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        m_delegates.append(delegate);
        m_mainLayout->insertWidget(nextDelegateNo, delegate);
        ++nextDelegateNo;
    }

    for (int row = 0; row < numDelegatesRequired; ++row) {
        m_delegates[row]->setRow(row);
    }
    m_mainLayout->invalidate();
}

TextureImageDelegateWidget::TextureImageDelegateWidget(QAbstractItemModel *model, QWidget *parent)
    : QWidget(parent)
{
    auto nameValue = new QLineEdit;
    nameValue->setReadOnly(true);
    auto mipLevelValue = new QLabel;
    auto layerValue = new QLabel;
    auto faceValue = new QLabel;
    auto sourveValue = new QLineEdit;
    sourveValue->setReadOnly(true);
    auto mirroredValue = new QCheckBox;
    mirroredValue->setEnabled(false);
    auto openImage = new QToolButton;
    openImage->setArrowType(Qt::NoArrow);
    auto openAction = new QAction("\u2750");
    openImage->setDefaultAction(openAction);
    connect(openAction, &QAction::triggered, this, [sourveValue]() {
        QDesktopServices::openUrl(QUrl(sourveValue->text()));
    });

    auto *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(new QLabel(tr("Name:")), 0, 0);
    gridLayout->addWidget(nameValue, 0, 1, 1, 3);
    gridLayout->addWidget(new QLabel(tr("Source:")), 1, 0);
    gridLayout->addWidget(sourveValue, 1, 1, 1, 2);
    gridLayout->addWidget(openImage, 1, 3);
    gridLayout->addWidget(new QLabel(tr("Miplevel:")), 2, 0);
    gridLayout->addWidget(mipLevelValue, 2, 1);
    gridLayout->addWidget(new QLabel(tr("Layer:")), 2, 2);
    gridLayout->addWidget(layerValue, 2, 3);
    gridLayout->addWidget(new QLabel(tr("Face:")), 3, 0);
    gridLayout->addWidget(faceValue, 3, 1);
    gridLayout->addWidget(new QLabel(tr("Mirrored:")), 3, 2);
    gridLayout->addWidget(mirroredValue, 3, 3);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(model);
    m_mapper->addMapping(nameValue, TextureImagesModel::ImageNameColumn, "text");
    m_mapper->addMapping(mipLevelValue, TextureImagesModel::MipLevelColumn, "text");
    m_mapper->addMapping(layerValue, TextureImagesModel::LayerColumn, "text");
    m_mapper->addMapping(faceValue, TextureImagesModel::FaceColumn, "text");
    m_mapper->addMapping(sourveValue, TextureImagesModel::SourceColumn, "text");
    m_mapper->addMapping(mirroredValue, TextureImagesModel::MirroredColumn, "checked");
}

TextureImageDelegateWidget::~TextureImageDelegateWidget()
{
}

void TextureImageDelegateWidget::setRow(int row)
{
    m_mapper->setCurrentIndex(row);
}

FixedWidthScrollArea::FixedWidthScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
}

void FixedWidthScrollArea::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);
    widget()->resize(width() - (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0),
                     widget()->sizeHint().height());
}
