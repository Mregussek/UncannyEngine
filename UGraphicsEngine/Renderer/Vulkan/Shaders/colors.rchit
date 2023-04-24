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
layout(set = 1, binding = 1) uniform Light_ { vec3 position; } light;

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
    const vec3 normal = normalize(vertex0.normal * barycentricCoords.x + vertex1.normal * barycentricCoords.y + vertex2.normal * barycentricCoords.z);

    // Basic lighting
    vec3 lightVector = normalize(light.position.xyz);
    float dot_product = dot(lightVector, normal);
    vec3 color = vertex0.color.rgb * dot_product;

    payload = vec4(color.rgb, 0.0);
}
