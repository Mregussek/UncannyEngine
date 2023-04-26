
#define M_PI 3.1415926535897932384626433832795

struct Vertex
{
    vec3 position;
    vec3 normal;
    vec3 color;
};

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float specularShininess;
    float indexOfRefraction;
    int illuminationModel;
};

struct BottomStructureUniformData
{
    uint64_t vertexAddress;
    uint64_t indexAddress;
    uint64_t materialAddress;
    uint64_t materialIndicesAddress;
};

struct LightData
{
    vec3 position;
};

struct HitPayload
{
  vec3 rayOrigin;
  vec3 rayDirection;
  vec3 directColor;
  bool IsReflective;
};
