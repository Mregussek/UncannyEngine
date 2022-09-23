
#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UObject {
    mat4 matrixMVP;
    mat4 matrixModel;
    mat4 matrixView;
    mat4 matrixProjection;
    mat4 matrixMeshLocalTransform;
    mat4 matrixMeshWorldTransform;
} uniformobject;

void main() {
    gl_Position = uniformobject.matrixMeshWorldTransform * vec4(inPosition, 1.f);
    fragColor = inColor;
}
