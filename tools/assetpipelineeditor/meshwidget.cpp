/*
    meshwidget.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "meshwidget.h"
#include "ui_meshwidget.h"
#include "meshinspector.h"
#include <QGridLayout>
#include <QDataWidgetMapper>
#include <QLabel>
#include <QAbstractItemModel>

MeshWidget::MeshWidget(QWidget *parent)
    : QWidget(parent)
    , m_inspector(nullptr)
    , m_ui(new Ui::MeshWidget)
{
    m_ui->setupUi(this);
}

MeshWidget::~MeshWidget()
{
    delete m_ui;
}

void MeshWidget::setMeshInspector(MeshInspector *inspector)
{
    m_inspector = inspector;
    connect(m_inspector, &MeshInspector::meshParamsChanged, this, &MeshWidget::updateData);

    BufferViewer *bufferViewer = new BufferViewer(m_inspector->bufferModel());
    auto layout = new QVBoxLayout(m_ui->bufferGroupBox);
    layout->addWidget(bufferViewer);
}

void MeshWidget::updateData()
{
    m_ui->nameLabel->setText(m_inspector->assetName());
    m_ui->totalSizeLabel->setText(QString::number(m_inspector->totalSize()));
    m_ui->vertexCountLabel->setText(QString::number(m_inspector->vertexCount()));
    m_ui->primitiveTypeLabel->setText(m_inspector->primitiveType());
    m_ui->primitiveCountLabel->setText(QString::number(m_inspector->primitiveCount()));
}

BufferViewer::BufferViewer(QAbstractItemModel *model, QWidget *parent)
    : QScrollArea(parent)
    , m_model(model)
{
    setFrameShape(QFrame::NoFrame);
    auto contentsWidget = new QWidget;
    m_mainLayout = new QVBoxLayout(contentsWidget);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addStretch(1);
    setWidget(contentsWidget);
    setWidgetResizable(true);

    connect(model, &QAbstractItemModel::layoutChanged, this, &BufferViewer::updateDelegates);
    connect(model, &QAbstractItemModel::rowsInserted, this, &BufferViewer::updateDelegates);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &BufferViewer::updateDelegates);
    connect(model, &QAbstractItemModel::rowsMoved, this, &BufferViewer::updateDelegates);
    connect(model, &QAbstractItemModel::modelReset, this, &BufferViewer::updateDelegates);
}

BufferViewer::~BufferViewer()
{
}

void BufferViewer::updateDelegates()
{
    const int numDelegatesRequired = m_model->rowCount();

    // delete any extra delegates from the end, leaving the others in place
    // so their row # stays same
    while (m_delegates.count() > numDelegatesRequired)
        delete m_delegates.takeLast();

    // create more delegates if needed at the end
    int nextDelegateNo = m_delegates.count();
    while (m_delegates.count() < numDelegatesRequired) {
        auto delegate = new DelegateWidget(m_model);
        m_delegates.append(delegate);
        m_mainLayout->insertWidget(nextDelegateNo, delegate);
        ++nextDelegateNo;
    }

    for (int row = 0; row < numDelegatesRequired; ++row) {
        m_delegates[row]->setRow(row);
    }
    m_mainLayout->invalidate();
}

DelegateWidget::DelegateWidget(QAbstractItemModel *model, QWidget *parent)
    : QWidget(parent)
{
    auto nameLabel = new QLabel;
    auto vertexCountLabel = new QLabel;
    auto vertexTypeLabel = new QLabel;
    auto vertexSizeLabel = new QLabel;
    auto bufferIDLabel = new QLabel;
    auto offsetLabel = new QLabel;
    auto strideLabel = new QLabel;

    auto font = nameLabel->font();
    font.setUnderline(true);
    nameLabel->setFont(font);

    auto *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(nameLabel, 0, 0, 1, 4, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel(tr("Vertex Count:")), 1, 0);
    gridLayout->addWidget(vertexCountLabel, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Vertex Type:")), 2, 0);
    gridLayout->addWidget(vertexTypeLabel, 2, 1);
    gridLayout->addWidget(new QLabel(tr("Vertex Size:")), 3, 0);
    gridLayout->addWidget(vertexSizeLabel, 3, 1);
    gridLayout->addWidget(new QLabel(tr("BufferView:")), 1, 2);
    gridLayout->addWidget(bufferIDLabel, 1, 3);
    gridLayout->addWidget(new QLabel(tr("Offset:")), 2, 2);
    gridLayout->addWidget(offsetLabel, 2, 3);
    gridLayout->addWidget(new QLabel(tr("Stride:")), 3, 2);
    gridLayout->addWidget(strideLabel, 3, 3);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(model);
    m_mapper->addMapping(nameLabel, BufferModel::BufferNameColumn, "text");
    m_mapper->addMapping(vertexCountLabel, BufferModel::CountColumn, "text");
    m_mapper->addMapping(vertexTypeLabel, BufferModel::VertexTypeColumn, "text");
    m_mapper->addMapping(vertexSizeLabel, BufferModel::VertexSizeColumn, "text");
    m_mapper->addMapping(bufferIDLabel, BufferModel::BufferViewIndexColumn, "text");
    m_mapper->addMapping(offsetLabel, BufferModel::OffsetColumn, "text");
    m_mapper->addMapping(strideLabel, BufferModel::StrideColumn, "text");
}

DelegateWidget::~DelegateWidget()
{
}

void DelegateWidget::setRow(int row)
{
    m_mapper->setCurrentIndex(row);
}
