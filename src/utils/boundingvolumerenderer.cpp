/*
    boundingvolumerenderer.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "boundingvolumerenderer.h"
#include <Kuesa/private/kuesa_utils_p.h>

#include <Qt3DCore/QTransform>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTechnique>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QGeometry>
#include <Qt3DCore/QAttribute>
#else
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#endif


#include <QUrl>

using namespace KuesaUtils;
using namespace Qt3DCore;
using namespace Qt3DGeometry;

BoundingVolumeRenderer::BoundingVolumeRenderer(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_color(Qt::yellow)
    , m_updateScheduled(false)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
    qWarning() << "BoundingVolumeRenderer required Qt 5.13 or better";
#endif
    initRendering();
}

BoundingVolumeRenderer::~BoundingVolumeRenderer()
{
    clearSelection();
}

void BoundingVolumeRenderer::select(QEntity *entity)
{
    select(entity, Replace | Recurse);
}

void BoundingVolumeRenderer::select(QEntity *entity, SelectionModes mode)
{
    if (mode & Replace)
        clearSelection();

#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    std::vector<Qt3DCore::QEntity *> workList = { entity };
    bool updatedBV = false;
    while (1) {
        auto e = workList.back();
        workList.pop_back();

        if (e->isEnabled()) {
            const auto &renderers = Kuesa::componentsFromEntity<Qt3DRender::QGeometryRenderer>(e);
            if (renderers.size()) {
                auto renderer = renderers.front();
                auto geometry = renderer->geometry();
                if (geometry->isEnabled()) {
                    auto v = geometry->maxExtent() - geometry->minExtent();
                    if (!v.isNull()) {
                        QMatrix4x4 workMatrix;
                        Qt3DCore::QTransform *workTransform = nullptr;
                        std::vector<Qt3DCore::QTransform *> transformsToWatch;
                        auto p = e;
                        while (p) {
                            const auto &tl = Kuesa::componentsFromEntity<Qt3DCore::QTransform>(p);
                            if (tl.size()) {
                                workTransform = tl.front();
                                transformsToWatch.push_back(workTransform);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                                workMatrix = workTransform->worldMatrix();
                                break;
#else
                                workMatrix = workTransform->matrix() * workMatrix;
#endif
                            }
                            p = p->parentEntity();
                        }

                        SelectionDetails record;
                        record.m_id = e->id();
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
                        record.m_entity = e;
#endif
                        record.m_transform = workTransform;
                        record.m_geometry = geometry;
                        for (auto tr : transformsToWatch) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                            record.m_changeTrackers.push_back(QObject::connect(tr, &Qt3DCore::QTransform::worldMatrixChanged, this, [this, e]() { updateRecord(e); }));
#else
                            record.m_changeTrackers.push_back(QObject::connect(tr, &Qt3DCore::QTransform::matrixChanged, this, [this, e]() { updateRecord(e); }));
#endif
                            record.m_changeTrackers.push_back(QObject::connect(tr, &Qt3DCore::QNode::nodeDestroyed, this, [this, e]() { deselect(e); }));
                        }
                        record.m_changeTrackers.push_back(QObject::connect(geometry, &Qt3DGeometry::QGeometry::minExtentChanged, this, [this, e]() { updateRecord(e); }));
                        record.m_changeTrackers.push_back(QObject::connect(geometry, &Qt3DGeometry::QGeometry::maxExtentChanged, this, [this, e]() { updateRecord(e); }));
                        record.m_changeTrackers.push_back(QObject::connect(geometry, &Qt3DCore::QNode::enabledChanged, this, [this, e](bool enabled) { if (!enabled) deselect(e); }));
                        record.m_changeTrackers.push_back(QObject::connect(entity, &Qt3DCore::QNode::enabledChanged, this, [this, e](bool enabled) { if (!enabled) deselect(e); }));
                        record.m_changeTrackers.push_back(QObject::connect(entity, &Qt3DCore::QNode::nodeDestroyed, this, [this, e]() { deselect(e); }));
                        record.m_changeTrackers.push_back(QObject::connect(geometry, &Qt3DCore::QNode::nodeDestroyed, this, [this, e]() { deselect(e); }));
                        updateRecord(record, false);

                        m_selectionData.push_back(record);
                        updatedBV = true;
                    }

                    if (!(mode & Recurse) && e == entity)
                        break;
                }
            }

            const auto children = e->childNodes();
            for (auto node : children) {
                Qt3DCore::QEntity *f = qobject_cast<Qt3DCore::QEntity *>(node);
                if (f)
                    workList.push_back(f);
            }
        }
        if (workList.empty())
            break;
    }

    if (updatedBV)
        scheduleBufferUpdate();
#endif
}

void BoundingVolumeRenderer::deselect(QEntity *entity)
{
    if (entity)
        deselect(entity->id());
}

void BoundingVolumeRenderer::clearSelection()
{
    for (auto record : m_selectionData) {
        for (auto c : record.m_changeTrackers)
            QObject::disconnect(c);
    }
    m_selectionData.clear();
    m_instanceBuffer->setData({});
    m_view->setInstanceCount(0);
}

void BoundingVolumeRenderer::initRendering()
{
    // clang-format off
    using namespace Qt3DRender;

    // these are required for Kuesa ForwardRenderer
    auto opaqueRenderPassFilterKey = new QFilterKey(this);
    opaqueRenderPassFilterKey->setName(QStringLiteral("KuesaDrawStage"));
    opaqueRenderPassFilterKey->setValue(QStringLiteral("Opaque"));
    auto techniqueFilterKey1 = new QFilterKey(this);
    techniqueFilterKey1->setName(QStringLiteral("renderingStyle"));
    techniqueFilterKey1->setValue(QStringLiteral("forward"));
    auto techniqueFilterKey2 = new QFilterKey(this);
    techniqueFilterKey2->setName(QStringLiteral("allowCulling"));
    techniqueFilterKey2->setValue(false);       // using instanced rendering, culling won't run anyway

    QEffect *flatLinesEffect = new QEffect();
    {
        QTechnique *technique = new QTechnique();
        technique->addFilterKey(techniqueFilterKey1);
        technique->addFilterKey(techniqueFilterKey2);
        QRenderPass *renderPass = new QRenderPass();
        renderPass->addFilterKey(opaqueRenderPassFilterKey);
        QShaderProgram *shaderProgram = new QShaderProgram();

        technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
        technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);
        technique->graphicsApiFilter()->setMajorVersion(3);
        technique->graphicsApiFilter()->setMajorVersion(2);

        shaderProgram->setVertexShaderCode(
                QShaderProgram::loadSource(
                        QUrl(QLatin1String("qrc:/shaders/selection_flatlines.vert"))));
        shaderProgram->setFragmentShaderCode(
                QShaderProgram::loadSource(
                        QUrl(QLatin1String("qrc:/shaders/selection_flatlines.frag"))));

        renderPass->setShaderProgram(shaderProgram);
        technique->addRenderPass(renderPass);
        flatLinesEffect->addTechnique(technique);
    }

    {
        // cube outline
        static const float coordinates[] = {
            -1.f, -1.f, +1.f,   // bfl
            +1.f, -1.f, +1.f,   // bfr
            -1.f, -1.f, -1.f,   // bbl
            +1.f, -1.f, -1.f,   // bbr
            -1.f, +1.f, +1.f,   // tfl
            +1.f, +1.f, +1.f,   // tfr
            -1.f, +1.f, -1.f,   // tbl
            +1.f, +1.f, -1.f,   // tbr
        };

        QByteArray data(reinterpret_cast<const char *>(coordinates), 8 * 3 * sizeof(float));
        Qt3DGeometry::QBuffer *vertexDataBuffer = new Qt3DGeometry::QBuffer;
        vertexDataBuffer->setData(data);

        QAttribute *positionAttribute = new QAttribute();
        positionAttribute->setAttributeType(QAttribute::VertexAttribute);
        positionAttribute->setBuffer(vertexDataBuffer);
        positionAttribute->setVertexBaseType(QAttribute::Float);
        positionAttribute->setVertexSize(3);
        positionAttribute->setCount(8);
        positionAttribute->setName(QAttribute::defaultPositionAttributeName());

        QByteArray indexBufferData;
        indexBufferData.resize(12 * 2 * sizeof(ushort));
        ushort *rawIndexArray = reinterpret_cast<ushort *>(indexBufferData.data());
        int i = 0;
        rawIndexArray[i++] = 0;
        rawIndexArray[i++] = 1;
        rawIndexArray[i++] = 2;
        rawIndexArray[i++] = 3;
        rawIndexArray[i++] = 4;
        rawIndexArray[i++] = 5;
        rawIndexArray[i++] = 6;
        rawIndexArray[i++] = 7;
        rawIndexArray[i++] = 0;
        rawIndexArray[i++] = 4;
        rawIndexArray[i++] = 1;
        rawIndexArray[i++] = 5;
        rawIndexArray[i++] = 2;
        rawIndexArray[i++] = 6;
        rawIndexArray[i++] = 3;
        rawIndexArray[i++] = 7;
        rawIndexArray[i++] = 0;
        rawIndexArray[i++] = 2;
        rawIndexArray[i++] = 1;
        rawIndexArray[i++] = 3;
        rawIndexArray[i++] = 4;
        rawIndexArray[i++] = 6;
        rawIndexArray[i++] = 5;
        rawIndexArray[i++] = 7;

        Qt3DGeometry::QBuffer *indexDataBuffer = new Qt3DGeometry::QBuffer;
        indexDataBuffer->setData(indexBufferData);

        QAttribute *indexAttribute = new QAttribute();
        indexAttribute->setAttributeType(QAttribute::IndexAttribute);
        indexAttribute->setVertexBaseType(QAttribute::UnsignedShort);
        indexAttribute->setVertexSize(1);
        indexAttribute->setCount(24);
        indexAttribute->setBuffer(indexDataBuffer);

        auto customGeometry = new QGeometry;
        customGeometry->addAttribute(positionAttribute);
        customGeometry->addAttribute(indexAttribute);

        m_view = new QGeometryRenderer;
        m_view->setPrimitiveType(QGeometryRenderer::Lines);
        m_view->setGeometry(customGeometry);
        m_view->setVertexCount(24); // 12 segments * 2 points
        m_view->setInstanceCount(0);

        auto customMaterial = new QMaterial;
        customMaterial->setEffect(flatLinesEffect);

        m_colorParameter = new QParameter;
        m_colorParameter->setName(QLatin1String("lineColor"));
        m_colorParameter->setValue(m_color);
        customMaterial->addParameter(m_colorParameter);

        auto customEntity = new Qt3DCore::QEntity(this);
        customEntity->addComponent(m_view);
        customEntity->addComponent(customMaterial);

        m_instanceBuffer = new Qt3DGeometry::QBuffer;
        auto matrixAttribute = new QAttribute;
        matrixAttribute->setAttributeType(QAttribute::VertexAttribute);
        matrixAttribute->setVertexBaseType(QAttribute::Float);
        matrixAttribute->setVertexSize(16);
        matrixAttribute->setDivisor(1);
        matrixAttribute->setName(QLatin1String("selectionMatrix"));
        matrixAttribute->setByteOffset(sizeof(QNodeId));
        matrixAttribute->setByteStride(sizeof(SelectionDetails));
        matrixAttribute->setBuffer(m_instanceBuffer);
        customGeometry->addAttribute(matrixAttribute);
    }

    // clang-format on
}

void BoundingVolumeRenderer::updateRecord(const QEntity *entity)
{
    for (auto &record : m_selectionData) {
        if (record.m_id == entity->id()) {
            updateRecord(record);
            break;
        }
    }
}

void BoundingVolumeRenderer::updateRecord(BoundingVolumeRenderer::SelectionDetails &record, bool updateBuffer)
{
    // HACK: this finds the first renderer in the hierarchy, might not be all the geometry
    // Fix: - continue and grow the extents, but extents are potentially wrong anyway
    //      - or implement render plugin which can run a job to query the backend asynchronously
    //      - in Qt6 use a BoundingVolume to query

    if (!record.m_geometry->isEnabled()) {
        deselect(record.m_id);
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    QMatrix4x4 n;
    auto minExtent = record.m_geometry->minExtent();
    auto maxExtent = record.m_geometry->maxExtent();
    auto scale = (maxExtent - minExtent) / 2.;
    auto center = minExtent + (maxExtent - minExtent) / 2.;
    n.translate(center);
    n.scale(scale);

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    record.m_matrix = record.m_transform->worldMatrix() * n;
#else
    auto p = record.m_entity;
    record.m_matrix = n;
    while (p) {
        const auto &tl = Kuesa::componentsFromEntity<Qt3DCore::QTransform>(p);
        if (tl.size())
            record.m_matrix = tl.front()->matrix() * record.m_matrix;
        p = p->parentEntity();
    }
#endif

    if (updateBuffer)
        scheduleBufferUpdate();
#endif
}

void BoundingVolumeRenderer::scheduleBufferUpdate()
{
    if (m_updateScheduled)
        return;
    m_updateScheduled = true;
    QMetaObject::invokeMethod(this, "updateInstanceBuffer", Qt::QueuedConnection);
}

void BoundingVolumeRenderer::updateInstanceBuffer()
{
    m_updateScheduled = false;
    // TODO avoid duplicating and uploading all the data every time
    m_instanceBuffer->setData({ reinterpret_cast<const char *>(m_selectionData.data()), int(m_selectionData.size() * sizeof(SelectionDetails)) });
    m_view->setInstanceCount(m_selectionData.size());
}

void BoundingVolumeRenderer::deselect(const QNodeId &id)
{
    m_selectionData.erase(std::remove_if(std::begin(m_selectionData), std::end(m_selectionData), [this, id](SelectionDetails &record) {
                              if (record.m_id != id)
                                  return false;
                              for (auto c : record.m_changeTrackers)
                                  QObject::disconnect(c);
                              scheduleBufferUpdate();
                              return true;
                          }),
                          std::end(m_selectionData));
}

QColor BoundingVolumeRenderer::color() const
{
    return m_color;
}

void BoundingVolumeRenderer::setColor(const QColor &color)
{
    m_color = color;
    m_colorParameter->setValue(m_color);
}
