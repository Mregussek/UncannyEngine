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
};

struct Material
{
  vec3 diffuse;
};

struct BottomStructureUniformData
{
  uint64_t vertexAddress;
  uint64_t indexAddress;
  uint64_t materialAddress;
};

struct LightData
{
  vec3 position;
};

layout(location = 0) rayPayloadInEXT vec4 payload;

layout(buffer_reference, scalar) buffer Vertices { Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices { ivec3 i[]; };
layout(buffer_reference, scalar) buffer Materials { Material m[]; };
layout(buffer_reference, scalar) buffer MaterialIndices { uint32_t i[]; };
layout(set = 1, binding = 0, scalar) buffer BottomStructureUniformData_ { BottomStructureUniformData d[]; } bottomASData;
layout(set = 1, binding = 1) uniform LightData_ { LightData data; } lightData;

hitAttributeEXT vec3 attribs;

void main()
{
    BottomStructureUniformData asData = bottomASData.d[gl_InstanceCustomIndexEXT];
    Vertices objectVertices = Vertices(asData.vertexAddress);
    Indices objectIndices = Indices(asData.indexAddress);
    Materials objectMaterials = Materials(asData.materialAddress);

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

    // Basic lighting
    vec3 lightVector = normalize(light.position);
    // Lambertian
    float dotNL = max(dot(worldHitNormal, lightVector), 0.0);
    vec3 color = vertex0 * dotNL;

    payload = vec4(color, 0.0);
}
