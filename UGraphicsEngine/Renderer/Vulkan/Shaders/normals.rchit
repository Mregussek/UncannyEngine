#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

struct Vertex
{
  vec3 position;
  vec3 normal;
  vec3 color;
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
    ObjDesc objResource = objDesc.i[gl_InstanceCustomIndexEXT];
    Indices objectIndices = Indices(objResource.indexAddress);
    Vertices objectVertices = Vertices(objResource.vertexAddress);

    const ivec3 triangleIndices = objectIndices.i[gl_PrimitiveID];

    Vertex vertex0 = objectVertices.v[triangleIndices.x];
    Vertex vertex1 = objectVertices.v[triangleIndices.y];
    Vertex vertex2 = objectVertices.v[triangleIndices.z];

    const vec3 barycentricCoords = vec3(1.f - attribs.x - attribs.y, attribs.x, attribs.y);

    // Computing the coordinates of the hit position
    const vec3 hitPos = vertex0.position * barycentricCoords.x + vertex1.position * barycentricCoords.y + vertex2.position * barycentricCoords.z;
    // Transforming the position to world space
    const vec3 worldHitPos = vec3(gl_ObjectToWorldEXT * vec4(hitPos, 1.0));

    // Computing the normal at hit position
    const vec3 hitNormal = vertex0.normal * barycentricCoords.x + vertex1.normal * barycentricCoords.y + vertex2.normal * barycentricCoords.z;
    // Transforming the normal to world space
    const vec3 worldHitNormal = normalize(vec3(hitNormal * gl_WorldToObjectEXT));

    payload = vec4(worldHitNormal, 0.0);
}
