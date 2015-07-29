#version 400

in vec2 fragmentUV;
in vec4 fragmentNormal;
in vec4 fragmentLightDirection;

uniform sampler2D textureDiffuse;
layout (location = 0) out vec4 color;

const float diffus = 2.0;

vec3 computeLi(vec3 texel)
{
    vec3 normal = normalize(fragmentNormal.xyz);
    vec3 lightDirection = normalize(-fragmentLightDirection.xyz);
    vec3 texelLi = texel * (diffus * max(dot(lightDirection, normal), 0.0f));

    return texelLi;
}

void main()
{
    vec3 texelDiffuse = texture2D(textureDiffuse, fragmentUV).rgb;
    texelDiffuse = texelDiffuse * texelDiffuse;

    color = vec4(1.0f, 1.0f, 1.0f, 1.0f);//vec4(sqrt(computeLi(texelDiffuse)), 1.0f);
}
