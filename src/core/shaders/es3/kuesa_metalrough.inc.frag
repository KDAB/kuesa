/****************************************************************************
**
** Copyright (C) 2017-2019 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef FP
#define FP highp
#endif

#pragma include light.inc.frag

const FP float M_PI = 3.141592653589793;

uniform sampler2D brdfLUT;

int mipLevelCount(const in FP samplerCube cube)
{
   int baseSize = textureSize(cube, 0).x;
   int nMips = int(log2(float(baseSize > 0 ? baseSize : 1))) + 1;
   return nMips;
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
    int mipLevels = mipLevelCount(envLight.specular);

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
    FP vec3 l = reflect(-wView, n);
    FP vec3 v = wView;
    FP float vDotN = clamp(dot(v, n), 0.0, 1.0);

    // Calculate diffuse and specular (F0) colors
    FP vec3 dielectricColor = vec3(0.04);
    FP vec3 diffuseColor = baseColor * (vec3(1.0) - dielectricColor);
    diffuseColor *= (1.0 - metalness);
    FP vec3 F0 = mix(dielectricColor, baseColor, metalness); // = specularColor

    FP vec2 brdfUV = clamp(vec2(vDotN, 1.0 - sqrt(alpha)), vec2(0.0), vec2(1.0));
    FP vec2 brdf = texture(brdfLUT, brdfUV).rg;

    FP vec3 diffuseLight = texture(envLight.irradiance, l).rgb;
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
    FP vec3 specularLight = textureLod(envLight.specular, l, lod).rgb;

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
