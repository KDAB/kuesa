// Use non-filtered texture fetches
const int samples = 5;
const float pixelOffset[samples] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
const float weight[samples] = float[]( 0.2270270270,
                                       0.1945945946,
                                       0.1216216216,
                                       0.0540540541,
                                       0.0162162162 );

vec4 kuesa_gaussianBlur(const in int pass,
                        const in float width,
                        const in float height,
                        const in vec2 texCoord)
{
    vec4 sum;

    if (pass == 1) {
        float dy = 1.0 / height;
        sum = texture(textureSampler, texCoord) * weight[0];
        for (int i = 1; i < samples; i++) {
            sum += texture(textureSampler, texCoord + vec2(0.0, pixelOffset[i]) * dy) * weight[i];
            sum += texture(textureSampler, texCoord - vec2(0.0, pixelOffset[i]) * dy) * weight[i];
        }
    } else {
        float dx = 1.0 / width;
        sum = texture(textureSampler, texCoord) * weight[0];
        for (int i = 1; i < samples; i++) {
            sum += texture(textureSampler, texCoord + vec2(pixelOffset[i], 0.0) * dx) * weight[i];
            sum += texture(textureSampler, texCoord - vec2(pixelOffset[i], 0.0) * dx) * weight[i];
        }
    }

    return sum;
}
