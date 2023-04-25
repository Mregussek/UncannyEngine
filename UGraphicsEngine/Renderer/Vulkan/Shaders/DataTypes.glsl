
struct Vertex
{
    vec3 position;
    vec3 normal;
    vec3 color;
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
    uint64_t materialIndicesAddress;
};

struct LightData
{
    vec3 position;
};
