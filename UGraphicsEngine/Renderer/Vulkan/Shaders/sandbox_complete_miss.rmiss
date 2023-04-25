#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_GOOGLE_include_directive : enable

#include "DataTypes.glsl"

layout(location = 0) rayPayloadInEXT HitPayload hitPayload;

void main()
{
    hitPayload.directColor = vec3(0.1f, 0.2f, 0.1f);
}
