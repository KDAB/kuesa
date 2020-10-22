/*
    effectslibrary.cpp

    This file is part of Kuesa.

    Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Juan Casafranca <juan.casafranca@kdab.com>

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

#include "effectslibrary_p.h"

#include "metallicroughnesseffect.h"
#include "unliteffect.h"

#include "kuesa_p.h"

QT_BEGIN_NAMESPACE

namespace Kuesa {

UnlitEffect *EffectsLibrary::createUnlitEffectWithKey(EffectProperties::Properties properties)
{
    UnlitEffect *effect = new UnlitEffect;

    effect->setBaseColorMapEnabled(properties & EffectProperties::BaseColorMap);
    effect->setOpaque(!(properties & EffectProperties::Blend));
    effect->setAlphaCutoffEnabled(properties & EffectProperties::Mask);
    effect->setDoubleSided(properties & EffectProperties::DoubleSided);
    effect->setUsingColorAttribute(properties & EffectProperties::VertexColor);
    effect->setUsingNormalAttribute(properties & EffectProperties::VertexNormal);
    effect->setUsingTangentAttribute(properties & EffectProperties::VertexTangent);
    effect->setUsingTexCoordAttribute(properties & EffectProperties::VertexTexCoord);
    effect->setUsingTexCoord1Attribute(properties & EffectProperties::VertexTexCoord1);
    effect->setUsingSkinning(properties & EffectProperties::Skinning);
    effect->setUsingMorphTargets(properties & EffectProperties::MorphTargets);

    return effect;
}

MetallicRoughnessEffect *EffectsLibrary::createMetallicRoughnessEffectWithKey(EffectProperties::Properties properties)
{
    MetallicRoughnessEffect *effect = new MetallicRoughnessEffect;

    effect->setBaseColorMapEnabled(properties & EffectProperties::BaseColorMap);
    effect->setMetalRoughMapEnabled(properties & EffectProperties::MetalRoughnessMap);
    effect->setAmbientOcclusionMapEnabled(properties & EffectProperties::AOMap);
    effect->setNormalMapEnabled(properties & EffectProperties::NormalMap);
    effect->setEmissiveMapEnabled(properties & EffectProperties::EmissiveMap);
    effect->setOpaque(!(properties & EffectProperties::Blend));
    effect->setAlphaCutoffEnabled(properties & EffectProperties::Mask);
    effect->setDoubleSided(properties & EffectProperties::DoubleSided);
    effect->setUsingColorAttribute(properties & EffectProperties::VertexColor);
    effect->setUsingNormalAttribute(properties & EffectProperties::VertexNormal);
    effect->setUsingTangentAttribute(properties & EffectProperties::VertexTangent);
    effect->setUsingTexCoordAttribute(properties & EffectProperties::VertexTexCoord);
    effect->setUsingTexCoord1Attribute(properties & EffectProperties::VertexTexCoord1);
    effect->setUsingSkinning(properties & EffectProperties::Skinning);
    effect->setUsingMorphTargets(properties & EffectProperties::MorphTargets);

    return effect;
}

GLTF2MaterialEffect *EffectsLibrary::createEffectWithKey(EffectProperties::Properties properties)
{
    Q_ASSERT(!(properties & EffectProperties::Custom));

    if (properties & EffectProperties::Unlit) {
        return EffectsLibrary::createUnlitEffectWithKey(properties);
    }

    if (properties & EffectProperties::MetallicRoughness) {
        return EffectsLibrary::createMetallicRoughnessEffectWithKey(properties);
    }

    qCWarning(kuesa, "Trying to create an effect with invalid key");
    return nullptr;
}

EffectsLibrary::EffectsLibrary()
    : m_dummyRootNode(new Qt3DCore::QNode)
{
}

EffectsLibrary::~EffectsLibrary()
{
    delete m_dummyRootNode;
    m_dummyRootNode = nullptr;
}

GLTF2MaterialEffect *EffectsLibrary::getOrCreateCustomEffect(EffectsLibrary::CustomEffectKey customEffectKey,
                                                             Qt3DCore::QNode *effectOwner)
{
    auto customEffectKeyIsEqual = [customEffectKey](const CustomEffectKeyPair &a) {
        return std::get<0>(a) == customEffectKey;
    };
    const auto it = std::find_if(m_customEffects.cbegin(),
                                 m_customEffects.cend(),
                                 customEffectKeyIsEqual);
    if (it != m_customEffects.cend()) {
        auto res = std::get<1>(*it);
        res->setParent(effectOwner);
        return res;
    }

    GLTF2MaterialEffect *effect = qobject_cast<GLTF2MaterialEffect *>(
            customEffectKey.effectClassMetaObject->newInstance(
                    Q_ARG(Qt3DCore::QNode *, effectOwner)));
    Q_ASSERT(effect);
    const EffectProperties::Properties properties = customEffectKey.properties;
    effect->setOpaque(!(properties & EffectProperties::Blend));
    effect->setAlphaCutoffEnabled(properties & EffectProperties::Mask);
    effect->setDoubleSided(properties & EffectProperties::DoubleSided);
    effect->setUsingColorAttribute(properties & EffectProperties::VertexColor);
    effect->setUsingNormalAttribute(properties & EffectProperties::VertexNormal);
    effect->setUsingTangentAttribute(properties & EffectProperties::VertexTangent);
    effect->setUsingTexCoordAttribute(properties & EffectProperties::VertexTexCoord);
    effect->setUsingTexCoord1Attribute(properties & EffectProperties::VertexTexCoord1);
    effect->setUsingSkinning(properties & EffectProperties::Skinning);
    effect->setUsingMorphTargets(properties & EffectProperties::MorphTargets);
    m_customEffects.push_back({ customEffectKey, effect });
    return effect;
}

GLTF2MaterialEffect *EffectsLibrary::getOrCreateEffect(EffectProperties::Properties properties,
                                                       Qt3DCore::QNode *effectOwner)
{
    auto effectIt = m_effects.find(properties);
    if (effectIt == m_effects.end()) {
        auto *effect = createEffectWithKey(properties);
        if (effect) {
            effect->setParent(effectOwner);
            m_effects.insert(properties, effect);
        }
        return effect;
    }

    auto effect = effectIt.value();
    effect->setParent(effectOwner);
    return effect;
}

int EffectsLibrary::count() const
{
    return m_effects.size() + m_customEffects.size();
}

void EffectsLibrary::clear()
{
    m_effects.clear();
    m_customEffects.clear();
}

void EffectsLibrary::reset()
{
    for (auto effect : m_effects)
        effect->setParent(m_dummyRootNode);
    for (auto effectKeyPair : m_customEffects)
        effectKeyPair.second->setParent(m_dummyRootNode);
}

void EffectsLibrary::cleanUp()
{
    {
        auto it = std::remove_if(std::begin(m_effects), std::end(m_effects), [this](GLTF2MaterialEffect *effect) {
            return effect->parentNode() == m_dummyRootNode;
        });
        while (it != m_effects.end())
            it = m_effects.erase(it);
    }
    {
        auto it = std::remove_if(std::begin(m_customEffects), std::end(m_customEffects), [this](const CustomEffectKeyPair &effectKey) {
            return effectKey.second->parentNode() == m_dummyRootNode;
        });
        while (it != m_customEffects.end())
            it = m_customEffects.erase(it);
    }
}

QHash<EffectProperties::Properties, GLTF2MaterialEffect *> EffectsLibrary::effects() const
{
    return m_effects;
}

QVector<std::pair<EffectsLibrary::CustomEffectKey, GLTF2MaterialEffect *>> EffectsLibrary::customEffects() const
{
    return m_customEffects;
}

bool operator==(const EffectsLibrary::CustomEffectKey &a, const EffectsLibrary::CustomEffectKey &b)
{
    return a.effectClassMetaObject == b.effectClassMetaObject && a.properties == b.properties;
}

} // namespace Kuesa

QT_END_NAMESPACE
