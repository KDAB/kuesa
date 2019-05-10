/*
    mainwindow.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <qtkuesa-config.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "collectionmodel.h"
#include "assetinspector.h"
#include "assetinspectorwidget.h"
#include "texturepreviewmaterial.h"
#include "settingsdialog.h"
#include "meshinspector.h"
#include "animationwidget.h"
#include "camerawidget.h"
#include "orbitcameracontroller.h"
#include "exportdialog.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <Qt3DCore/QTransform>
#include <Qt3DAnimation/QAnimationAspect>
#include <Qt3DQuick/QQmlAspectEngine>
#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DRender/private/qpickevent_p.h>
#endif

#include <QFileDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QSettings>
#include <QGuiApplication>
#include <QClipboard>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <GLTF2Importer>

#include <Kuesa/SceneEntity>
#include <Kuesa/private/gltf2exporter_p.h>
#include <Kuesa/private/kuesa_utils_p.h>

namespace {
const QLatin1String LASTPATHSETTING("mainwindow/lastPath");

template<class AssetType>
bool selectAssetType(Qt3DCore::QEntity *node, Kuesa::AbstractAssetCollection *collection,
                     CollectionModel *model, QAbstractItemView *view)
{
    AssetType *picked_asset = Kuesa::componentFromEntity<AssetType>(node);
    if (!picked_asset)
        return false;

    auto index = model->index(collection, picked_asset->objectName());
    if (index.isValid()) {
        view->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
        view->scrollTo(index);
    }
    return index.isValid();
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_view(new Qt3DExtras::Quick::Qt3DQuickWindow)
    , m_entity(nullptr)
    , m_camera(nullptr)
    , m_activeCamera(-1)
    , m_clearColor(Qt::gray)
    , m_renderAreaSize(1024, 768)
{
    m_ui->setupUi(this);
    m_ui->actionOpen->setShortcut(QKeySequence::Open);
    m_ui->actionReload->setShortcut(QKeySequence::Refresh);

    m_ui->actionOpen->setIcon(QIcon(":/icons/open-48.png"));
    m_ui->actionReload->setIcon(QIcon(":/icons/reload-48.png"));
    m_ui->actionViewAll->setIcon(QIcon(":/icons/max-48.png"));

    QSettings settings;
    m_clearColor = settings.value("clearColor", m_clearColor).value<QColor>();
    if (settings.value("defaultClearColor", true).toBool()) {
        QPalette p = this->palette();
        m_clearColor = p.color(QPalette::Base);
    }

    m_generateTangents = settings.value("generateTangents", false).value<bool>();

    connect(m_ui->actionCopy, &QAction::triggered, this, &MainWindow::copyAssetName);
    connect(m_ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_ui->actionReload, &QAction::triggered, this, &MainWindow::reloadFile);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionViewAll, &QAction::triggered, this, &MainWindow::viewAll);
    connect(m_ui->collectionBrowser, &CollectionBrowser::viewCamera, this, QOverload<const QString &>::of(&MainWindow::setCamera));
    connect(m_ui->actionSettings, &QAction::triggered, this, &MainWindow::openSettings);
    connect(m_ui->actionExport, &QAction::triggered, this, &MainWindow::exportFile);
#if !defined(KUESA_DRACO_COMPRESSION)
    m_ui->actionExport->setVisible(false);
#endif

    m_assetInspector = new AssetInspector(this);
    m_assetInspectorWidget = new AssetInspectorWidget(m_assetInspector);
    m_ui->assetInspectorDockWidget->setWidget(m_assetInspectorWidget);

    m_animationWidget = new AnimationWidget;
    m_ui->animationDockWidget->setWidget(m_animationWidget);

    m_cameraWidget = new CameraWidget;
    m_ui->cameraDockWidget->setWidget(m_cameraWidget);
    m_ui->cameraDockWidget->setVisible(false);

    qmlRegisterType<TexturePreviewMaterial>("AssetPipelineEditor", 1, 0, "TexturePreviewMaterial");
    qmlRegisterType<OrbitCameraController>("AssetPipelineEditor", 1, 0, "OrbitCameraController");

    m_view->engine()->qmlEngine()->rootContext()->setContextProperty("_mainWindow", this);
    m_view->registerAspect(new Qt3DAnimation::QAnimationAspect());
    m_view->setSource(QUrl("qrc:/main.qml"));
    setCentralWidget(QWidget::createWindowContainer(m_view, this));

    connect(m_assetInspectorWidget, &AssetInspectorWidget::showAsset, this, &MainWindow::selectAsset);

    connect(m_ui->collectionBrowser, &CollectionBrowser::assetSelected, m_assetInspector, &AssetInspector::setAsset);
    connect(m_ui->collectionBrowser, &CollectionBrowser::assetSelected, this, &MainWindow::assetSelected);

    m_assetInspectorWidget->setPreviewRenderContext(m_view->engine()->qmlEngine()->rootContext());

    m_ui->menuView->addAction(m_ui->collectionBrowserDockWidget->toggleViewAction());
    m_ui->menuView->addAction(m_ui->assetInspectorDockWidget->toggleViewAction());
    m_ui->menuView->addAction(m_ui->animationDockWidget->toggleViewAction());
    m_ui->menuView->addAction(m_ui->cameraDockWidget->toggleViewAction());

    m_ui->collectionBrowser->setModel(new CollectionModel(this));

    m_cameraSelector = new QComboBox(this);
    m_cameraSelector->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_cameraSelector->setEnabled(false);
    m_ui->actionViewAll->setEnabled(false);
    connect(m_cameraSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, QOverload<int>::of(&MainWindow::setCamera));
    m_ui->toolBar->insertWidget(m_ui->actionViewAll, m_cameraSelector);

    connect(m_view, &QWindow::widthChanged, this, &MainWindow::updateCameraAspectRatio);
    connect(m_view, &QWindow::heightChanged, this, &MainWindow::updateCameraAspectRatio);

    const auto state = settings.value(QStringLiteral("mainWindowState")).toByteArray();
    if (!state.isNull())
        restoreState(state);
    const auto geometry = settings.value(QStringLiteral("mainWindowGeometry")).toByteArray();
    if (!geometry.isNull())
        restoreGeometry(geometry);
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.setValue(QStringLiteral("mainWindowState"), saveState());
    delete m_ui;
}

QString MainWindow::filePath() const
{
    return m_filePathURL;
}

void MainWindow::setup(Qt3DRender::QCamera *camera)
{
    m_camera = camera;
    m_cameraWidget->setCamera(camera);
}

void MainWindow::updateScene(Kuesa::SceneEntity *entity)
{
    CollectionModel *model = qobject_cast<CollectionModel *>(m_ui->collectionBrowser->model());
    if (model)
        model->update(entity);
    m_ui->collectionBrowser->expandAll();
    m_ui->actionReload->setEnabled(true);
#if defined(KUESA_DRACO_COMPRESSION)
    m_ui->actionExport->setEnabled(true);
#endif

    m_entity = entity;

    m_animationWidget->update(m_entity);

    m_cameraSelector->blockSignals(true);
    m_cameraSelector->clear();
    m_cameraSelector->addItem("Default", QVariant(-1));
    if (entity->cameras() && entity->cameras()->size()) {
        const QStringList names = entity->cameras()->names();
        for (const QString &n : names)
            m_cameraSelector->addItem(n);
    }
    m_cameraSelector->setEnabled(m_cameraSelector->count() > 1);
    m_cameraSelector->blockSignals(false);
    m_cameraSelector->setCurrentIndex(0);
    setCamera(0);
}

void MainWindow::assetSelected(const QString &assetName, Kuesa::AbstractAssetCollection *collection)
{
    Q_UNUSED(collection);
    m_selectedAssetName = assetName;
    m_ui->actionCopy->setEnabled(!assetName.isEmpty());
}

void MainWindow::setCamera(int index)
{
    if (index < 0)
        return;
    if (index > 0) {
        m_activeCamera = index - 1;
        emit activeCameraChanged(m_activeCamera);
        if (m_cameraSelector->currentIndex() != index)
            m_cameraSelector->setCurrentIndex(index);
        m_ui->actionViewAll->setEnabled(false);
        updateCameraAspectRatio();
        m_cameraWidget->setCamera(m_entity ? m_entity->camera(m_cameraSelector->currentText()) : nullptr);
        m_cameraWidget->setEnabled(false);
        return;
    }

    m_cameraWidget->setCamera(m_camera);
    m_cameraWidget->setEnabled(true);

    m_activeCamera = -1;
    emit activeCameraChanged(m_activeCamera);
    m_ui->actionViewAll->setEnabled(true);

    if (!m_entity)
        return;

    // Names are sorted alphabetically, between consecutive runs, order is always the same
    const QStringList names = m_entity->cameras() ? m_entity->cameras()->names() : QStringList();
    Qt3DRender::QCamera *firstCamera = names.size() > 0 ? qobject_cast<Qt3DRender::QCamera *>(m_entity->camera(names.first())) : nullptr;
    if (firstCamera) {
        m_camera->setProjectionType(firstCamera->projectionType());
        m_camera->setNearPlane(firstCamera->nearPlane());
        m_camera->setFarPlane(firstCamera->farPlane());
        switch (firstCamera->projectionType()) {
        case Qt3DRender::QCameraLens::PerspectiveProjection:
            m_camera->setFieldOfView(firstCamera->fieldOfView());
            break;
        case Qt3DRender::QCameraLens::CustomProjection:
            m_camera->setProjectionMatrix(firstCamera->projectionMatrix());
            break;
        default:
            break;
        }
    }

    // Note: we don't orient the camera like the firstCamera as we don't really care
    // nor know where firstCamera is looking. Rather we rely on our m_camera looking
    // at where the content is using viewAll
    m_camera->setPosition(QVector3D(0.0f, 0.0f, 7.0f));
    m_camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    m_camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
    viewAll();
}

void MainWindow::setCamera(const QString &name)
{
    if (!m_camera || !m_entity || !m_entity->cameras())
        return;

    for (int i = 1; i < m_cameraSelector->count(); ++i) {
        if (name == m_cameraSelector->itemText(i)) {
            m_cameraSelector->setCurrentIndex(i);
            return;
        }
    }
}

void MainWindow::viewAll()
{
    if (!m_camera || m_activeCamera != -1)
        return;

    connect(m_camera, &Qt3DRender::QCamera::viewMatrixChanged, this, &MainWindow::autoNearFarPlanes);
    m_camera->viewAll();
}

void MainWindow::pickEntity(Qt3DRender::QPickEvent *event)
{
    using namespace Qt3DCore;
    using namespace Qt3DRender;

    if (!m_entity || event->button() != QPickEvent::LeftButton)
        return;
    CollectionModel *model = qobject_cast<CollectionModel *>(m_ui->collectionBrowser->model());
    if (!model)
        return;

#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    QEntity *node = event->entity();
#else
    auto event_p = QPickEventPrivate::get(event);
    auto scene_p = QNodePrivate::get(m_entity);
    QEntity *node = qobject_cast<QEntity *>(scene_p->m_scene->lookupNode(event_p->m_entity));
#endif
    if (!node)
        return;

    if (event->modifiers() == Qt::AltModifier) {
        while (node) {
            if (!node->objectName().isEmpty())
                break;
            node = node->parentEntity();
        }
        if (node) {
            auto entityIndex = model->index(m_entity->entities(), node->objectName());
            if (entityIndex.isValid()) {
                m_ui->collectionBrowser->selectionModel()->select(entityIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
                m_ui->collectionBrowser->scrollTo(entityIndex);
            }
        }
    } else if (event->modifiers() == Qt::ShiftModifier) {
        selectAssetType<QMaterial>(node, m_entity->materials(), model, m_ui->collectionBrowser);
    } else {
        selectAssetType<QGeometryRenderer>(node, m_entity->meshes(), model, m_ui->collectionBrowser);
    }
}

int MainWindow::activeCamera() const
{
    return m_activeCamera;
}

QColor MainWindow::clearColor() const
{
    return m_clearColor;
}

QSize MainWindow::renderAreaSize() const
{
    return m_renderAreaSize;
}

bool MainWindow::generateTangents() const
{
    return m_generateTangents;
}

void MainWindow::setFilePath(QString filePath)
{
    auto filePathURL = QUrl::fromLocalFile(filePath).toString();
    if (m_filePathURL == filePathURL)
        return;

    QFileInfo fi(filePath);
    if (!fi.exists())
        return;

    if (m_entity) {
        m_entity->clearCollections();
        m_assetInspector->clear();
        m_animationWidget->update(m_entity);
        CollectionModel *model = qobject_cast<CollectionModel *>(m_ui->collectionBrowser->model());
        if (model)
            model->update(m_entity);
    }

    m_filePathURL = filePathURL;
    emit filePathChanged(m_filePathURL);
    setWindowFilePath(m_filePathURL);
    setWindowTitle(QLatin1String("Kuesa Asset Pipeline Editor - ") + fi.fileName());
    m_ui->actionReload->setEnabled(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    QSettings settings;
    settings.setValue(QStringLiteral("mainWindowState"), saveState());
    settings.setValue(QStringLiteral("mainWindowGeometry"), saveGeometry());
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) {
        QSettings settings;
        if (settings.value("defaultClearColor", true).toBool()) {
            QPalette p = palette();
            m_clearColor = p.color(QPalette::Base);
            emit clearColorChanged(m_clearColor);
        }
    }

    return QMainWindow::event(event);
}

void MainWindow::openFile()
{
    QSettings settings;
    QString lastPath = settings.value(LASTPATHSETTING, QDir::homePath()).toString();
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open GLTF2 File"),
                                                    lastPath, QLatin1String("*.gltf *.glb"));
    if (filePath.isEmpty())
        return;

    QFileInfo fi(filePath);
    settings.setValue(LASTPATHSETTING, fi.absolutePath());

    setFilePath(filePath);
}

void MainWindow::exportFile()
{
    auto importer = m_entity->findChild<Kuesa::GLTF2Importer *>();
    if (!importer)
        return;

    Kuesa::GLTF2Exporter exporter;
    exporter.setContextFromImporter(importer);
    exporter.setScene(m_entity);

    ExportDialog dial{ exporter, QUrl(m_filePathURL).toLocalFile(), this };

    dial.exec();
}

void MainWindow::reloadFile()
{
    // clear filepath, emit, then set the old filepath which will force a reload
    QString c = QUrl(m_filePathURL).toLocalFile();
    m_filePathURL.clear();
    emit filePathChanged(m_filePathURL);
    setFilePath(c);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("Kuesa Asset Pipeline Editor"),
                       tr("Powered by Kuesa 1.0"));
}

void MainWindow::copyAssetName()
{
    if (m_selectedAssetName.isEmpty())
        return;

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(m_selectedAssetName);
}

void MainWindow::selectAsset(const QString &assetName)
{
    m_ui->collectionBrowser->keyboardSearch(assetName);
}

void MainWindow::updateCameraAspectRatio()
{
    const float aspectRatio = static_cast<float>(m_view->width()) / static_cast<float>(m_view->height());

    if (m_camera)
        m_camera->setAspectRatio(aspectRatio);

    auto activeCam = m_entity ? m_entity->camera(m_cameraSelector->currentText()) : nullptr;
    if (activeCam)
        activeCam->setAspectRatio(aspectRatio);

    const auto viewSize = m_view->size();
    if (viewSize.width() > 0 && viewSize.height() > 0) {
        m_renderAreaSize = viewSize;
        emit renderAreaSizeChanged(m_renderAreaSize);
    }
}

void MainWindow::openSettings()
{
    QSettings settings;
    auto clearColor = settings.value("clearColor", QColor(Qt::gray)).value<QColor>();

    SettingsDialog dlg(this);
    dlg.setSelectionColor(m_assetInspector->meshInspector()->selectionColor());
    dlg.setClearColor(clearColor);
    dlg.setDefaultClearColor(settings.value("defaultClearColor", true).toBool());
    dlg.setGenerateTangents(m_generateTangents);
    if (dlg.exec() == QDialog::Accepted) {
        m_assetInspector->meshInspector()->setSelectionColor(dlg.selectionColor());
        settings.setValue("clearColor", dlg.clearColor());
        settings.setValue("defaultClearColor", dlg.defaultClearColor());
        if (dlg.defaultClearColor()) {
            QPalette p = this->palette();
            m_clearColor = p.color(QPalette::Base);
        } else {
            m_clearColor = dlg.clearColor();
        }
        emit clearColorChanged(m_clearColor);

        settings.setValue("generateTangents", dlg.generateTangents());
        m_generateTangents = dlg.generateTangents();
        emit generateTangentsChanged(m_generateTangents);
    }
}

void MainWindow::autoNearFarPlanes()
{
    disconnect(m_camera, &Qt3DRender::QCamera::viewMatrixChanged, this, &MainWindow::autoNearFarPlanes);
    const QVector3D position = m_camera->position();
    const QVector3D viewCenter = m_camera->viewCenter();
    const float sceneExtent = 2.0f * (viewCenter - position).length();
    m_camera->setFarPlane(10.0f * sceneExtent); // add some headroom to be able to zoom out
    m_camera->setNearPlane(0.001f * sceneExtent);
}
