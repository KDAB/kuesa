/*
    bufferviewsparser_p.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <private/gltf2utils_p.h>
#include <private/kuesa_utils_p.h>

#include <QJsonValue>
#include <QJsonObject>
#include <QRegularExpression>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QJoint>
#include <Qt3DAnimation/QChannel>
#include <Qt3DAnimation/QChannelMapping>
#include <Qt3DAnimation/QAnimationClipData>
#include <private/gltf2keys_p.h>
#include <private/gltf2context_p.h>
#include <MorphController>
#include <GLTF2MaterialProperties>
#include <cstring>

QT_BEGIN_NAMESPACE

using namespace Kuesa::GLTF2Import;

namespace {

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
    const quint8 elemByteSize = Kuesa::accessorDataTypeToBytes(accessor.type);
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

struct PathInfo {
    int componentCount = 0;
    QString propertyName;
    QString channelBaseName;
    AnimatableMappingsGenerator mappingGenerator;
};

struct AnimatablePropertiesCache {
private:
    QHash<QString, PathInfo> m_registeredAnimatables;

    static AnimatablePropertiesCache &instance()
    {
        static AnimatablePropertiesCache c;
        return c;
    }

public:
    static QHash<QString, PathInfo> &registeredAnimatables()
    {
        return instance().m_registeredAnimatables;
    }
};

quint8 expectedComponentsCountForChannel(const ChannelInfo &channelInfo)
{
    const AnimationTarget &target = channelInfo.target;
    const quint8 componentCount = AnimatablePropertiesCache::registeredAnimatables().value(target.path).componentCount;
    if (componentCount == 0)
        qCWarning(kuesa) << "Unrecognized animation channel" << target.path;
    return componentCount;
}

QVector<Qt3DAnimation::QChannelMapping *> mappingsForTransformTargetNode(const GLTF2Context *ctx,
                                                                         const ChannelMapping &mapping)
{
    const TreeNode targetNode = ctx->treeNode(mapping.target.targetNodeId);
    QVector<Qt3DAnimation::QChannelMapping *> mappings;

    // Map channel to joint
    for (auto &joint : targetNode.joints) {
        if (joint) {
            auto channelMapping = new Qt3DAnimation::QChannelMapping();
            channelMapping->setTarget(joint);
            channelMapping->setChannelName(mapping.name);
            channelMapping->setProperty(mapping.property == QStringLiteral("scale3D") ? QStringLiteral("scale") : mapping.property);
            mappings.push_back(channelMapping);
        }
    }

    if (targetNode.entity != nullptr) {
        auto transformComponent = Kuesa::componentFromEntity<Qt3DCore::QTransform>(targetNode.entity);
        if (!transformComponent) {
            qCWarning(kuesa, "Target node doesn't have a transform component");
            return mappings;
        }
        auto channelMapping = new Qt3DAnimation::QChannelMapping();
        channelMapping->setTarget(transformComponent);
        channelMapping->setChannelName(mapping.name);
        channelMapping->setProperty(mapping.property);
        mappings.push_back(channelMapping);
    }

    return mappings;
}

QVector<Qt3DAnimation::QChannelMapping *> mappingsForMorphTargetWeights(const GLTF2Context *ctx,
                                                                        const ChannelMapping &mapping)
{
    const TreeNode targetNode = ctx->treeNode(mapping.target.targetNodeId);
    QVector<Qt3DAnimation::QChannelMapping *> mappings;

    if (targetNode.entity != nullptr) {
        // The actual entities to animate are those which render the
        // primitive not the Node Entity

        const auto primitiveEntities = ctx->primitiveEntitiesForEntity(targetNode.entity);
        Q_ASSERT(!primitiveEntities.empty());

        for (Qt3DCore::QEntity *primitiveEntity : primitiveEntities) {
            Kuesa::MorphController *morphControllerComponent =
                    Kuesa::componentFromEntity<Kuesa::MorphController>(primitiveEntity);
            if (!morphControllerComponent) {
                qCWarning(kuesa) << "Target node doesn't have a morph "
                                    "controller component to animate";
                continue;
            }
            auto channelMapping = new Qt3DAnimation::QChannelMapping();
            channelMapping->setTarget(morphControllerComponent);
            channelMapping->setChannelName(mapping.name);
            channelMapping->setProperty(mapping.property);
            mappings.push_back(channelMapping);
        }
    }

    return mappings;
}

QVector<Qt3DAnimation::QChannelMapping *> mappingsForGenericTargetNode(const GLTF2Context *ctx,
                                                                       const ChannelMapping &mapping)
{
    Qt3DCore::QNode *target = nullptr;
    switch (mapping.target.type) {
    case AnimationTarget::Material: {
        const Material &mat = ctx->material(mapping.target.targetNodeId);
        // For materials, we animate the material's properties class instance
        target = mat.materialProperties();
        break;
    }
    case AnimationTarget::Camera: {
        const Camera &cam = ctx->camera(mapping.target.targetNodeId);
        target = cam.lens;
    }
    case AnimationTarget::Light: {
        const Light &light = ctx->light(mapping.target.targetNodeId);
        target = light.lightComponent;
        break;
    }
    default:
        break;
    }

    if (!target)
        return {};

    auto channelMapping = new Qt3DAnimation::QChannelMapping();
    channelMapping->setTarget(target);
    channelMapping->setChannelName(mapping.name);
    channelMapping->setProperty(mapping.property);

    return { channelMapping };
}

void registerDefaultAnimatables()
{
    AnimationParser::registerAnimatableProperty(QStringLiteral("translation"), 3, QStringLiteral("translation"), &mappingsForTransformTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("rotation"), 4, QStringLiteral("rotation"), &mappingsForTransformTargetNode, QStringLiteral("Rotation"));
    AnimationParser::registerAnimatableProperty(QStringLiteral("scale"), 3, QStringLiteral("scale3D"), &mappingsForTransformTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("weights"), 1, QStringLiteral("morphWeights"), &mappingsForMorphTargetWeights, QStringLiteral("MorphWeights"));
    AnimationParser::registerAnimatableProperty(QStringLiteral("orthographic/xmag"), 1, QStringLiteral("left"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("orthographic/ymag"), 1, QStringLiteral("top"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("orthographic/zfar"), 1, QStringLiteral("farPlane"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("orthographic/znear"), 1, QStringLiteral("nearPlane"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("perspective/zfar"), 1, QStringLiteral("farPlane"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("perspective/znear"), 1, QStringLiteral("nearPlane"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("perspective/aspectRatio"), 1, QStringLiteral("aspectRatio"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("perspective/yfov"), 1, QStringLiteral("fieldOfView"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("pbrMetallicRoughness/baseColorFactor"), 4, QStringLiteral("baseColorFactor"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("pbrMetallicRoughness/metallicFactor"), 1, QStringLiteral("metallicFactor"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("pbrMetallicRoughness/roughnessFactor"), 1, QStringLiteral("roughnessFactor"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("alphaCutoff"), 1, QStringLiteral("alphaCutoff"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("emissiveFactor"), 3, QStringLiteral("emissiveFactor"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("normalTexture/scale"), 1, QStringLiteral("normalScale"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("occlusionTexture/strength"), 1, QStringLiteral("occlusionStrength"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("intensity"), 1, QStringLiteral("intensity"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("color"), 4, QStringLiteral("color"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("innerConeAngle"), 1, QStringLiteral("innerConeAngle"), &mappingsForGenericTargetNode);
    AnimationParser::registerAnimatableProperty(QStringLiteral("outerConeAngle"), 1, QStringLiteral("outerConeAngle"), &mappingsForGenericTargetNode);
}

} // namespace

// Register default animatable properties
// Can later be extended by custom extensions
Q_CONSTRUCTOR_FUNCTION(registerDefaultAnimatables)

void AnimationParser::registerAnimatableProperty(const QString gltfPath,
                                                 int componentCount,
                                                 const QString &targetPropertyName,
                                                 const AnimatableMappingsGenerator &targetNodeRetriever,
                                                 const QString &channelBaseName)
{
    auto &animatables = AnimatablePropertiesCache::registeredAnimatables();

    animatables.insert(gltfPath, { componentCount, targetPropertyName, (channelBaseName.isEmpty()) ? targetPropertyName : channelBaseName, targetNodeRetriever });
}

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

Qt3DAnimation::QChannel AnimationParser::animationChannelDataFromBuffer(const ChannelInfo &channelInfo,
                                                                        const AnimationParser::AnimationSampler &sampler) const
{
    const AnimationTarget &target = channelInfo.target;
    const QString channelPath = channelInfo.target.path;
    const QString channelName = AnimatablePropertiesCache::registeredAnimatables().value(channelPath).channelBaseName + QStringLiteral("_") + QString::number(target.targetNodeId);
    auto channel = Qt3DAnimation::QChannel(channelName);
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
    const quint8 expectedComponents = expectedComponentsCountForChannel(channelInfo);
    if (expectedComponents == 0)
        return channel;

    if (nbComponents != expectedComponents) {
        qCWarning(kuesa) << "Channel components for" << channelPath << "expected" << expectedComponents << "but obtained" << nbComponents;
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
    const bool isMorphTargetWeightChannel = (channelPath == QStringLiteral("weights"));
    if (isMorphTargetWeightChannel) {
        if (target.type != AnimationTarget::Node) {
            qWarning(kuesa) << "Invalid Target Type for MorphTarget animation";
            return channel;
        }
        const TreeNode targetNode = m_context->treeNode(target.targetNodeId);
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

    const bool isRotationChannel = (channelPath == QStringLiteral("rotation"));
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

bool AnimationParser::checkChannelInfo(const ChannelInfo &channelInfo) const
{
    const int samplerIdx = channelInfo.sampler;
    if (samplerIdx < 0 || samplerIdx > m_context->accessorCount()) {
        qCWarning(kuesa, "Invalid input accessor id for channel");
        return false;
    }

    const AnimationTarget &target = channelInfo.target;
    if (target.type == AnimationTarget::UnknownType) {
        qCWarning(kuesa, "Missing target for animation channel");
        return false;
    }

    if (target.path.isEmpty()) {
        qCWarning(kuesa, "Missing path for channel's animation target");
        return false;
    }

    // Verify path is a valid value
    if (!AnimatablePropertiesCache::registeredAnimatables().contains(target.path)) {
        qCWarning(kuesa, "Channel Path value is not valid");
        return false;
    }

    return true;
}

ChannelInfo AnimationParser::channelInfoFromJson(const QJsonObject &channelObject) const
{
    const QJsonValue samplerValue = channelObject.value(KEY_SAMPLER);
    const QJsonObject targetObject = channelObject.value(KEY_TARGET).toObject();
    const QString path = targetObject.value(KEY_PATH).toString();
    const qint32 targetNodeIdx = targetObject.value(KEY_NODE).toInt(-1);
    const qint32 sampleIdx = samplerValue.toInt(-1);

    const AnimationTarget target = { AnimationTarget::Node, targetNodeIdx, path };
    return { sampleIdx, target };
}

std::tuple<bool, Qt3DAnimation::QChannel>
AnimationParser::channelFromChannelInfo(const ChannelInfo &channelInfo) const
{
    Qt3DAnimation::QChannel channel;
    const AnimationSampler sampler = m_samplers.at(channelInfo.sampler);

    channel = animationChannelDataFromBuffer(channelInfo, sampler);

    if (channel.channelComponentCount() == 0) {
        qCWarning(kuesa, "Channel doesn't have components");
        return std::make_tuple(false, channel);
    }

    return std::make_tuple(true, channel);
}

std::tuple<bool, ChannelMapping>
AnimationParser::mappingForChannel(const ChannelInfo &channelInfo, const QString &channelName) const
{
    ChannelMapping mapping;
    const AnimationTarget &target = channelInfo.target;

    switch (target.type) {
    case AnimationTarget::Node: {
        if (target.targetNodeId < 0 || target.targetNodeId > m_context->treeNodeCount()) {
            qCWarning(kuesa, "Invalid node for animation target");
            return std::make_tuple(false, mapping);
        }
        break;
    }
    case AnimationTarget::Camera: {
        if (target.targetNodeId < 0 || target.targetNodeId > m_context->cameraCount()) {
            qCWarning(kuesa, "Invalid camera for animation target");
            return std::make_tuple(false, mapping);
        }
        break;
    }
    case AnimationTarget::Light: {
        if (target.targetNodeId < 0 || target.targetNodeId > m_context->lightCount()) {
            qCWarning(kuesa, "Invalid light for animation target");
            return std::make_tuple(false, mapping);
        }
        break;
    }
    case AnimationTarget::Material: {
        if (target.targetNodeId < 0 || target.targetNodeId > m_context->materialsCount()) {
            qCWarning(kuesa, "Invalid material for animation target");
            return std::make_tuple(false, mapping);
        }
        break;
    }
    default:
        Q_UNREACHABLE();
        break;
    }

    const PathInfo info = AnimatablePropertiesCache::registeredAnimatables().value(target.path);
    mapping.name = channelName;
    mapping.generator = info.mappingGenerator;
    mapping.property = info.propertyName;
    mapping.target = target;
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

        QVector<ChannelInfo> channelsInfo;


        // Parse EXT_property_animation channels
        const QJsonObject extensionsObj = animationObject.value(KEY_EXTENSIONS).toObject();
        const QJsonObject extPropertyAnimationObj = extensionsObj.value(KEY_EXT_PROPERTY_ANIMATION_EXTENSION).toObject();
        if (!extPropertyAnimationObj.empty()) {
            ExtPropertyAnimationHandler propertyAnimationParser;
            if (!propertyAnimationParser.parse(extPropertyAnimationObj)) {
                qCWarning(kuesa, "An animation channel is incorrect");
                return false;
            }
            channelsInfo += propertyAnimationParser.channelsInfo();
        }

        // Parse regular Channels
        for (const QJsonValue &channelValue : channelsArray) {
            const ChannelInfo channelInfo = channelInfoFromJson(channelValue.toObject());
            // If the regular channel uses the same sampler as one of the extension channels,
            // we will ignore it
            const auto it = std::find_if(channelsInfo.cbegin(), channelsInfo.cend(), [&channelInfo] (const ChannelInfo &a) {
                return a.sampler == channelInfo.sampler;
            });
            if (it == channelsInfo.cend())
                channelsInfo.push_back(channelInfo);
        }

        // Check Channel Objects have a correct structure
        for (const ChannelInfo &channelInfo : channelsInfo) {
            if (!checkChannelInfo(channelInfo)) {
                qCWarning(kuesa, "An animation channel is incorrect");
                return false;
            }
        }

        Animation animation;
        animation.name = animationObject.value(KEY_NAME).toString();
        // Animation Clip Data
        // Generate channels and mappings based on the gathered ChannelInfo object
        Qt3DAnimation::QAnimationClipData clipData;
        for (const ChannelInfo &channelInfo : channelsInfo) {
            // Create Channel
            bool channelIsCorrect = false;
            Qt3DAnimation::QChannel channel;
            std::tie(channelIsCorrect, channel) = channelFromChannelInfo(channelInfo);
            if (!channelIsCorrect) {
                qCWarning(kuesa, "An animation channel is incorrect");
                return false;
            }
            clipData.appendChannel(channel);

            // Create Channel Mapping
            bool channelMappingIsCorrect = false;
            ChannelMapping mapping;
            std::tie(channelMappingIsCorrect, mapping) = mappingForChannel(channelInfo, channel.name());
            if (!channelMappingIsCorrect) {
                qCWarning(kuesa, "An animation mapping is incorrect");
                return false;
            }
            animation.mappings.push_back(mapping);
        }
        animation.clipData = clipData;
        context->addAnimation(animation);
    }

    return animationsArray.size();
}

namespace {

AnimationTarget::TargetType targetTypeFromName(const QString &name)
{
    if (name == QLatin1String("nodes"))
        return AnimationTarget::Node;
    if (name == QLatin1String("materials"))
        return AnimationTarget::Material;
    if (name == QLatin1String("extensions/KHR_lights/lights"))
        return AnimationTarget::Light;
    if (name == QLatin1String("cameras"))
        return AnimationTarget::Camera;
    return AnimationTarget::UnknownType;
}

} // namespace

bool ExtPropertyAnimationHandler::parse(const QJsonObject &extensionObject)
{
    static const QRegularExpression targetPathRegExp = QRegularExpression(QStringLiteral("^\\/((?:(?:extensions\\/)(?:\\w+\\/\\w+))|(?:\\w+))\\/(\\d+)\\/(.*)$"));
    const QJsonArray channelsArray = extensionObject.value(KEY_CHANNELS).toArray();
    for (const QJsonValue &channelValue : channelsArray) {
        const QJsonObject channel = channelValue.toObject();
        const qint32 samplerIdx = channel.value(KEY_SAMPLER).toInt(-1);
        const QString targetPath = channel.value(KEY_TARGET).toString();

        // Extract type, target idx and property from targetPath
        QRegularExpressionMatch match = targetPathRegExp.match(targetPath);
        if (match.hasMatch()) {
            const AnimationTarget target = { targetTypeFromName(match.captured(1)),
                                             match.captured(2).toInt(),
                                             match.captured(3) };
            const ChannelInfo channelInfo = { samplerIdx, target };
            m_channelsInfo.push_back(channelInfo);
        } else {
            qCWarning(kuesa) << "Failed to parse EXT_property_animation path";
            return false;
        }
    }
    return true;
}

QVector<ChannelInfo> ExtPropertyAnimationHandler::channelsInfo() const
{
    return m_channelsInfo;
}

QT_END_NAMESPACE
