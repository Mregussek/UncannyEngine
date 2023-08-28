#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_GOOGLE_include_directive : enable

#include "../ScatterMaterials.glsl"

layout(location = 0) rayPayloadInEXT HitPayload hitPayload;
layout(location = 1) rayPayloadEXT bool IsInShadow;

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;

layout(buffer_reference, scalar) buffer Vertices { Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices { ivec3 i[]; };
layout(buffer_reference, scalar) buffer Materials { Material m[]; };
layout(buffer_reference, scalar) buffer MaterialIndices { uint i[]; };
layout(set = 1, binding = 0, scalar) buffer BottomStructureUniformData_ { BottomStructureUniformData d[]; } bottomASData;

hitAttributeEXT vec3 attribs;

vec3 Mix(vec3 v0, vec3 v1, vec3 v2, vec3 barycentricCoords)
{
    return v0 * barycentricCoords.x + v1 * barycentricCoords.y + v2 * barycentricCoords.z;
}

void main()
{
    BottomStructureUniformData asData = bottomASData.d[gl_InstanceCustomIndexEXT];
    Vertices objectVertices = Vertices(asData.vertexAddress);
    Indices objectIndices = Indices(asData.indexAddress);
    Materials objectMaterials = Materials(asData.materialAddress);
    MaterialIndices objectMaterialIndices = MaterialIndices(asData.materialIndicesAddress);

    const ivec3 triangleIndices = objectIndices.i[gl_PrimitiveID];
    Vertex vertex0 = objectVertices.v[triangleIndices.x];
    Vertex vertex1 = objectVertices.v[triangleIndices.y];
    Vertex vertex2 = objectVertices.v[triangleIndices.z];

    const uint triangleMaterialIndex = objectMaterialIndices.i[gl_PrimitiveID];
    Material triangleMaterial = objectMaterials.m[triangleMaterialIndex];

    const vec3 barycentricCoords = vec3(1.f - attribs.x - attribs.y, attribs.x, attribs.y);

    // Computing the normal at hit position
    const vec3 hitNormal = Mix(vertex0.normal, vertex1.normal, vertex2.normal, barycentricCoords);
    // Transforming the normal to object space
    const vec3 worldHitNormal = normalize(vec3(hitNormal * gl_WorldToObjectEXT));

    if (triangleMaterial.illuminationModel == 5) // metallic
    {
        EvaluateMetallic(hitPayload, triangleMaterial, gl_WorldRayOriginEXT, gl_WorldRayDirectionEXT, gl_HitTEXT,
                         worldHitNormal, false, 0.2);
    }
    else if (triangleMaterial.illuminationModel == 7) // dielectric
    {
        EvaluateDielectric(hitPayload, triangleMaterial, gl_WorldRayOriginEXT, gl_WorldRayDirectionEXT, gl_HitTEXT,
                           worldHitNormal);
    }
    else if (triangleMaterial.illuminationModel == 4) // emissive material
    {
        EvaluateLightSource(hitPayload, triangleMaterial, gl_WorldRayOriginEXT, gl_WorldRayDirectionEXT, gl_HitTEXT,
                            worldHitNormal);
    }
    else // lambertian
    {
        EvaluateLambertian(hitPayload, triangleMaterial, gl_WorldRayOriginEXT, gl_WorldRayDirectionEXT, gl_HitTEXT,
                           worldHitNormal);
    }
}
