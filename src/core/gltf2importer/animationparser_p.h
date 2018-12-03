/*
    animationparser_p.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KUESA_GLTF2IMPORT_ANIMATIONPARSER_P_H
#define KUESA_GLTF2IMPORT_ANIMATIONPARSER_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <QVector>
#include <QJsonArray>
#include <Qt3DAnimation/QChannel>
#include <Qt3DAnimation/QAnimationClipData>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
class QChannelMapping;
class QClipAnimator;
class QAnimationClip;
} // Qt3DAnimation

namespace Kuesa {
namespace GLTF2Import {
class GLTF2Context;

struct ChannelMapping
{
    int targetNodeId = -1;
    QString name;
    QString property;
};

struct Animation
{
    QString name;
    Qt3DAnimation::QAnimationClipData clipData;
    QVector<ChannelMapping> mappings;
};

class Q_AUTOTEST_EXPORT AnimationParser
{
public:
    AnimationParser() = default;

    bool parse(const QJsonArray &animationsArray, GLTF2Context *context);

    enum InterpolationMethod
    {
        Linear,
        Step,
        CubicSpline,
        Unknown
    };

private:
    struct AnimationSampler
    {
        int inputAccessor = -1;
        int outputAccessor = -1;
        InterpolationMethod interpolationMethod = Linear;
    };

    bool checkChannelJSON(const QJsonObject &channelObject) const;
    bool checkSamplerJSON(const QJsonObject &samplerObject) const;

    std::tuple<bool, AnimationSampler>
    animationSamplersFromJson(const QJsonObject &samplerObject) const;

    std::tuple<bool, Qt3DAnimation::QChannel>
    channelFromJson(const QJsonObject &channelObject) const;

    std::tuple<bool, ChannelMapping>
    mappingFromJson(const QJsonObject &channelObject) const;

    Qt3DAnimation::QChannel animationChannelDataFromBuffer(const QString &channelName, const AnimationSampler &sampler) const;
    std::tuple<int, std::vector<float> > animationChannelDataFromData(const AnimationSampler &sampler) const;

    QVector<AnimationSampler> m_samplers;
    GLTF2Context *m_context = nullptr;
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

#endif // KUESA_GLTF2IMPORT_ANIMATIONPARSER_P_H
