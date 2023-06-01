#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;


layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 lightTransform;
} ubo;

layout(location = 0) out vec3 fragColor;

void main() {
    vec4 light = vec4(0.0, 100.0, 100.0, 0.0);
    vec4 positionTrans = ubo.proj * ubo.view * ubo.model * vec4(position, 1.0);

    mat4 transformNormal = transpose(inverse(ubo.model));
    vec4 normalTrans = transformNormal * vec4(normal, 0.0);
    vec4 lightTrans = ubo.lightTransform * light;
    gl_Position = positionTrans;

    vec4 normalizedNormalTrans = normalTrans / sqrt(normalTrans.x * normalTrans.x + normalTrans.y * normalTrans.y + normalTrans.z * normalTrans.z);
    
    float shade = dot(normalizedNormalTrans, normalize(lightTrans - ubo.model * vec4(position, 1.0)));
    fragColor = vec3((shade * 1.0) + 0.0, (shade * 1.0) + 0.0, (shade * 1.0) + 0.0);
}
