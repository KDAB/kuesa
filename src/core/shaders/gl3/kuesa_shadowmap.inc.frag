/*
    kuesa_shadowmap.inc.frag

    This file is part of Kuesa.

    Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jim Albamont <jim.albamont@kdab.com>

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

/*
 * Useful resources:
 * https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
 * https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
 * https://stackoverflow.com/questions/48654578/omnidirectional-lighting-in-opengl-glsl-4-1
*/

#ifdef LAYER_useCubeMapArrays
uniform samplerCubeArrayShadow shadowMapCubeDepthTexture;
#else
uniform samplerCubeShadow shadowMapCubeDepthTexture;
#endif

uniform sampler2DArrayShadow shadowMapDepthTextureArray;

const int NUM_POINTLIGHT_PCF_SAMPLES = 20;

vec3 sampleOffsetDirections[NUM_POINTLIGHT_PCF_SAMPLES] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

//https://stackoverflow.com/questions/10786951/omnidirectional-shadow-mapping-with-depth-cubemap
float vectorToDepthValue(const in vec3 v, const in Light l)
{
    vec3 absVec = abs(v);
    float localZComp = max(absVec.x, max(absVec.y, absVec.z));

    float n = l.nearFarPlanes.x;
    float f = l.nearFarPlanes.y;
    float normZComp = (f+n) / (f-n) - (2*f*n)/(f-n)/localZComp;
    return (normZComp + 1.0) * 0.5;
}

float shadowMapCoverage(const in Light l, const in vec3 worldPosition, const in vec3 wNormal, const in vec3 wView)
{
    vec3 lightToFrag = l.type == TYPE_DIRECTIONAL ? l.direction : worldPosition - l.position;
    vec3 ftlNormalized = normalize(-lightToFrag);
    float cosTheta = clamp(dot(wNormal, ftlNormalized), 0.0, 1.0);
    float bias = l.shadowBias * tan(acos(cosTheta));
    if (l.type == TYPE_POINT) {

        if (l.usePCF)
        {
            float shadowCoverage = 0.0;
            float viewDistance = length(wView);
            float diskRadius = (1.0 + (viewDistance / l.nearFarPlanes.y)) / 50.0;
            for(int i = 0; i < NUM_POINTLIGHT_PCF_SAMPLES; ++i)
            {
                vec3 lightToFragPCF = lightToFrag + sampleOffsetDirections[i] * diskRadius;
                float fragDepth = vectorToDepthValue(lightToFragPCF, l);
#ifdef LAYER_useCubeMapArrays
                shadowCoverage += texture(shadowMapCubeDepthTexture, vec4(lightToFragPCF, l.depthArrayIndex), fragDepth - bias);
#else
                shadowCoverage += texture(shadowMapCubeDepthTexture, vec4(lightToFragPCF, fragDepth - bias));
#endif
            }
            return shadowCoverage / float(NUM_POINTLIGHT_PCF_SAMPLES);
        }
        else
        {
            float fragDepth = vectorToDepthValue(lightToFrag, l);
#ifdef LAYER_useCubeMapArrays
            return texture(shadowMapCubeDepthTexture, vec4(lightToFrag, l.depthArrayIndex), fragDepth - bias);
#else
            return texture(shadowMapCubeDepthTexture, vec4(lightToFrag, fragDepth - bias));
#endif
        }
    }
    else
    {
        // TODO: calculate positionInLightSpace in vertex shader
        vec4 positionInLightSpace = l.lightProjectionMatrix * vec4(worldPosition, 1.0);
        vec3 projLightPos = positionInLightSpace.xyz / positionInLightSpace.w;
        projLightPos = projLightPos * 0.5 +.5;

        // if projected depth > 1, outside range of our shadowmap and should not be in shadow.
        if (projLightPos.z > 1.0)
            return 1.0;

        // check x&y manually as work-around for Qt3D not having setting for GL_TEXTURE_BORDER_COLOR
        if ( projLightPos.x > 1.0 || projLightPos.x < 0|| projLightPos.y > 1.0 || projLightPos.y < 0)
            return 1.0;

        if (l.usePCF)
        {
            vec2 offset = 1.0 / textureSize(shadowMapDepthTextureArray, 0).xy;
            float shadowCoverage = 0.0;
            for(int i = -1; i <=1; i++)
                for(int j= -1; j <=1; j++)
                    shadowCoverage += texture(shadowMapDepthTextureArray, vec4(projLightPos.x + i*offset.x, projLightPos.y + j*offset.y, l.depthArrayIndex, projLightPos.z - bias));
            return shadowCoverage/9;
        }
        else {
            return texture(shadowMapDepthTextureArray, vec4(projLightPos.x, projLightPos.y, l.depthArrayIndex, projLightPos.z - bias));
        }
    }
}
