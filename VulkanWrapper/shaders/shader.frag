#version 450

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D ySampler;
layout(binding = 3) uniform sampler2D uSampler;
layout(binding = 4) uniform sampler2D vSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    // 获取 YUV 分量的采样值
    float y = texture(ySampler, fragTexCoord).r;
    float u = texture(uSampler, fragTexCoord).r - 0.5;
    float v = texture(vSampler, fragTexCoord).r - 0.5;

    // 将 YUV 转换为 RGBA
    float r = y + 1.13983 * v;
    float g = y - 0.39465 * u - 0.58060 * v;
    float b = y + 2.03211 * u;

    // outColor = texture(texSampler, fragTexCoord);
    outColor = vec4(r, g, b, 1.0);
}
