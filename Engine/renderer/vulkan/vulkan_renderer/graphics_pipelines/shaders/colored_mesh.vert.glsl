
#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UCamera {
    mat4 matrixMVP;
} camera;

void main() {
    gl_Position = camera.matrixMVP * vec4(inPosition, 1.f);
    fragColor = inColor;
}
