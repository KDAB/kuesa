/*
    kuesa_metalrough.inc.frag

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

#ifndef FP
#define FP highp
#endif

#pragma include light.inc.frag

const FP float M_PI = 3.141592653589793;

uniform sampler2D brdfLUT;

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
    const FP float minRoughness = 0.00141421; // sqrt(2.0 / (maxSpecPower + 2.0))
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
    // See http://graphicrants.blogspot.co.uk/2013/08/specular-brdf-reference.html
    // for a good reference on NDFs and geometric shadowing models.
    // GGX
    FP float alphaSq = alpha * alpha;
    FP float nDotH = dot(n, h);
    FP float factor = nDotH * nDotH * (alphaSq - 1.0) + 1.0;
    return alphaSq / (3.14159 * factor * factor);
}

FP vec3 fresnelFactor(const in FP vec3 F0, const in FP vec3 F90, const in FP float cosineFactor)
{
    // Calculate the Fresnel effect value
    FP vec3 F = F0 + (F90 - F0) * pow(clamp(1.0 - cosineFactor, 0.0, 1.0), 5.0);
    return clamp(F, vec3(0.0), vec3(1.0));
}

FP float geometricModel(const in FP float lDotN,
                        const in FP float vDotN,
                        const in FP vec3 h,
                        const in FP float alpha)
{
    // Smith GGX
    FP float alphaSq = alpha * alpha;
    FP float termSq = alphaSq + (1.0 - alphaSq) * vDotN * vDotN;
    return 2.0 * vDotN / (vDotN + sqrt(termSq));
}

FP vec3 diffuse(const in FP vec3 diffuseColor)
{
    return diffuseColor / M_PI;
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
    FP float att = 1.0;

    if (lights[lightIndex].type != TYPE_DIRECTIONAL) {
        // Point and Spot lights
        FP vec3 sUnnormalized = vec3(lights[lightIndex].position) - wPosition;
        s = normalize(sUnnormalized);

        // Calculate the attenuation factor
        sDotN = dot(s, n);
        if (sDotN > 0.0) {
            FP float dist = length(sUnnormalized);
            FP float d2 = dist * dist;
            if (lights[lightIndex].range > 0.0)
            {
                // recommended attenuation with range based on KHR_lights_punctual extension
                FP float d2OverR2 = d2/(lights[lightIndex].range * lights[lightIndex].range);
                att = max( min( 1.0 - ( d2OverR2 * d2OverR2 ), 1.0 ), 0.0 ) / d2;
            }
            else {
               att = 1.0 / d2;
            }
            att = clamp(att, 0.0, 1.0);
        }
        if (lights[lightIndex].type == TYPE_SPOT) {
            // Calculate angular attenuation of spotlight, between 0 and 1.
            // yields 1 inside innerCone, 0 outside outerConeAngle, and value interpolated
            // between 1 and 0 between innerConeAngle and outerConeAngle
            FP float cd = dot(-lights[lightIndex].direction, s);
            FP float angularAttenuation = clamp(cd * lights[lightIndex].lightAngleScale + lights[lightIndex].lightAngleOffset, 0.0, 1.0);
            angularAttenuation *= angularAttenuation;
            att *= angularAttenuation;
        }
    } else {
        // Directional lights
        // The light direction is in world space already
        s = normalize(-lights[lightIndex].direction);
        sDotN = dot(s, n);
    }

    // This light doesn't contribute anything
    if (sDotN <= 0.0)
        return vec3(0.0);

    h = normalize(s + v);
    FP float vDotH = dot(v, h);

    FP vec3 dielectricColor = vec3(0.04);
    FP vec3 diffuseColor = baseColor * (vec3(1.0) - dielectricColor);
    diffuseColor *= (1.0 - metalness);
    FP vec3 F0 = mix(dielectricColor, baseColor, metalness); // = specularColor

    // Compute reflectance.
    FP float reflectance = max(max(F0.r, F0.g), F0.b);
    FP vec3 F90 = clamp(reflectance * 25.0, 0.0, 1.0) * vec3(1.0);

    // Compute shading terms
    FP vec3 F = fresnelFactor(F0, F90, vDotH);
    FP float G = geometricModel(sDotN, vDotN, h, alpha);
    FP float D = normalDistribution(n, h, alpha);

    // Analytical (punctual) lighting
    FP vec3 diffuseContrib = (vec3(1.0) - F) * diffuse(diffuseColor);
    FP vec3 specularContrib = F * G * D / (4.0 * sDotN * vDotN);

    // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
    FP vec3 color = att * sDotN * lights[lightIndex].intensity * lights[lightIndex].color
                    * (diffuseContrib + specularContrib);

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
    FP vec3 l = -reflect(wView, n);
    FP vec3 v = wView;
    FP float vDotN = clamp(dot(v, n), 0.0, 1.0);

    // Calculate diffuse and specular (F0) colors
    FP vec3 dielectricColor = vec3(0.04);
    FP vec3 diffuseColor = baseColor * (vec3(1.0) - dielectricColor);
    diffuseColor *= (1.0 - metalness);
    FP vec3 F0 = mix(dielectricColor, baseColor, metalness); // = specularColor

    FP vec2 brdfUV = clamp(vec2(vDotN, 1.0 - sqrt(alpha)), vec2(0.0), vec2(1.0));
    FP vec2 brdf = texture2D(brdfLUT, brdfUV).rg;

    FP vec2 octohedralTexCoords = octohedralProjection(l);
    FP vec3 diffuseLight = sampleTextureAtLevel(envLight.irradiance, octohedralTexCoords, 0).rgb;
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
    FP vec3 specularLight = sampleTextureAtLevel(envLight.specular, octohedralTexCoords, int(lod)).rgb;

    FP vec3 diffuse = diffuseLight * diffuseColor;
    FP vec3 specular = specularLight * (F0 * brdf.x + brdf.y);

    return diffuse + specular;
}

FP vec3 kuesa_metalRoughFunction(const in FP vec4 baseColor,
                                 const in FP float metalness,
                                 const in FP float roughness,
                                 const in FP float ambientOcclusion,
                                 const in FP vec4 emissive,
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

    // Apply ambient occlusion and emissive channels
    cLinear *= ambientOcclusion;
    cLinear += emissive.rgb;

    return cLinear;
}
