/*
    effectslibrary.cpp

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

namespace {

Qt3DRender::QEffect *createUnlitEffectWithKey(Kuesa::EffectProperties::Properties properties)
{
    Kuesa::UnlitEffect *effect = new Kuesa::UnlitEffect;

    effect->setBaseColorMapEnabled(properties & Kuesa::EffectProperties::BaseColorMap);
    effect->setOpaque(!(properties & Kuesa::EffectProperties::Blend));
    effect->setAlphaCutoffEnabled(properties & Kuesa::EffectProperties::Mask);
    effect->setDoubleSided(properties & Kuesa::EffectProperties::DoubleSided);
    effect->setUsingColorAttribute(properties & Kuesa::EffectProperties::VertexColor);
    effect->setUseSkinning(properties & Kuesa::EffectProperties::Skinning);

    return effect;
}

Qt3DRender::QEffect *createMetallicRoughnessEffectWithKey(Kuesa::EffectProperties::Properties properties)
{
    Kuesa::MetallicRoughnessEffect *effect = new Kuesa::MetallicRoughnessEffect;

    effect->setBaseColorMapEnabled(properties & Kuesa::EffectProperties::BaseColorMap);
    effect->setMetalRoughMapEnabled(properties & Kuesa::EffectProperties::MetalRoughnessMap);
    effect->setAmbientOcclusionMapEnabled(properties & Kuesa::EffectProperties::AOMap);
    effect->setNormalMapEnabled(properties & Kuesa::EffectProperties::NormalMap);
    effect->setEmissiveMapEnabled(properties & Kuesa::EffectProperties::EmissiveMap);
    effect->setOpaque(!(properties & Kuesa::EffectProperties::Blend));
    effect->setAlphaCutoffEnabled(properties & Kuesa::EffectProperties::Mask);
    effect->setDoubleSided(properties & Kuesa::EffectProperties::DoubleSided);
    effect->setUsingColorAttribute(properties & Kuesa::EffectProperties::VertexColor);
    effect->setUseSkinning(properties & Kuesa::EffectProperties::Skinning);

    return effect;
}

Qt3DRender::QEffect *createEffectWithKey(Kuesa::EffectProperties::Properties properties)
{
    Q_ASSERT(!(properties & Kuesa::EffectProperties::Custom));

    if (properties & Kuesa::EffectProperties::Unlit) {
        return createUnlitEffectWithKey(properties);
    }

    if (properties & Kuesa::EffectProperties::MetallicRoughness) {
        return createMetallicRoughnessEffectWithKey(properties);
    }

    qCWarning(kuesa, "Trying to create an effect with invalid key");
    return nullptr;
}
} // namespace

Kuesa::EffectsLibrary::EffectsLibrary()
{
}

Qt3DRender::QEffect *Kuesa::EffectsLibrary::getOrCreateCustomEffect(Kuesa::EffectsLibrary::CustomEffectKey customEffectKey,
                                                                    Qt3DCore::QNode *effectOwner)
{
    auto customEffectKeyIsEqual = [customEffectKey] (const CustomEffectKeyPair &a) {
        return std::get<0>(a) == customEffectKey;
    };
    const auto it = std::find_if(m_customEffects.cbegin(),
                                 m_customEffects.cend(),
                                 customEffectKeyIsEqual);
    if (it != m_customEffects.cend())
        return std::get<1>(*it);

    Qt3DRender::QEffect *effect = qobject_cast<Qt3DRender::QEffect *>(
                customEffectKey.effectClassMetaObject->newInstance(
                    Q_ARG(Qt3DCore::QNode *, effectOwner)));
    Q_ASSERT(effect);
    const EffectProperties::Properties properties = customEffectKey.properties;
    effect->setProperty("opaque", !(properties & Kuesa::EffectProperties::Blend));
    effect->setProperty("alphaCutoffEnabled", bool(properties & Kuesa::EffectProperties::Mask));
    effect->setProperty("doubleSided", bool(properties & Kuesa::EffectProperties::DoubleSided));
    effect->setProperty("useSkinning", bool(properties & Kuesa::EffectProperties::Skinning));
    m_customEffects.push_back({customEffectKey, effect});
    return effect;
}

Qt3DRender::QEffect *Kuesa::EffectsLibrary::getOrCreateEffect(Kuesa::EffectProperties::Properties properties,
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
    return effectIt.value();
}

int Kuesa::EffectsLibrary::count() const
{
    return m_effects.size() + m_customEffects.size();
}

void Kuesa::EffectsLibrary::clear()
{
    m_effects.clear();
    m_customEffects.clear();
}

QHash<Kuesa::EffectProperties::Properties, Qt3DRender::QEffect *> Kuesa::EffectsLibrary::effects() const
{
    return m_effects;
}

QVector<std::pair<Kuesa::EffectsLibrary::CustomEffectKey, Qt3DRender::QEffect *> > Kuesa::EffectsLibrary::customEffects() const
{
    return m_customEffects;
}

bool Kuesa::operator ==(const Kuesa::EffectsLibrary::CustomEffectKey &a, const Kuesa::EffectsLibrary::CustomEffectKey &b)
{
    return a.effectClassMetaObject == b.effectClassMetaObject &&
           a.properties == b.properties;
}

QT_END_NAMESPACE
