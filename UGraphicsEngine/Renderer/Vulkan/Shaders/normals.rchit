#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_buffer_reference2 : require

struct Vertex
{
  vec3 position;
};

struct ObjDesc
{
  uint64_t vertexAddress;
  uint64_t indexAddress;
};

layout(location = 0) rayPayloadInEXT vec4 payload;

layout(buffer_reference, scalar) buffer Vertices { Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices { ivec3 i[]; };
layout(set = 1, binding = 0, scalar) buffer ObjDesc_ { ObjDesc i[]; } objDesc;

hitAttributeEXT vec3 attribs;

void main()
{
    const vec3 barycentricCoords = vec3(1.f - attribs.x - attribs.y, attribs.x, attribs.y);
    payload = vec4(barycentricCoords, 0.0);
}
