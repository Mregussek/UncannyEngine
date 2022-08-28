
#ifndef UNCANNYENGINE_VULKANMESH_H
#define UNCANNYENGINE_VULKANMESH_H


#include <utilities/Variables.h>
#include <utilities/Includes.h>
#include <volk.h>


namespace uncanny
{


b32 getVertexInputBindingDescription(VkVertexInputBindingDescription* pOutDescription);


b32 getVertexInputAttributeDescription(
    std::vector<VkVertexInputAttributeDescription>* pOutDescriptionVector);


}


#endif //UNCANNYENGINE_VULKANMESH_H
