/*
    metallicroughnesseffect.h

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

#ifndef FP
#define FP highp
#endif

// Exposure correction
uniform FP float exposure;
// Gamma correction
const FP float gamma = 2.2;

#pragma include light.inc.frag

int mipLevelCount(const in FP sampler2D cube, const in FP vec3 size)
{
    // Width is 3/2 * size.y, so we need to use size.y to deduce number of mips
    int baseSize = int(size.y);
    int nMips = int(log2(float(baseSize > 0 ? baseSize : 1))) + 1;
    return nMips;
}

FP vec2 octohedralProjection(const in FP vec3 dir) {
    FP vec3 d = dir / dot(vec3(1.0), abs(dir));
    FP vec2 rev = abs(d.zx) - vec2(1.0);
    FP vec2 neg = -sign(d).xz * rev.xy;
    FP vec2 uv = d.y < 0.0 ? neg : d.xz;
    return 0.5 * uv + vec2(0.5);
}

FP vec4 sampleTextureAtLevel(const in FP sampler2D t, const in FP vec2 texCoords, int level)
{
    // X,Y coordinates are in the 0 - 1 range

    // Level 0 goes from 0 - 0.66 on X and 0 - 1 on Y
    // Level 1 goes from 0.66 - 1.0 on X and 0 - 0.5 on Y
    // Level 2 goes from 0.66 - 0.83 on X and 0.5 - 0.75 on Y
    // Level 3 goes from 0.66 - 0.745 on X and 0.75 - 0.875 on Y

    FP float levelF = float(level);

    // Bring coordinates into the proper range for level
    FP vec2 offsetCoords = vec2(texCoords.x * 2.0 / 3.0, texCoords.y) / pow(2.0, levelF);

    FP vec2 offset =  vec2(step(1.0, levelF) * 2.0 / 3.0,
                           step(2.0, levelF) * (1.0 - 1.0 / pow(2.0, levelF - 1.0)));
    // Offset coordinates into proper area
    offsetCoords += offset;

    return texture2D(t, offsetCoords);
}

FP float remapRoughness(const in FP float roughness)
{
    // As per page 14 of
    // http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
    // we remap the roughness to give a more perceptually linear response
    // of "bluriness" as a function of the roughness specified by the user.
    // r = roughness^2
    const FP float maxSpecPower = 999999.0;
    const FP float minRoughness = sqrt(2.0 / (maxSpecPower + 2.0));
    return max(roughness * roughness, minRoughness);
}

FP float alphaToMipLevel(FP float alpha)
{
    FP float specPower = 2.0 / (alpha * alpha) - 2.0;

    // We use the mip level calculation from Lys' default power drop, which in
    // turn is a slight modification of that used in Marmoset Toolbag. See
    // https://docs.knaldtech.com/doku.php?id=specular_lys for details.
    // For now we assume a max specular power of 999999 which gives
    // maxGlossiness = 1.
    const FP float k0 = 0.00098;
    const FP float k1 = 0.9921;
    FP float glossiness = (pow(2.0, -10.0 / sqrt(specPower)) - k0) / k1;

    // TODO: Optimize by doing this on CPU and set as
    // uniform int envLight.specularMipLevels say (if present in shader).
    // Lookup the number of mips in the specular envmap
    int mipLevels = mipLevelCount(envLight.specular, envLight.specularSize);

    // Offset of smallest miplevel we should use (corresponds to specular
    // power of 1). I.e. in the 32x32 sized mip.
    const FP float mipOffset = 5.0;

    // The final factor is really 1 - g / g_max but as mentioned above g_max
    // is 1 by definition here so we can avoid the division. If we make the
    // max specular power for the spec map configurable, this will need to
    // be handled properly.
    FP float mipLevel = (float(mipLevels) - 1.0 - mipOffset) * (1.0 - glossiness);
    return mipLevel;
}

FP float normalDistribution(const in FP vec3 n, const in FP vec3 h, const in FP float alpha)
{
    // Blinn-Phong approximation - see
    // http://graphicrants.blogspot.co.uk/2013/08/specular-brdf-reference.html
    FP float specPower = 2.0 / (alpha * alpha) - 2.0;
    return (specPower + 2.0) / (2.0 * 3.14159) * pow(max(dot(n, h), 0.0), specPower);
}

FP vec3 fresnelFactor(const in FP vec3 color, const in FP float cosineFactor)
{
    // Calculate the Fresnel effect value
    FP vec3 f = color;
    FP vec3 F = f + (1.0 - f) * pow(1.0 - cosineFactor, 5.0);
    return clamp(F, f, vec3(1.0));
}

FP float geometricModel(const in FP float lDotN,
                        const in FP float vDotN,
                        const in FP vec3 h)
{
    // Implicit geometric model (equal to denominator in specular model).
    // This currently assumes that there is no attenuation by geometric shadowing or
    // masking according to the microfacet theory.
    return lDotN * vDotN;
}

FP vec3 specularModel(const in FP vec3 F0,
                      const in FP float sDotH,
                      const in FP float sDotN,
                      const in FP float vDotN,
                      const in FP vec3 n,
                      const in FP vec3 h)
{
    // Clamp sDotN and vDotN to small positive value to prevent the
    // denominator in the reflection equation going to infinity. Balance this
    // by using the clamped values in the geometric factor function to
    // avoid ugly seams in the specular lighting.
    FP float sDotNPrime = max(sDotN, 0.001);
    FP float vDotNPrime = max(vDotN, 0.001);

    FP vec3 F = fresnelFactor(F0, sDotH);
    FP float G = geometricModel(sDotNPrime, vDotNPrime, h);

    FP vec3 cSpec = F * G / (4.0 * sDotNPrime * vDotNPrime);
    return clamp(cSpec, vec3(0.0), vec3(1.0));
}

FP vec3 pbrModel(const in int lightIndex,
                 const in FP vec3 wPosition,
                 const in FP vec3 wNormal,
                 const in FP vec3 wView,
                 const in FP vec3 baseColor,
                 const in FP float metalness,
                 const in FP float alpha,
                 const in FP float ambientOcclusion)
{
    // Calculate some useful quantities
    FP vec3 n = wNormal;
    FP vec3 s = vec3(0.0);
    FP vec3 v = wView;
    FP vec3 h = vec3(0.0);

    FP float vDotN = dot(v, n);
    FP float sDotN = 0.0;
    FP float sDotH = 0.0;
    FP float att = 1.0;

    if (lights[lightIndex].type != TYPE_DIRECTIONAL) {
        // Point and Spot lights
        FP vec3 sUnnormalized = vec3(lights[lightIndex].position) - wPosition;
        s = normalize(sUnnormalized);

        // Calculate the attenuation factor
        sDotN = dot(s, n);
        if (sDotN > 0.0) {
            if (lights[lightIndex].constantAttenuation != 0.0
             || lights[lightIndex].linearAttenuation != 0.0
             || lights[lightIndex].quadraticAttenuation != 0.0) {
                FP float dist = length(sUnnormalized);
                att = 1.0 / (lights[lightIndex].constantAttenuation +
                             lights[lightIndex].linearAttenuation * dist +
                             lights[lightIndex].quadraticAttenuation * dist * dist);
            }

            // The light direction is in world space already
            if (lights[lightIndex].type == TYPE_SPOT) {
                // Check if fragment is inside or outside of the spot light cone
                if (degrees(acos(dot(-s, lights[lightIndex].direction))) > lights[lightIndex].cutOffAngle)
                    sDotN = 0.0;
            }
        }
    } else {
        // Directional lights
        // The light direction is in world space already
        s = normalize(-lights[lightIndex].direction);
        sDotN = dot(s, n);
    }

    h = normalize(s + v);
    sDotH = dot(s, h);

    // Calculate diffuse component
    FP vec3 diffuseColor = (1.0 - metalness) * baseColor * lights[lightIndex].color;
    FP vec3 diffuse = diffuseColor * max(sDotN, 0.0) / 3.14159;

    // Calculate specular component
    FP vec3 dielectricColor = vec3(0.04);
    FP vec3 F0 = mix(dielectricColor, baseColor, metalness);
    FP vec3 specularFactor = vec3(0.0);
    if (sDotN > 0.0) {
        specularFactor = specularModel(F0, sDotH, sDotN, vDotN, n, h);
        specularFactor *= normalDistribution(n, h, alpha);
    }
    FP vec3 specularColor = lights[lightIndex].color;
    FP vec3 specular = specularColor * specularFactor;

    // Blend between diffuse and specular to conserver energy
    FP vec3 color = att * lights[lightIndex].intensity * (specular + diffuse * (vec3(1.0) - specular));

    // Reduce by ambient occlusion amount
    color *= ambientOcclusion;

    return color;
}

FP vec3 pbrIblModel(const in FP vec3 wNormal,
                    const in FP vec3 wView,
                    const in FP vec3 baseColor,
                    const in FP float metalness,
                    const in FP float alpha,
                    const in FP float ambientOcclusion)
{
    // Calculate reflection direction of view vector about surface normal
    // vector in world space. This is used in the fragment shader to sample
    // from the environment textures for a light source. This is equivalent
    // to the l vector for punctual light sources. Armed with this, calculate
    // the usual factors needed
    FP vec3 n = wNormal;
    FP vec3 l = reflect(-wView, n);
    FP vec3 v = wView;
    FP vec3 h = normalize(l + v);
    FP float vDotN = dot(v, n);
    FP float lDotN = dot(l, n);
    FP float lDotH = dot(l, h);

    // Calculate diffuse component
    FP vec3 diffuseColor = (1.0 - metalness) * baseColor;
    FP vec2 octohedralTexCoords = octohedralProjection(l);
    FP vec3 diffuse = diffuseColor * sampleTextureAtLevel(envLight.irradiance, octohedralTexCoords, 0).rgb;

    // Calculate specular component
    FP vec3 dielectricColor = vec3(0.04);
    FP vec3 F0 = mix(dielectricColor, baseColor, metalness);
    FP vec3 specularFactor = specularModel(F0, lDotH, lDotN, vDotN, n, h);

    FP float lod = alphaToMipLevel(alpha);
//#define DEBUG_SPECULAR_LODS
#ifdef DEBUG_SPECULAR_LODS
    if (lod > 7.0)
        return vec3(1.0, 0.0, 0.0);
    else if (lod > 6.0)
        return vec3(1.0, 0.333, 0.0);
    else if (lod > 5.0)
        return vec3(1.0, 1.0, 0.0);
    else if (lod > 4.0)
        return vec3(0.666, 1.0, 0.0);
    else if (lod > 3.0)
        return vec3(0.0, 1.0, 0.666);
    else if (lod > 2.0)
        return vec3(0.0, 0.666, 1.0);
    else if (lod > 1.0)
        return vec3(0.0, 0.0, 1.0);
    else if (lod > 0.0)
        return vec3(1.0, 0.0, 1.0);
#endif
    FP vec3 specularSkyColor = sampleTextureAtLevel(envLight.specular, octohedralTexCoords, int(lod)).rgb;
    FP vec3 specular = specularSkyColor * specularFactor;

    // Blend between diffuse and specular to conserve energy
    FP vec3 color = specular + diffuse * (vec3(1.0) - specularFactor);

    // Reduce by ambient occlusion amount
    color *= ambientOcclusion;

    return color;
}

FP vec3 toneMap(const in FP vec3 c)
{
    return c / (c + vec3(1.0));
}

FP vec3 gammaCorrect(const in FP vec3 color)
{
    return pow(color, vec3(1.0 / gamma));
}

FP vec4 kuesa_metalRoughFunction(const in FP vec4 baseColor,
                                 const in FP float metalness,
                                 const in FP float roughness,
                                 const in FP float ambientOcclusion,
                                 const in FP vec3 worldPosition,
                                 const in FP vec3 worldView,
                                 const in FP vec3 worldNormal)
{
    FP vec3 cLinear = vec3(0.0);

    // Remap roughness for a perceptually more linear correspondence
    FP float alpha = remapRoughness(roughness);

    for (int i = 0; i < envLightCount; ++i) {
        cLinear += pbrIblModel(worldNormal,
                               worldView,
                               baseColor.rgb,
                               metalness,
                               alpha,
                               ambientOcclusion);
    }

    for (int i = 0; i < lightCount; ++i) {
        cLinear += pbrModel(i,
                            worldPosition,
                            worldNormal,
                            worldView,
                            baseColor.rgb,
                            metalness,
                            alpha,
                            ambientOcclusion);
    }

    // Apply exposure correction
    cLinear *= pow(2.0, exposure);

    // Apply simple (Reinhard) tonemap transform to get into LDR range [0, 1]
    FP vec3 cToneMapped = toneMap(cLinear);

    // Apply gamma correction prior to display
    FP vec3 cGamma = gammaCorrect(cToneMapped);

    return vec4(cGamma, 1.0);
}
