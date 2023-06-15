#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;
layout(location = 4) in uint matId;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 lightTransform;
    vec3 viewDir;
} ubo;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 normalTrans;
layout(location = 3) out vec4 positionModelTrans;
layout(location = 4) out vec3 lightTrans;
layout(location = 5) flat out uint matIdOut;

void main() {
    positionModelTrans = ubo.model * vec4(position, 1.0);
    vec4 positionTrans = ubo.proj * ubo.view * positionModelTrans;
    matIdOut = matId;
    gl_Position = positionTrans;

    vec3 light = vec3(0.0, 0.0, -90.0);
//    lightTrans = mat3(ubo.lightTransform) * light;
    lightTrans = light;

    mat3 transformNormal = transpose(inverse(mat3(ubo.model)));
    normalTrans = transformNormal * normal;

    fragTexCoord = texCoord;
}
