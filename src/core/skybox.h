/*
    skybox.h

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

#ifndef KUESA_SKYBOX_H
#define KUESA_SKYBOX_H

#include <Qt3DCore/qentity.h>
#include <Kuesa/kuesa_global.h>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QParameter;
class QAbstractTexture;
} // namespace Qt3DRender

namespace Kuesa {

class KUESASHARED_EXPORT Skybox : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(QString baseName READ baseName WRITE setBaseName NOTIFY baseNameChanged)
    Q_PROPERTY(QString extension READ extension WRITE setExtension NOTIFY extensionChanged)
    Q_PROPERTY(bool gammaCorrect READ isGammaCorrectEnabled WRITE setGammaCorrectEnabled NOTIFY gammaCorrectEnabledChanged)
public:
    explicit Skybox(Qt3DCore::QNode *parent = nullptr);
    ~Skybox();

    QString baseName() const;
    QString extension() const;
    bool isGammaCorrectEnabled() const;

public Q_SLOTS:
    void setBaseName(const QString &baseName);
    void setExtension(const QString &extension);
    void setGammaCorrectEnabled(bool enabled);

Q_SIGNALS:
    void baseNameChanged(const QString &path);
    void extensionChanged(const QString &extension);
    void gammaCorrectEnabledChanged(bool enabled);

private:
    void reloadTexture();

    Qt3DRender::QParameter *m_gammaStrengthParameter;
    Qt3DRender::QParameter *m_textureParameter;
    Qt3DRender::QAbstractTexture *m_texture;

    QString m_extension;
    QString m_baseName;
    bool m_hasPendingReloadTextureCall;
};

} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_SKYBOX_H
