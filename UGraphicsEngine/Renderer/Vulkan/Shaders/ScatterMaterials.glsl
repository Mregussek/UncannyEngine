
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_GOOGLE_include_directive : enable

#include "DataTypes.glsl"
#include "Random.glsl"


// Polynomial approximation by Christophe Schlick
float Schlick(const float cosine, const float refractionIndex)
{
    float r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 *= r0;
    return r0 + (1 - r0) * pow(1 - cosine, 5);
}


void EvaluateMetallic(inout HitPayload hitPayload, Material triangleMaterial, vec3 rayWorldOrigin,
                      vec3 rayWorldDirection, float t, vec3 worldHitNormal, bool randomSampling,
                      float randomSamplingCoef)
{
    hitPayload.rayDirection = reflect(rayWorldDirection, worldHitNormal);
    hitPayload.isScattered = dot(hitPayload.rayDirection, worldHitNormal) > 0;

    if (randomSampling)
    {
        hitPayload.rayDirection += randomSamplingCoef * RandomInUnitSphere(hitPayload.raySeed);
    }

    hitPayload.directColor = triangleMaterial.specular;
    hitPayload.indirectColor = vec3(0.f);

    hitPayload.rayOrigin = rayWorldOrigin + t * rayWorldDirection;
    hitPayload.t = t;
    hitPayload.previousNormal = worldHitNormal;
}


void EvaluateDielectric(inout HitPayload hitPayload, Material triangleMaterial, vec3 rayWorldOrigin,
                        vec3 rayWorldDirection, float t, vec3 worldHitNormal)
{
    const float dot = dot(rayWorldDirection, worldHitNormal);
    const vec3 outwardNormal = dot > 0 ? -worldHitNormal : worldHitNormal;
    const float niOverNt = dot > 0 ? triangleMaterial.indexOfRefraction : 1 / triangleMaterial.indexOfRefraction;
    const float cosine = dot > 0 ? triangleMaterial.indexOfRefraction * dot : -dot;

    const vec3 refracted = refract(rayWorldDirection, outwardNormal, niOverNt);
    const float reflectProb = refracted != vec3(0) ? Schlick(cosine, triangleMaterial.indexOfRefraction) : 1;

    if (RandomFloat(hitPayload.raySeed) < reflectProb)
    {
        hitPayload.rayDirection = reflect(rayWorldDirection, worldHitNormal);
    }
    else
    {
        hitPayload.rayDirection = refracted;
    }
    hitPayload.isScattered = true;

    hitPayload.directColor = triangleMaterial.specular;
    hitPayload.indirectColor = vec3(0.f);

    hitPayload.rayOrigin = rayWorldOrigin + t * rayWorldDirection;
    hitPayload.t = t;
    hitPayload.previousNormal = worldHitNormal;
}


void EvaluateLambertian(inout HitPayload hitPayload, Material triangleMaterial, vec3 rayWorldOrigin,
                        vec3 rayWorldDirection, float t, vec3 worldHitNormal)
{
    hitPayload.rayDirection = worldHitNormal + RandomInUnitSphere(hitPayload.raySeed);
    hitPayload.isScattered = dot(rayWorldDirection, worldHitNormal) < 0;

    if (hitPayload.rayDepth == 1)
    {
        hitPayload.directColor = triangleMaterial.diffuse;
        hitPayload.indirectColor = vec3(0.f);
    }
    else
    {
        hitPayload.directColor = vec3(0.f);
        hitPayload.indirectColor = (1.f / float(hitPayload.rayDepth)) * triangleMaterial.diffuse;
    }

    hitPayload.rayOrigin = rayWorldOrigin + t * rayWorldDirection;
    hitPayload.t = t;
    hitPayload.previousNormal = worldHitNormal;
}


void EvaluateLightSource(inout HitPayload hitPayload, Material triangleMaterial, vec3 rayWorldOrigin,
                         vec3 rayWorldDirection, float t, vec3 worldHitNormal)
{
    vec3 materialColor = triangleMaterial.emissive + triangleMaterial.diffuse + triangleMaterial.specular;

    if (hitPayload.rayDepth == 1)
    {
        hitPayload.directColor = normalize(materialColor);
        hitPayload.indirectColor = vec3(0.f);
    }
    else
    {
        hitPayload.directColor = vec3(0.f);
        hitPayload.indirectColor = materialColor;// * dot(hitPayload.previousNormal, rayWorldDirection);
    }
    hitPayload.isScattered = false;
}


void EvaluateCompleteMiss(inout HitPayload hitPayload)
{
    hitPayload.directColor = vec3(0.f);
    hitPayload.indirectColor = vec3(0.f);
    hitPayload.isScattered = false;
}
