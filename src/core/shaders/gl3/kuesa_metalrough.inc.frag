/*
    kuesa_metalrough.inc.frag

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

#pragma include light.inc.frag
#pragma include kuesa_shadowmap.inc.frag

const float M_PI = 3.141592653589793;

uniform sampler2D brdfLUT;

int mipLevelCount(const in samplerCube cube)
{
   int baseSize = textureSize(cube, 0).x;
   int nMips = int(log2(float(baseSize > 0 ? baseSize : 1))) + 1;
   return nMips;
}

float remapRoughness(const in float roughness)
{
    // As per page 14 of
    // http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
    // we remap the roughness to give a more perceptually linear response
    // of "bluriness" as a function of the roughness specified by the user.
    // r = roughness^2
    const float maxSpecPower = 999999.0;
    const float minRoughness = sqrt(2.0 / (maxSpecPower + 2));
    return max(roughness * roughness, minRoughness);
}

float alphaToMipLevel(float alpha)
{
    float specPower = 2.0 / (alpha * alpha) - 2.0;

    // We use the mip level calculation from Lys' default power drop, which in
    // turn is a slight modification of that used in Marmoset Toolbag. See
    // https://docs.knaldtech.com/doku.php?id=specular_lys for details.
    // For now we assume a max specular power of 999999 which gives
    // maxGlossiness = 1.
    const float k0 = 0.00098;
    const float k1 = 0.9921;
    float glossiness = (pow(2.0, -10.0 / sqrt(specPower)) - k0) / k1;

    // TODO: Optimize by doing this on CPU and set as
    // uniform int envLightSpecularMipLevels say (if present in shader).
    // Lookup the number of mips in the specular envmap
    int mipLevels = mipLevelCount(envLight.specular);

    // Offset of smallest miplevel we should use (corresponds to specular
    // power of 1). I.e. in the 32x32 sized mip.
    const float mipOffset = 5.0;

    // The final factor is really 1 - g / g_max but as mentioned above g_max
    // is 1 by definition here so we can avoid the division. If we make the
    // max specular power for the spec map configurable, this will need to
    // be handled properly.
    float mipLevel = (mipLevels - 1.0 - mipOffset) * (1.0 - glossiness);
    return mipLevel;
}

float normalDistribution(const in vec3 n, const in vec3 h, const in float alpha)
{
    // See http://graphicrants.blogspot.co.uk/2013/08/specular-brdf-reference.html
    // for a good reference on NDFs and geometric shadowing models.
    // GGX
    float alphaSq = alpha * alpha;
    float nDotH = dot(n, h);
    float factor = nDotH * nDotH * (alphaSq - 1.0) + 1.0;
    return alphaSq / (3.14159 * factor * factor);
}

vec3 fresnelFactor(const in vec3 F0, const in vec3 F90, const in float cosineFactor)
{
    // Calculate the Fresnel effect value
    vec3 F = F0 + (F90 - F0) * pow(clamp(1.0 - cosineFactor, 0.0, 1.0), 5.0);
    return clamp(F, vec3(0.0), vec3(1.0));
}

float geometricModel(const in float lDotN,
                     const in float vDotN,
                     const in vec3 h,
                     const in float alpha)
{
    // Smith GGX
    float alphaSq = alpha * alpha;
    float termSq = alphaSq + (1.0 - alphaSq) * vDotN * vDotN;
    return 2.0 * vDotN / (vDotN + sqrt(termSq));
}

vec3 diffuse(const in vec3 diffuseColor)
{
    return diffuseColor / M_PI;
}

vec3 pbrModel(const in int lightIndex,
              const in vec3 wPosition,
              const in vec3 wNormal,
              const in vec3 wView,
              const in vec3 baseColor,
              const in float metalness,
              const in float alpha,
              const in float ambientOcclusion,
              const in bool receivesShadows)
{
    // Calculate some useful quantities
    vec3 n = wNormal;
    vec3 s = vec3(0.0);
    vec3 v = wView;
    vec3 h = vec3(0.0);

    float vDotN = dot(v, n);
    float sDotN = 0.0;
    float att = 1.0;

    if (lights[lightIndex].type != TYPE_DIRECTIONAL) {
        // Point and Spot lights
        vec3 sUnnormalized = vec3(lights[lightIndex].position) - wPosition;
        s = normalize(sUnnormalized);

        // Calculate the attenuation factor
        sDotN = dot(s, n);
        if (sDotN > 0.0) {
            float dist = length(sUnnormalized);
            float d2 = dist * dist;
            if (lights[lightIndex].range > 0.0)
            {
                // recommended attenuation with range based on KHR_lights_punctual extension
                float d2OverR2 = d2/(lights[lightIndex].range * lights[lightIndex].range);
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
            float cd = dot(-lights[lightIndex].direction, s);
            float angularAttenuation = clamp(cd * lights[lightIndex].lightAngleScale + lights[lightIndex].lightAngleOffset, 0.0, 1.0);
            angularAttenuation *= angularAttenuation;
            att *= angularAttenuation;
        }
    }
    else {
        // Directional lights
        // The light direction is in world space already
        s = normalize(-lights[lightIndex].direction);
        sDotN = dot(s, n);
    }

    // This light doesn't contribute anything
    if (sDotN <= 0.0)
        return vec3(0.0);

    h = normalize(s + v);
    float vDotH = dot(v, h);

    vec3 dielectricColor = vec3(0.04);
    vec3 diffuseColor = baseColor * (vec3(1.0) - dielectricColor);
    diffuseColor *= (1.0 - metalness);
    vec3 F0 = mix(dielectricColor, baseColor, metalness); // = specularColor

    // Compute reflectance.
    float reflectance = max(max(F0.r, F0.g), F0.b);
    vec3 F90 = clamp(reflectance * 25.0, 0.0, 1.0) * vec3(1.0);

    // Compute shading terms
    vec3 F = fresnelFactor(F0, F90, vDotH);
    float G = geometricModel(sDotN, vDotN, h, alpha);
    float D = normalDistribution(n, h, alpha);

    // Analytical (punctual) lighting
    vec3 diffuseContrib = (vec3(1.0) - F) * diffuse(diffuseColor);
    vec3 specularContrib = F * G * D / (4.0 * sDotN * vDotN);

    // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
    vec3 color = att * sDotN * lights[lightIndex].intensity * lights[lightIndex].color
                 * (diffuseContrib + specularContrib);

    float shadowCoverage = light.castsShadows && receivesShadows ? shadowMapCoverage(light, wPosition, wNormal, wView) : 1.0;
    color *= shadowCoverage;

    return color;
}

vec3 pbrIblModel(const in vec3 wNormal,
                 const in vec3 wView,
                 const in vec3 baseColor,
                 const in float metalness,
                 const in float alpha,
                 const in float ambientOcclusion)
{
    // Calculate reflection direction of view vector about surface normal
    // vector in world space. This is used in the fragment shader to sample
    // from the environment textures for a light source. This is equivalent
    // to the l vector for punctual light sources. Armed with this, calculate
    // the usual factors needed
    vec3 n = wNormal;
    vec3 l = -reflect(wView, n);
    vec3 v = wView;
    float vDotN = clamp(dot(v, n), 0.0, 1.0);

    // Calculate diffuse and specular (F0) colors
    vec3 dielectricColor = vec3(0.04);
    vec3 diffuseColor = baseColor * (vec3(1.0) - dielectricColor);
    diffuseColor *= (1.0 - metalness);
    vec3 F0 = mix(dielectricColor, baseColor, metalness); // = specularColor

    vec2 brdfUV = clamp(vec2(vDotN, 1.0 - sqrt(alpha)), vec2(0.0), vec2(1.0));
    vec2 brdf = texture(brdfLUT, brdfUV).rg;

    vec3 diffuseLight = texture(envLight.irradiance, l).rgb;
    float lod = alphaToMipLevel(alpha);
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
    vec3 specularLight = textureLod(envLight.specular, l, lod).rgb;

    vec3 diffuse = diffuseLight * diffuseColor;
    vec3 specular = specularLight * (F0 * brdf.x + brdf.y);

    return diffuse + specular;
}

vec3 kuesa_metalRoughFunction(const in vec4 baseColor,
                              const in float metalness,
                              const in float roughness,
                              const in float ambientOcclusion,
                              const in vec4 emissive,
                              const in vec3 worldPosition,
                              const in vec3 worldView,
                              const in vec3 worldNormal,
                              const in bool receivesShadows)
{
    vec3 cLinear = vec3(0.0);

    // Remap roughness for a perceptually more linear correspondence
    float alpha = remapRoughness(roughness);

    // Add up the contributions from image based lighting
    for (int i = 0; i < envLightCount; ++i) {
        cLinear += pbrIblModel(worldNormal,
                               worldView,
                               baseColor.rgb,
                               metalness,
                               alpha,
                               ambientOcclusion);
    }

    // Add up the contributions from punctual lights
    for (int i = 0; i < lightCount; ++i) {
        cLinear += pbrModel(i,
                            worldPosition,
                            worldNormal,
                            worldView,
                            baseColor.rgb,
                            metalness,
                            alpha,
                            ambientOcclusion,
                            receivesShadows);
    }

    // Apply ambient occlusion and emissive channels
    cLinear *= ambientOcclusion;
    cLinear += emissive.rgb;

    return cLinear;
}
