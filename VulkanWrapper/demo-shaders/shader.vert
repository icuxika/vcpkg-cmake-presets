#version 450

layout (binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inTexCoord;

layout (location = 0) out vec3 vertPosition;
layout (location = 1) out vec4 fragColor;
layout (location = 2) out vec2 fragTexCoord;

void main() {
    if (inPosition.z > 0.0) {
        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition.xy, 0.0, 1.0);
    } else {
        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    }
    vertPosition = inPosition;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}