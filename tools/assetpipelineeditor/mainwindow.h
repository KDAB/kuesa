/*
    mainwindow.h

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qt3DQuickExtras/Qt3DQuickWindow>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPickEvent>

class QComboBox;
class AnimationWidget;
class AssetInspectorWidget;
class AssetInspector;
class CameraWidget;

namespace Ui {
class MainWindow;
}

namespace Kuesa {
class SceneEntity;
class AbstractAssetCollection;
} // namespace Kuesa

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(int activeCamera READ activeCamera NOTIFY activeCameraChanged)
    Q_PROPERTY(QColor clearColor READ clearColor NOTIFY clearColorChanged)
    Q_PROPERTY(QSize renderAreaSize READ renderAreaSize NOTIFY renderAreaSizeChanged)
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString filePath() const;
    int activeCamera() const;
    QColor clearColor() const;
    QSize renderAreaSize() const;

    Q_INVOKABLE void setup(Qt3DRender::QCamera *camera);
    Q_INVOKABLE void updateScene(Kuesa::SceneEntity *entity);
    Q_INVOKABLE void setCamera(const QString &name);
    Q_INVOKABLE void viewAll();
    Q_INVOKABLE void pickEntity(Qt3DRender::QPickEvent *event);

    bool event(QEvent *event) override;

public slots:
    void setFilePath(QString filePath);

signals:
    void filePathChanged(QString filePath);
    void activeCameraChanged(int activeCamera);
    void clearColorChanged(QColor clearColor);
    void renderAreaSizeChanged(QSize renderAreaSize);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openFile();
    void exportFile();
    void reloadFile();
    void about();
    void setCamera(int index);
    void assetSelected(const QString &assetName, Kuesa::AbstractAssetCollection *collection);
    void copyAssetName();
    void selectAsset(const QString &assetName);
    void updateCameraAspectRatio();
    void openSettings();
    void autoNearFarPlanes();

private:
    Ui::MainWindow *m_ui;
    QString m_filePathURL;
    Qt3DExtras::Quick::Qt3DQuickWindow *m_view;
    Kuesa::SceneEntity *m_entity;
    Qt3DRender::QCamera *m_camera;
    int m_activeCamera;
    QComboBox *m_cameraSelector;
    AssetInspector *m_assetInspector;
    AssetInspectorWidget *m_assetInspectorWidget;
    AnimationWidget *m_animationWidget;
    QString m_selectedAssetName;
    QColor m_clearColor;
    CameraWidget *m_cameraWidget;
    QSize m_renderAreaSize;
};

#endif // MAINWINDOW_H
