#version 450

layout (binding = 1) uniform sampler2D texSampler;

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec4 fragColor;
layout (location = 2) in vec2 fragTexCoord;

layout (location = 0) out vec4 outColor;

void main() {
    if (vertPosition.z > 0.0) {
        vec3 color = mix(fragColor.xyz, vec3(0.5, 0.5, 0.5), 0.7);
        outColor = fragColor;
    } else {
        outColor = texture(texSampler, fragTexCoord);
    }
}