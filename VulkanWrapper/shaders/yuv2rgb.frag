#version 450
layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

uniform sampler2D YTexture;
uniform sampler2D UTexture;
uniform sampler2D VTexture;

void main() {
    float Y = texture(YTexture, fragTexCoord).r;
    float U = texture(UTexture, fragTexCoord).r - 0.5;
    float V = texture(VTexture, fragTexCoord).r - 0.5;

    float R = Y + 1.13983 * V;
    float G = Y - 0.39465 * U - 0.58060 * V;
    float B = Y + 2.03211 * U;

    outColor = vec4(R, G, B, 1.0);
}
