#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexCoord;
layout (location = 2) in vec3 normalTrans;
layout (location = 3) in vec4 positionTrans;
layout (location = 4) in vec3 lightTrans;

layout(binding = 1) uniform sampler2D texSampler;


layout (location = 0) out vec4 outColor;

void main() {
    float shade = dot(normalize(normalTrans), normalize(lightTrans - vec3(positionTrans)));
    outColor = texture(texSampler, fragTexCoord) * shade + 0.2 * vec4(1.0, 1.0, 1.0, 1.0f);
}
