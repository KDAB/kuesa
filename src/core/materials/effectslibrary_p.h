/*
    effectslibrary_p.h

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

#ifndef KUESA_EFFECTSLIBRARY_P_H
#define KUESA_EFFECTSLIBRARY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Kuesa API.  It exists for the convenience
// of other Kuesa classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <unordered_map>
#include <private/kuesa_global_p.h>
#include <Qt3DCore/QNode>
#include <Kuesa/effectproperties.h>
#include <memory>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QEffect;
};

namespace Kuesa {

class KUESA_PRIVATE_EXPORT EffectsLibrary
{
public:
    EffectsLibrary();

    struct CustomEffectKey
    {
        QMetaObject effectClassMetaObject;
        EffectProperties::Properties properties;
    };
    using CustomEffectKeyPair = std::pair<CustomEffectKey, Qt3DRender::QEffect *>;

    Qt3DRender::QEffect *getOrCreateCustomEffect(CustomEffectKey customEffectKey,
                                                 Qt3DCore::QNode *effectOwner);

    Qt3DRender::QEffect *getOrCreateEffect(EffectProperties::Properties properties,
                                           Qt3DCore::QNode *effectOwner);
    int count() const;
    void clear();

    QHash<EffectProperties::Properties, Qt3DRender::QEffect *> effects() const;
    QVector<CustomEffectKeyPair> customEffects() const;

private:
    QHash<EffectProperties::Properties, Qt3DRender::QEffect *> m_effects;
    QVector<CustomEffectKeyPair> m_customEffects;
};

bool operator ==(const EffectsLibrary::CustomEffectKey &a, const EffectsLibrary::CustomEffectKey &b);

} // namespace Kuesa

QT_END_NAMESPACE

#endif // EFFECTSLIBRARY_H
