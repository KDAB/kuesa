/*
    gltf2material.h

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

#ifndef KUESA_GLTF2MATERIAL_H
#define KUESA_GLTF2MATERIAL_H

#include <Qt3DRender/qmaterial.h>
#include <Kuesa/kuesa_global.h>
#include <QtGui/QMatrix3x3>
#include <QtGui/qcolor.h>


QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QAbstractTexture;
class QParameter;
} // namespace Qt3DRender

namespace Kuesa {

class MorphController;

class KUESASHARED_EXPORT GLTF2Material : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QMatrix3x3 textureTransform READ textureTransform WRITE setTextureTransform NOTIFY textureTransformChanged)
    Q_PROPERTY(bool usingColorAttribute READ isUsingColorAttribute WRITE setUsingColorAttribute NOTIFY usingColorAttributeChanged)
    Q_PROPERTY(bool doubleSided READ isDoubleSided WRITE setDoubleSided NOTIFY doubleSidedChanged)
    Q_PROPERTY(bool useSkinning READ useSkinning WRITE setUseSkinning NOTIFY useSkinningChanged)
    Q_PROPERTY(bool opaque READ isOpaque WRITE setOpaque NOTIFY opaqueChanged)
    Q_PROPERTY(float alphaCutoff READ alphaCutoff WRITE setAlphaCutoff NOTIFY alphaCutoffChanged)
    Q_PROPERTY(bool alphaCutoffEnabled READ isAlphaCutoffEnabled WRITE setAlphaCutoffEnabled NOTIFY alphaCutoffEnabledChanged)
    Q_PROPERTY(Kuesa::MorphController *morphController READ morphController NOTIFY morphControllerChanged)

public:
    ~GLTF2Material();

    QMatrix3x3 textureTransform() const;
    bool isUsingColorAttribute() const;
    bool isDoubleSided() const;
    bool useSkinning() const;
    bool isOpaque() const;
    bool isAlphaCutoffEnabled() const;
    float alphaCutoff() const;
    MorphController *morphController() const;

public Q_SLOTS:
    void setTextureTransform(const QMatrix3x3 &textureTransform);
    void setUsingColorAttribute(bool usingColorAttribute);
    void setDoubleSided(bool doubleSided);
    void setUseSkinning(bool useSkinning);
    void setOpaque(bool opaque);
    void setAlphaCutoffEnabled(bool enabled);
    void setAlphaCutoff(float alphaCutoff);
    void setMorphController(MorphController *morphController);

Q_SIGNALS:
    void textureTransformChanged(const QMatrix3x3 &textureTransform);
    void usingColorAttributeChanged(bool usingColorAttribute);
    void doubleSidedChanged(bool doubleSided);
    void useSkinningChanged(bool useSkinning);
    void opaqueChanged(bool opaque);
    void alphaCutoffEnabledChanged(bool enabled);
    void alphaCutoffChanged(float value);
    void morphControllerChanged(MorphController *morphController);

protected:
    explicit GLTF2Material(Qt3DCore::QNode *parent = nullptr);

    Qt3DRender::QParameter *m_textureTransformParameter;
    Qt3DRender::QParameter *m_alphaCutoffParameter;
    Qt3DRender::QParameter *m_morphControllerParameter;
    bool m_isUsingColorAttribute;
    bool m_isDoubleSided;
    bool m_usesSkinning;
    bool m_isOpaque;
    bool m_isAlphaCutOffEnabled;
};

} // Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2MATERIAL_H
