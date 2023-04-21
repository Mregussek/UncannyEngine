#version 460
#extension GL_EXT_ray_tracing : enable

layout(location = 0) rayPayloadInEXT vec4 payload;

hitAttributeEXT vec3 attribs;

void main()
{
    const vec3 barycentricCoords = vec3(1.f - attribs.x - attribs.y, attribs.x, attribs.y);
    payload = vec4(barycentricCoords, 0.0);
}
