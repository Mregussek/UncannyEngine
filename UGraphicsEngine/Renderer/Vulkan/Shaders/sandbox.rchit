#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_GOOGLE_include_directive : enable

#include "DataTypes.glsl"

layout(location = 0) rayPayloadInEXT HitPayload hitPayload;
layout(location = 1) rayPayloadEXT bool IsInShadow;

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;

layout(buffer_reference, scalar) buffer Vertices { Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices { ivec3 i[]; };
layout(buffer_reference, scalar) buffer Materials { Material m[]; };
layout(buffer_reference, scalar) buffer MaterialIndices { uint i[]; };
layout(set = 1, binding = 0, scalar) buffer BottomStructureUniformData_ { BottomStructureUniformData d[]; } bottomASData;
layout(set = 1, binding = 1) uniform LightData_ { LightData data; } lightData;

hitAttributeEXT vec3 attribs;

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

    // Computing the coordinates of the hit position
    const vec3 hitPos = vertex0.position * barycentricCoords.x + vertex1.position * barycentricCoords.y + vertex2.position * barycentricCoords.z;
    // Transforming the position to world space
    const vec3 worldHitPos = vec3(gl_ObjectToWorldEXT * vec4(hitPos, 1.0));

    // Computing the normal at hit position
    const vec3 hitNormal = vertex0.normal * barycentricCoords.x + vertex1.normal * barycentricCoords.y + vertex2.normal * barycentricCoords.z;
    // Transforming the normal to world space
    const vec3 worldHitNormal = normalize(vec3(hitNormal * gl_WorldToObjectEXT));

    // Basic lighting
    vec3 lightVector = normalize(lightData.data.position);
    float dotNL = max(dot(lightVector, worldHitNormal), 0.2);
    vec3 surfaceColor = triangleMaterial.diffuse * dotNL;

    // Shadow casting
    vec3 shadowRayOrigin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    vec3 shadowRayDirection = lightVector;
    float shadowRayDistance = length(lightData.data.position - shadowRayOrigin) - 0.001f;
    uint rayFlags = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
    IsInShadow = true;
    traceRayEXT(topLevelAS,             // accelerationStructureEXT topLevel
                rayFlags,               // rayFlags
                0xff,                   // cullMask
                1,                      // sbtRecordOffset
                0,                      // sbtRecordStride
                1,                      // missIndex
                shadowRayOrigin,        // origin
                0.001f,                 // Tmin
                shadowRayDirection,     // direction
                shadowRayDistance,      // Tmax
                1                       // payload
    );
    if (IsInShadow) {
        surfaceColor *= 0.1;
    }

    hitPayload.directColor = surfaceColor;
}
