/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

// Exposure correction
uniform float exposure = 0.0;
// Gamma correction
uniform float gamma = 2.2;

#pragma include light.inc.frag

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
    // uniform int envLight.specularMipLevels say (if present in shader).
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

vec3 fresnelFactor(const in vec3 color, const in float cosineFactor)
{
    // Calculate the Fresnel effect value
    vec3 f = color;
    vec3 F = f + (1.0 - f) * pow(1.0 - cosineFactor, 5.0);
    return clamp(F, f, vec3(1.0));
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

vec3 specularModel(const in vec3 F0,
                   const in float sDotH,
                   const in float sDotN,
                   const in float vDotN,
                   const in vec3 n,
                   const in vec3 h,
                   const in float alpha)
{
    // Clamp sDotN and vDotN to small positive value to prevent the
    // denominator in the reflection equation going to infinity. Balance this
    // by using the clamped values in the geometric factor function to
    // avoid ugly seams in the specular lighting.
    float sDotNPrime = max(sDotN, 0.001);
    float vDotNPrime = max(vDotN, 0.001);

    vec3 F = fresnelFactor(F0, sDotH);
    float G = geometricModel(sDotNPrime, vDotNPrime, h, alpha);

    vec3 cSpec = F * G / (4.0 * sDotNPrime * vDotNPrime);
    return clamp(cSpec, vec3(0.0), vec3(1.0));
}

vec3 pbrModel(const in int lightIndex,
              const in vec3 wPosition,
              const in vec3 wNormal,
              const in vec3 wView,
              const in vec3 baseColor,
              const in float metalness,
              const in float alpha,
              const in float ambientOcclusion)
{
    // Calculate some useful quantities
    vec3 n = wNormal;
    vec3 s = vec3(0.0);
    vec3 v = wView;
    vec3 h = vec3(0.0);

    float vDotN = dot(v, n);
    float sDotN = 0.0;
    float sDotH = 0.0;
    float att = 1.0;

    if (lights[lightIndex].type != TYPE_DIRECTIONAL) {
        // Point and Spot lights
        vec3 sUnnormalized = vec3(lights[lightIndex].position) - wPosition;
        s = normalize(sUnnormalized);

        // Calculate the attenuation factor
        sDotN = dot(s, n);
        if (sDotN > 0.0) {
            if (lights[lightIndex].constantAttenuation != 0.0
             || lights[lightIndex].linearAttenuation != 0.0
             || lights[lightIndex].quadraticAttenuation != 0.0) {
                float dist = length(sUnnormalized);
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
    vec3 diffuseColor = (1.0 - metalness) * baseColor * lights[lightIndex].color;
    vec3 diffuse = diffuseColor * max(sDotN, 0.0) / 3.14159;

    // Calculate specular component
    vec3 dielectricColor = vec3(0.04);
    vec3 F0 = mix(dielectricColor, baseColor, metalness);
    vec3 specularFactor = vec3(0.0);
    if (sDotN > 0.0) {
        specularFactor = specularModel(F0, sDotH, sDotN, vDotN, n, h, alpha);
        specularFactor *= normalDistribution(n, h, alpha);
    }
    vec3 specularColor = lights[lightIndex].color;
    vec3 specular = specularColor * specularFactor;

    // Blend between diffuse and specular to conserver energy
    vec3 color = att * lights[lightIndex].intensity * (specular + diffuse * (vec3(1.0) - specular));

    // Reduce by ambient occlusion amount
    color *= ambientOcclusion;

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
    vec3 l = reflect(-wView, n);
    vec3 v = wView;
    vec3 h = normalize(l + v);
    float vDotN = dot(v, n);
    float lDotN = dot(l, n);
    float lDotH = dot(l, h);

    // Calculate diffuse component
    vec3 diffuseColor = (1.0 - metalness) * baseColor;
    vec3 diffuse = diffuseColor * texture(envLight.irradiance, l).rgb;

    // Calculate specular component
    vec3 dielectricColor = vec3(0.04);
    vec3 F0 = mix(dielectricColor, baseColor, metalness);
    vec3 specularFactor = specularModel(F0, lDotH, lDotN, vDotN, n, h, alpha);

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
    vec3 specularSkyColor = textureLod(envLight.specular, l, lod).rgb;
    vec3 specular = specularSkyColor * specularFactor;

    // Blend between diffuse and specular to conserve energy
    vec3 color = specular + diffuse * (vec3(1.0) - specularFactor);

    // Reduce by ambient occlusion amount
    color *= ambientOcclusion;

    return color;
}

vec3 toneMap(const in vec3 c)
{
    return c / (c + vec3(1.0));
}

vec3 gammaCorrect(const in vec3 color)
{
    return pow(color, vec3(1.0 / gamma));
}

vec4 kuesa_metalRoughFunction(const in vec4 baseColor,
                              const in float metalness,
                              const in float roughness,
                              const in float ambientOcclusion,
                              const in vec3 worldPosition,
                              const in vec3 worldView,
                              const in vec3 worldNormal)
{
    vec3 cLinear = vec3(0.0);

    // Remap roughness for a perceptually more linear correspondence
    float alpha = remapRoughness(roughness);

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
    vec3 cToneMapped = toneMap(cLinear);

    // Apply gamma correction prior to display
    vec3 cGamma = gammaCorrect(cToneMapped);

    return vec4(cGamma, 1.0);
}
