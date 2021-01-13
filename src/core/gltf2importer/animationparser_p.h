/*
    animationparser_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <functional>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QNode;
}

namespace Qt3DAnimation {
class QChannelMapping;
class QClipAnimator;
class QAnimationClip;
class QChannelMapper;
} // namespace Qt3DAnimation

namespace Kuesa {
namespace GLTF2Import {
class GLTF2Context;
struct TreeNode;

struct AnimationTarget {
    enum TargetType {
        Node = 0,
        Material,
        Light,
        Camera,
        UnknownType
    };
    TargetType type;
    qint32 targetNodeId;
    QString path;
};

struct ChannelInfo {
    qint32 sampler;
    AnimationTarget target;
};

struct ChannelMapping;
using AnimatableMappingsGenerator = std::function<std::vector<Qt3DAnimation::QChannelMapping *>(const GLTF2Context *, const ChannelMapping &)>;

struct ChannelMapping {
    AnimationTarget target;
    QString name;
    QString property;
    AnimatableMappingsGenerator generator;
};

struct Animation {
    QString name;
    Qt3DAnimation::QAnimationClipData clipData;
    QVector<ChannelMapping> mappings;
    Qt3DAnimation::QAnimationClip *clip = nullptr;
    Qt3DAnimation::QChannelMapper *mapper = nullptr;
};

class Q_AUTOTEST_EXPORT ExtPropertyAnimationHandler
{
public:
    bool parse(const QJsonObject &extensionObject);
    QVector<ChannelInfo> channelsInfo() const;

private:
    QVector<ChannelInfo> m_channelsInfo;
};

class Q_AUTOTEST_EXPORT AnimationParser
{
public:
    AnimationParser() = default;

    static void registerAnimatableProperty(const QString gltfPath,
                                           int componentCount,
                                           const QString &targetPropertyName,
                                           const AnimatableMappingsGenerator &mappingsGenerator,
                                           const QString &channelBaseName = QString());

    bool parse(const QJsonArray &animationsArray, GLTF2Context *context);

    enum InterpolationMethod {
        Linear,
        Step,
        CubicSpline,
        Unknown
    };

private:
    struct AnimationSampler {
        int inputAccessor = -1;
        int outputAccessor = -1;
        InterpolationMethod interpolationMethod = Linear;
    };

    bool checkChannelInfo(const ChannelInfo &channelInfo) const;
    bool checkSamplerJSON(const QJsonObject &samplerObject) const;

    ChannelInfo channelInfoFromJson(const QJsonObject &channelObject) const;

    std::tuple<bool, AnimationSampler>
    animationSamplersFromJson(const QJsonObject &samplerObject) const;

    std::tuple<bool, Qt3DAnimation::QChannel>
    channelFromChannelInfo(const ChannelInfo &channelInfo) const;

    std::tuple<bool, ChannelMapping>
    mappingForChannel(const ChannelInfo &channelInfo, const QString &channelName) const;

    Qt3DAnimation::QChannel animationChannelDataFromBuffer(const ChannelInfo &channelInfo,
                                                           const AnimationSampler &sampler) const;
    std::tuple<int, std::vector<float>> animationChannelDataFromData(const AnimationSampler &sampler) const;

    QVector<AnimationSampler> m_samplers;
    GLTF2Context *m_context = nullptr;
};

} // namespace GLTF2Import
} // namespace Kuesa

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Kuesa::GLTF2Import::AnimationTarget)
Q_DECLARE_METATYPE(Kuesa::GLTF2Import::ChannelInfo)

#endif // KUESA_GLTF2IMPORT_ANIMATIONPARSER_P_H
