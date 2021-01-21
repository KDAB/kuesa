/*
    window.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_KUESA_UTILS_WINDOW_H
#define KUESA_KUESA_UTILS_WINDOW_H

#include <KuesaUtils/kuesa_utils_global.h>
#include <KuesaUtils/view3dscene.h>

#include <Qt3DRender/qrenderapi.h>

#include <QWindow>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QAspectEngine;
class QEntity;
} // namespace Qt3DCore

namespace Qt3DRender {
class QRenderSettings;
}

namespace KuesaUtils {

class View3DScene;

class KUESAUTILS_SHARED_EXPORT Window : public QWindow
{
public:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    explicit Window(QScreen *screen = nullptr, Qt3DRender::API api = Qt3DRender::API::OpenGL);
#else
    explicit Window(QScreen *screen = nullptr, Qt3DRender::API api = Qt3DRender::API::RHI);
#endif
    ~Window() override;

    View3DScene *scene() const;

protected:
    void showEvent(QShowEvent *e) override;
    void resizeEvent(QResizeEvent *) override;
    bool event(QEvent *e) override;

private:
    void updateAspectRatio();

    Qt3DCore::QAspectEngine *m_aspectEngine;
    Qt3DRender::QRenderSettings *m_renderSettings;
    bool m_initialized;
    View3DScene *m_scene;
};

} // namespace KuesaUtils

QT_END_NAMESPACE

#endif // KUESA_KUESA_UTILS_WINDOW_H
