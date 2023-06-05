#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_GOOGLE_include_directive : enable

#include "DataTypes.glsl"
#include "Random.glsl"

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

vec3 Mix(vec3 v0, vec3 v1, vec3 v2, vec3 barycentricCoords)
{
    return v0 * barycentricCoords.x + v1 * barycentricCoords.y + v2 * barycentricCoords.z;
}


// Polynomial approximation by Christophe Schlick
float Schlick(const float cosine, const float refractionIndex)
{
    float r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 *= r0;
    return r0 + (1 - r0) * pow(1 - cosine, 5);
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

    // Computing the coordinates of the hit position
    const vec3 hitPos = Mix(vertex0.position, vertex1.position, vertex2.position, barycentricCoords);
    // Transforming the position to world space
    const vec3 worldHitPos = vec3(gl_ObjectToWorldEXT * vec4(hitPos, 1.0));

    // Computing the normal at hit position
    const vec3 hitNormal = Mix(vertex0.normal, vertex1.normal, vertex2.normal, barycentricCoords);
    // Transforming the normal to world space
    const vec3 worldHitNormal = normalize(vec3(hitNormal * gl_WorldToObjectEXT));

    // Lambertian material
    hitPayload.rayOrigin = gl_WorldRayOriginEXT + gl_HitTEXT * gl_WorldRayDirectionEXT;
    hitPayload.t = gl_HitTEXT;
    hitPayload.directColor = triangleMaterial.diffuse + triangleMaterial.specular + triangleMaterial.emissive;

    if (triangleMaterial.illuminationModel == 5) // metallic
    {
        const vec3 reflected = reflect(gl_WorldRayDirectionEXT, worldHitNormal);
        const bool isScattered = dot(reflected, worldHitNormal) > 0;
        const vec3 scatter = reflected + 0.f * RandomInUnitSphere(hitPayload.raySeed);

        hitPayload.directColor += 1.f;
        hitPayload.rayDirection = scatter;
        hitPayload.isScattered = isScattered;
    }
    else if (triangleMaterial.illuminationModel == 7) // dielectic
    {
        const float dot = dot(gl_WorldRayDirectionEXT, worldHitNormal);
        const vec3 outwardNormal = dot > 0 ? -worldHitNormal : worldHitNormal;
        const float niOverNt = dot > 0 ? triangleMaterial.indexOfRefraction : 1 / triangleMaterial.indexOfRefraction;
        const float cosine = dot > 0 ? triangleMaterial.indexOfRefraction * dot : -dot;

        const vec3 refracted = refract(gl_WorldRayDirectionEXT, outwardNormal, niOverNt);
        const float reflectProb = refracted != vec3(0) ? Schlick(cosine, triangleMaterial.indexOfRefraction) : 1;

        if (RandomFloat(hitPayload.raySeed) < reflectProb)
        {
            hitPayload.rayDirection = reflect(gl_WorldRayDirectionEXT, worldHitNormal);
        }
        else
        {
            hitPayload.rayDirection = refracted;
        }

        hitPayload.directColor += 1.f;
        hitPayload.isScattered = true;
    }
    else if (triangleMaterial.illuminationModel == 4) // emissive material
    {
        hitPayload.directColor = normalize(hitPayload.directColor);
        hitPayload.isScattered = false;
    }
    else // lambertian
    {
        const bool IsScattered = dot(gl_WorldRayDirectionEXT, worldHitNormal) < 0;
        const vec3 ScatteredDirection = worldHitNormal + RandomInUnitSphere(hitPayload.raySeed);

        hitPayload.rayDirection = ScatteredDirection;
        hitPayload.isScattered = IsScattered;
    }
}
