
#define M_PI 3.1415926535897932384626433832795

struct Vertex
{
    vec3 position;
    vec3 normal;
};

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    vec3 transparent;
    float shininess;
    float opacity;
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

struct PerFrameUniformData
{
    mat4 projInverse;
    mat4 viewInverse;
    uint randomSeed;
    uint notMovingCameraFrameCount;
    uint maxRayBounces;
    uint maxSamplesPerPixel;
};

struct HitPayload
{
    vec3 rayOrigin;
    vec3 rayDirection;
    vec3 rayColor;
    uint raySeed;
    float t;
    bool isScattered;
};
