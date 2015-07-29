#version 400

in vec2 fragmentUV;
in vec4 fragmentNormal;
in vec4 fragmentLightDirection;

uniform sampler2D textureDiffuse;
uniform sampler2D textureAlpha;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 threshold;

const float alpha = 0.1;
const float diffus = 2.0;

vec3 computeFog(vec3 finalColor)
{
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = exp2(-0.004f * 0.004f * depth * depth * 1.442695f);
    fogFactor = clamp(fogFactor, 0.0f, 1.0f);

    return mix(vec3(0.27f, 0.44f, 0.57f), finalColor, fogFactor);
}

vec3 computeLi(vec3 texel)
{
    vec3 normal = normalize(fragmentNormal.xyz);
    vec3 lightDirection = normalize(-fragmentLightDirection.xyz);
    vec3 texelLi = texel * (alpha + (diffus * max(dot(lightDirection, normal), 0.0f)));

    return texelLi;
}

void main()
{
    float texelAlpha = texture2D(textureAlpha, fragmentUV).r;
    if(texelAlpha < 0.8f)
        discard;

    vec3 texelDiffuse = texture2D(textureDiffuse, fragmentUV).rgb;
    texelDiffuse = texelDiffuse * texelDiffuse;

    color = vec4(sqrt(computeFog(computeLi(texelDiffuse))), 1.0f);
    threshold = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
