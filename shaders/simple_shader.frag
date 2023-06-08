#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexCoord;
layout (location = 2) in vec3 normalTrans;
layout (location = 3) in vec4 positionTrans;
layout (location = 4) in vec3 lightTrans;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 lightTransform;
    vec3 viewDir;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;


layout (location = 0) out vec4 outColor;

void main() {
    vec3 H1 = normalize((lightTrans - vec3(positionTrans)) + ubo.viewDir);
    vec3 H2 = normalize(vec3(0.0, 20.0,-20.0) + ubo.viewDir);
    float shade = clamp(dot(normalize(normalTrans), normalize(lightTrans - vec3(positionTrans))), 0.0, 1.0) +
        clamp((dot(normalize(normalTrans), normalize(vec3(0.0, 0.0,-20.0)))), 0.0, 1.0);
    float spec = clamp(pow(dot(normalize(normalTrans), H1), 100), 0.0, 1.0) + clamp(pow(dot(normalize(normalTrans), H2), 20), 0.0, 1.0);
    outColor = texture(texSampler, fragTexCoord) * shade + texture(texSampler, fragTexCoord) * spec + 0.2 * vec4(1.0, 1.0, 1.0, 1.0f);
}
