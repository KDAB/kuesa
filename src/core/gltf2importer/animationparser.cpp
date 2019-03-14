/*
    bufferviewsparser_p.h

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

#include "animationparser_p.h"

#include "gltf2context_p.h"
#include "kuesa_p.h"

#include <QJsonValue>
#include <QJsonObject>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DAnimation/QChannel>
#include <Qt3DAnimation/QAnimationClipData>

#include <cstring>

QT_BEGIN_NAMESPACE

using namespace Kuesa::GLTF2Import;

namespace {

const QLatin1String KEY_NAME = QLatin1Literal("name");
const QLatin1String KEY_CHANNELS = QLatin1Literal("channels");
const QLatin1String KEY_SAMPLERS = QLatin1Literal("samplers");
const QLatin1String KEY_SAMPLER = QLatin1Literal("sampler");
const QLatin1String KEY_TARGET = QLatin1Literal("target");
const QLatin1String KEY_INPUT = QLatin1Literal("input");
const QLatin1String KEY_OUTPUT = QLatin1Literal("output");
const QLatin1String KEY_INTERPOLATION = QLatin1Literal("interpolation");
const QLatin1String KEY_PATH = QLatin1Literal("path");
const QLatin1String KEY_NODE = QLatin1Literal("node");

AnimationParser::InterpolationMethod interpolationMethodFromSemantic(const QString &value)
{
    if (value == QStringLiteral("LINEAR"))
        return AnimationParser::Linear;
    if (value == QStringLiteral("STEP"))
        return AnimationParser::Step;
    if (value == QStringLiteral("CUBICSPLINE"))
        return AnimationParser::CubicSpline;

    return AnimationParser::Unknown;
}

quint8 accessorDataTypeToBytes(Qt3DRender::QAttribute::VertexBaseType type)
{
    switch (type) {
    case Qt3DRender::QAttribute::Byte:
    case Qt3DRender::QAttribute::UnsignedByte:
        return 1;
    case Qt3DRender::QAttribute::Short:
    case Qt3DRender::QAttribute::UnsignedShort:
        return 2;
    case Qt3DRender::QAttribute::Float:
        return 4;
    default:
        qCWarning(kuesa, "Invalid data type");
        return 0;
    }
}

QByteArray rawDataFromAccessor(const Accessor &accessor, GLTF2Context *ctx)
{
    if (accessor.bufferData.isEmpty()) {
        qCWarning(kuesa, "No Buffer found for accessor");
        return {};
    }

    const BufferView bufferViewData = ctx->bufferView(accessor.bufferViewIndex);

    const QByteArray bufferData = accessor.bufferData;

    // BufferData was generated using the bufferView's byteOffset
    const qint32 byteOffset = accessor.offset;
    const quint8 elemByteSize = accessorDataTypeToBytes(accessor.type);
    const qint16 byteStride = (!bufferViewData.bufferData.isEmpty() && bufferViewData.byteStride > 0 ? bufferViewData.byteStride : qint16(accessor.dataSize * elemByteSize));

    if (byteStride < qint16(elemByteSize * accessor.dataSize)) {
        qCWarning(kuesa, "Buffer Data byteStride doesn't match accessor dataSize and byte size for type");
        return {};
    }

    const qint32 subBufferByteLen = accessor.count * byteStride;
    if (byteOffset + subBufferByteLen > bufferData.size()) {
        qCWarning(kuesa, "Buffer Data size incompatible with accessor requirement");
        return {};
    }

    // Copy data based on bufferView strides accessors datasize and bufferview
    // and accessor cumulative start offset

    QByteArray outputData;
    outputData.resize(elemByteSize * accessor.count * accessor.dataSize);

    const char *rawBytes = bufferData.constData() + byteOffset;
    char *outputDataRawBytes = outputData.data();

    const qint32 vertexByteSize = elemByteSize * accessor.dataSize;
    for (quint32 i = 0; i < accessor.count; ++i) {
        std::memcpy(outputDataRawBytes, rawBytes, static_cast<size_t>(vertexByteSize));
        rawBytes += byteStride;
        outputDataRawBytes += vertexByteSize;
    }

    return outputData;
}

template<typename T>
std::vector<float> normalizeTypedChannelData(const QByteArray &rawData)
{
    const size_t size = static_cast<size_t>(rawData.size()) / sizeof(T);
    std::vector<float> channelData(size);

    const T *typedChannelData = reinterpret_cast<const T *>(rawData.constData());

    if (std::numeric_limits<T>::is_signed) {
        for (size_t i = 0; i < size; ++i)
            channelData[i] = std::max(static_cast<float>(typedChannelData[i]) / float(std::numeric_limits<T>::max()), -1.0f);
    } else {
        for (size_t i = 0; i < size; ++i)
            channelData[i] = static_cast<float>(typedChannelData[i]) / float(std::numeric_limits<T>::max());
    }

    return channelData;
}

QString channelPathToName(const QString &path)
{
    if (path == QStringLiteral("translation"))
        return QStringLiteral("Location");
    if (path == QStringLiteral("rotation"))
        return QStringLiteral("Rotation");
    if (path == QStringLiteral("scale"))
        return QStringLiteral("Scale3D");
    if (path == QStringLiteral("weights"))
        return QStringLiteral("MorphWeights");
    return {};
}

QString channelPathToProperty(const QString &path)
{
    if (path == QStringLiteral("translation"))
        return QStringLiteral("translation");
    if (path == QStringLiteral("rotation"))
        return QStringLiteral("rotation");
    if (path == QStringLiteral("scale"))
        return QStringLiteral("scale3D");
    if (path == QStringLiteral("weights"))
        return QStringLiteral("morphWeights");
    return {};
}

quint8 expectedComponentsCountForChannel(const QString &channelName)
{
    if (channelName == QStringLiteral("translation"))
        return 3;
    if (channelName == QStringLiteral("rotation"))
        return 4;
    if (channelName == QStringLiteral("scale"))
        return 3;
    if (channelName == QStringLiteral("weights"))
        return 1;

    qCWarning(kuesa) << "Unrecognized animation channel" << channelName;
    return 0;
}

} // namespace

std::tuple<int, std::vector<float>> AnimationParser::animationChannelDataFromData(const AnimationParser::AnimationSampler &sampler) const
{
    std::vector<float> channelData;
    if (sampler.outputAccessor < 0 || sampler.outputAccessor > m_context->accessorCount()) {
        qCWarning(kuesa, "Invalid input accessor id");
        return std::make_tuple(0, channelData);
    }
    const Accessor outputAccessor = m_context->accessor(sampler.outputAccessor);
    const QByteArray outputRawData = rawDataFromAccessor(outputAccessor, m_context);
    const int nbComponents = outputAccessor.dataSize;
    const int nbFrames = outputAccessor.count;

    // ensure the output buffer has all the data
    // TODO Check the size when doing cubic interpolation
    if (outputRawData.size() != nbComponents * nbFrames * accessorDataTypeToBytes(outputAccessor.type)) {
        qCWarning(kuesa, "Output buffer doesn't have enough data for the animation");
        return std::make_tuple(0, channelData);
    }

    switch (outputAccessor.type) {
    case Qt3DRender::QAttribute::Float:
        channelData.resize(static_cast<size_t>(outputRawData.size()) / sizeof(float));
        std::memcpy(channelData.data(), outputRawData.constData(), static_cast<size_t>(outputRawData.size()));
        break;
    case Qt3DRender::QAttribute::Byte:
        channelData = normalizeTypedChannelData<qint8>(outputRawData);
        break;
    case Qt3DRender::QAttribute::UnsignedByte:
        channelData = normalizeTypedChannelData<quint8>(outputRawData);
        break;
    case Qt3DRender::QAttribute::Short:
        channelData = normalizeTypedChannelData<qint16>(outputRawData);
        break;
    case Qt3DRender::QAttribute::UnsignedShort:
        channelData = normalizeTypedChannelData<quint16>(outputRawData);
        break;
    default:
        qCWarning(kuesa, "Output buffer data type is not correct");
        return std::make_tuple(0, std::vector<float>());
    }

    return std::make_tuple(nbComponents, channelData);
}

Qt3DAnimation::QChannel AnimationParser::animationChannelDataFromBuffer(const QString &channelName,
                                                                        const AnimationParser::AnimationSampler &sampler,
                                                                        const TreeNode &targetNode) const
{
    auto channel = Qt3DAnimation::QChannel(channelPathToName(channelName));
    const Accessor inputAccessor = m_context->accessor(sampler.inputAccessor);

    if (inputAccessor.type != Qt3DRender::QAttribute::Float) {
        qCWarning(kuesa, "Input accessor have a float component type");
        return channel;
    }

    if (inputAccessor.dataSize != 1) {
        qCWarning(kuesa, "Input accessor data size must be 1");
        return channel;
    }

    const QByteArray inputRawData = rawDataFromAccessor(inputAccessor, m_context);
    const qint32 nKeyframes = inputAccessor.count;
    if (inputRawData.size() != nKeyframes * accessorDataTypeToBytes(inputAccessor.type)) {
        qCWarning(kuesa, "Input buffer doesn't have enough data for the animation");
        return channel;
    }

    std::vector<float> timeStamps(nKeyframes);
    std::memcpy(timeStamps.data(), inputRawData.constData(), inputRawData.size());

    quint8 nbComponents = 0;
    std::vector<float> valueStamps;
    std::tie(nbComponents, valueStamps) = animationChannelDataFromData(sampler);

    // Check nbComponent and type are what is expected for a given channel
    const quint8 expectedComponents = expectedComponentsCountForChannel(channelName);
    if (expectedComponents == 0)
        return channel;

    if (nbComponents != expectedComponents) {
        qCWarning(kuesa) << "Channel components for" << channelName << "expected" << expectedComponents << "but obtained" << nbComponents;
        return channel;
    }

    if (nbComponents == 0 || valueStamps.empty())
        return channel;

    // When dealing with morph targets we have :
    // - morphTargetCount * timeStamps.size() keyframe values
    // - animationChannelDataFromData will set nbComponents to 1 because we are
    // dealing with a scalar weight type
    // - the channel should actually expose one scalar weight for each morph
    // targets
    // This means we should set nbComponent to be morphTargetCount
    const bool isMorphTargetWeightChannel = (channelName == QStringLiteral("weights"));
    if (isMorphTargetWeightChannel) {
        const Mesh morphTargetMesh = m_context->mesh(targetNode.meshIdx);
        if (targetNode.meshIdx < 0 || morphTargetMesh.morphTargetCount == 0) {
            qWarning(kuesa) << "Invalid Mesh for MorphTarget animation";
            return channel;
        }
        nbComponents = morphTargetMesh.morphTargetCount;
    }

    Qt3DAnimation::QKeyFrame::InterpolationType interpolationType = Qt3DAnimation::QKeyFrame::LinearInterpolation;

    std::vector<Qt3DAnimation::QChannelComponent> channelComponents(nbComponents);
    switch (sampler.interpolationMethod) {
    case AnimationParser::Step:
        interpolationType = Qt3DAnimation::QKeyFrame::ConstantInterpolation;
        Q_FALLTHROUGH();
    case AnimationParser::Linear: {
        // Verify we have the same number of keyframes as values
        if (nbComponents * timeStamps.size() != valueStamps.size()) {
            qCWarning(kuesa, "Input and output buffers have different number of key frames");
            return channel;
        }

        for (qint32 keyframeId = 0; keyframeId < nKeyframes; ++keyframeId) {
            for (qint32 componentId = 0; componentId < nbComponents; ++componentId) {
                auto keyframe = Qt3DAnimation::QKeyFrame(QVector2D(timeStamps[keyframeId], valueStamps[nbComponents * keyframeId + componentId]));
                keyframe.setInterpolationType(interpolationType);
                channelComponents[componentId].appendKeyFrame(keyframe);
            }
        }
        break;
    }
    case AnimationParser::CubicSpline: {
        interpolationType = Qt3DAnimation::QKeyFrame::ConstantInterpolation;
        // Verify we have the same number of keyframes as values
        // As we are using cubic spline interpolation, each keyframe has 3 values (p0, lDerivative, rDerivative)
        if (3 * nbComponents * timeStamps.size() != valueStamps.size()) {
            qCWarning(kuesa, "Input and output buffers have different number of key frames");
            return channel;
        }

        for (qint32 keyframeId = 0; keyframeId < nKeyframes; ++keyframeId) {
            const float tCurrent = timeStamps[keyframeId]; // Time in current keyframe
            const float tNext = timeStamps[std::min(nKeyframes - 1, keyframeId + 1)]; // Time in following keyframe
            const float tPrevious = timeStamps[std::max(0, keyframeId - 1)]; // Time in previous keyframe

            const size_t baseIndex = 3 * nbComponents * keyframeId;
            for (int componentId = 0; componentId < nbComponents; ++componentId) {
                const float lhTimeDisplacement = 1.0f / 3.0f * (tCurrent - tPrevious);
                const float rhTimeDisplacement = 1.0f / 3.0f * (tNext - tCurrent);

                const float t_lh = tCurrent - lhTimeDisplacement; // Time of the left handle
                const float t_rh = tCurrent + rhTimeDisplacement; // Time of the following handle

                const size_t a0Index = baseIndex + componentId;
                const float a0 = valueStamps[a0Index]; // In-tangent

                const size_t p0Index = baseIndex + componentId + nbComponents;
                const float p0 = valueStamps[p0Index]; // Value on the keyframe

                const size_t b0Index = baseIndex + componentId + 2 * nbComponents;
                const float b0 = valueStamps[b0Index]; // Out-tangent

                const float lh = p0 - lhTimeDisplacement * a0; // Value of left handle
                const float rh = p0 + rhTimeDisplacement * b0; // Value of right handle

                const Qt3DAnimation::QKeyFrame keyframe({ tCurrent, p0 }, { t_lh, lh }, { t_rh, rh });
                channelComponents[componentId].appendKeyFrame(keyframe);
            }
        }
        break;
    }
    default:
        Q_UNREACHABLE();
    }

    const bool isRotationChannel = (channelName == QStringLiteral("rotation"));
    if (isRotationChannel) {
        channel.appendChannelComponent(*(channelComponents.end() - 1));
        std::for_each(std::begin(channelComponents), std::end(channelComponents) - 1, [&channel](const Qt3DAnimation::QChannelComponent &channelComponent) {
            channel.appendChannelComponent(channelComponent);
        });
    } else {
        std::for_each(std::begin(channelComponents), std::end(channelComponents), [&channel](const Qt3DAnimation::QChannelComponent &channelComponent) {
            channel.appendChannelComponent(channelComponent);
        });
    }

    return channel;
}

bool AnimationParser::checkSamplerJSON(const QJsonObject &samplerObject) const
{
    const QJsonValue inputValue = samplerObject.value(KEY_INPUT);
    const QJsonValue outputValue = samplerObject.value(KEY_OUTPUT);

    if (inputValue.isUndefined()) {
        qCWarning(kuesa, "Missing input buffer in animation sampler");
        return false;
    }

    if (outputValue.isUndefined()) {
        qCWarning(kuesa, "Missing output buffer in animation sampler");
        return false;
    }

    const int inputAccessorIdx = inputValue.toInt(-1);

    if (inputAccessorIdx < 0 || inputAccessorIdx > m_context->accessorCount()) {
        qCWarning(kuesa, "Invalid input accessor index");
        return false;
    }

    const int outputAccessorIdx = outputValue.toInt(-1);

    if (outputAccessorIdx < 0 || outputAccessorIdx > m_context->accessorCount()) {
        qCWarning(kuesa, "Invalid output accessor index");
        return false;
    }

    return true;
}

std::tuple<bool, AnimationParser::AnimationSampler> AnimationParser::animationSamplersFromJson(const QJsonObject &samplerObject) const
{
    AnimationParser::AnimationSampler animationSampler;
    animationSampler.inputAccessor = samplerObject.value(KEY_INPUT).toInt();
    animationSampler.outputAccessor = samplerObject.value(KEY_OUTPUT).toInt();
    animationSampler.interpolationMethod = interpolationMethodFromSemantic(samplerObject.value(KEY_INTERPOLATION).toString(QStringLiteral("LINEAR")));

    if (animationSampler.interpolationMethod == AnimationParser::Unknown) {
        qCWarning(kuesa, "Invalid interpolation method");
        return std::make_tuple(false, animationSampler);
    }

    return std::make_tuple(true, animationSampler);
}

bool AnimationParser::checkChannelJSON(const QJsonObject &channelObject) const
{
    const QJsonValue samplerValue = channelObject.value(KEY_SAMPLER);
    const QJsonValue targetValue = channelObject.value(KEY_TARGET);

    if (samplerValue.isUndefined()) {
        qCWarning(kuesa, "Missing sampler for animation channel");
        return false;
    }

    const int samplerIdx = samplerValue.toInt(-1);
    if (samplerIdx < 0 || samplerIdx > m_context->accessorCount()) {
        qCWarning(kuesa, "Invalid input accessor id for channel");
        return false;
    }

    if (targetValue.isUndefined()) {
        qCWarning(kuesa, "Missing target for animation channel");
        return false;
    }

    const QJsonObject targetObject = targetValue.toObject();
    const QJsonValue pathValue = targetObject.value(KEY_PATH);

    if (pathValue.isUndefined()) {
        qCWarning(kuesa, "Missing path for channel's animation target");
        return false;
    }

    const QString path = pathValue.toString();

    // Verify path is a valid value
    const auto validPathValues = { QStringLiteral("translation"), QStringLiteral("rotation"), QStringLiteral("scale"), QStringLiteral("weights") };
    if (std::find(std::begin(validPathValues), std::end(validPathValues), path) == std::end(validPathValues)) {
        qCWarning(kuesa, "Channel Path value is not valid");
        return false;
    }

    return true;
}

std::tuple<bool, Qt3DAnimation::QChannel>
AnimationParser::channelFromJson(const QJsonObject &channelObject) const
{
    Qt3DAnimation::QChannel channel;

    const QJsonValue samplerValue = channelObject.value(KEY_SAMPLER);
    const QJsonObject targetObject = channelObject.value(KEY_TARGET).toObject();
    const QString path = targetObject.value(KEY_PATH).toString();
    const int targetNodeIdx = targetObject.value(KEY_NODE).toInt(-1);
    const AnimationSampler sampler = m_samplers.at(samplerValue.toInt());
    const TreeNode targetNode = m_context->treeNode(targetNodeIdx);

    channel = animationChannelDataFromBuffer(path, sampler, targetNode);
    channel.setName(channel.name() + QStringLiteral("_") + QString::number(targetNodeIdx));

    if (channel.channelComponentCount() == 0) {
        qCWarning(kuesa, "Channel doesn't have components");
        return std::make_tuple(false, channel);
    }

    return std::make_tuple(true, channel);
}

std::tuple<bool, ChannelMapping>
AnimationParser::mappingFromJson(const QJsonObject &channelObject) const
{
    const QJsonObject targetObject = channelObject.value(KEY_TARGET).toObject();
    const QString path = targetObject.value(KEY_PATH).toString();

    const QJsonValue nodeValue = targetObject.value(KEY_NODE);
    ChannelMapping mapping;

    if (nodeValue.isUndefined()) {
        // Spec doesn't require node, but how can we animate an unknown node?
        qCWarning(kuesa, "Missing node for animation target");
        return std::make_tuple(false, mapping);
    }

    const int &node = nodeValue.toInt(-1);

    if (node < 0 || node > m_context->treeNodeCount()) {
        qCWarning(kuesa, "Invalid node for animation target");
        return std::make_tuple(false, mapping);
    }
    const TreeNode targetNode = m_context->treeNode(node);

    mapping.name = channelPathToName(path) + QStringLiteral("_") + QString::number(node);
    mapping.property = channelPathToProperty(path);
    mapping.targetNodeId = node;
    return std::make_tuple(true, mapping);
}

bool AnimationParser::parse(const QJsonArray &animationsArray, GLTF2Context *context)
{
    m_context = context;

    for (const QJsonValue &animationValue : animationsArray) {

        m_samplers.clear();
        const QJsonObject animationObject = animationValue.toObject();
        const QJsonValue channelsValue = animationObject.value(KEY_CHANNELS);
        const QJsonValue samplersValue = animationObject.value(KEY_SAMPLERS);

        if (channelsValue.isUndefined()) {
            qCWarning(kuesa, "Missing channels array");
            return false;
        }

        if (samplersValue.isUndefined()) {
            qCWarning(kuesa, "Missing samplers array");
            return false;
        }

        const QJsonArray channelsArray = channelsValue.toArray();
        const QJsonArray samplersArray = samplersValue.toArray();

        // Check Sampler Objects have a correct structure
        for (const auto &samplerValue : samplersArray) {
            if (!checkSamplerJSON(samplerValue.toObject())) {
                qCWarning(kuesa, "An animation sampler is incorrect");
                return false;
            }
        }

        // Create Samplers to be used to generate Clip Data
        for (const auto &samplerValue : samplersArray) {
            AnimationSampler sampler;
            bool samplersIsValid = false;
            std::tie(samplersIsValid, sampler) = animationSamplersFromJson(samplerValue.toObject());
            if (!samplersIsValid) {
                qCWarning(kuesa, "An animation sampler is incorrect");
                return false;
            }
            m_samplers.push_back(sampler);
        }

        // Check Channel Objects have a correct structure
        for (const auto &channelValue : channelsArray) {
            if (!checkChannelJSON(channelValue.toObject())) {
                qCWarning(kuesa, "An animation channel is incorrect");
                return false;
            }
        }

        // Animation Clip Data
        Qt3DAnimation::QAnimationClipData clipData;
        for (const auto &channelValue : channelsArray) {
            bool channelIsCorrect = false;
            Qt3DAnimation::QChannel channel;
            std::tie(channelIsCorrect, channel) = channelFromJson(channelValue.toObject());
            if (!channelIsCorrect) {
                qCWarning(kuesa, "An animation channel is incorrect");
                return false;
            }

            clipData.appendChannel(channel);
        }

        Animation animation;
        animation.clipData = clipData;
        animation.name = animationObject.value(KEY_NAME).toString();

        // Channel Mappings
        for (const auto &channelValue : channelsArray) {
            bool channelMappingIsCorrect = false;
            ChannelMapping mapping;
            std::tie(channelMappingIsCorrect, mapping) = mappingFromJson(channelValue.toObject());
            if (!channelMappingIsCorrect) {
                qCWarning(kuesa, "An animation mapping is incorrect");
                return false;
            }
            animation.mappings.push_back(mapping);
        }

        context->addAnimation(animation);
    }

    return animationsArray.size();
}

QT_END_NAMESPACE
