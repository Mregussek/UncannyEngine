
#include "VulkanMesh.h"
#include <utilities/Logger.h>
#include <renderer/Mesh.h>


namespace uncanny
{


b32 getVertexInputBindingDescription(VkVertexInputBindingDescription* pOutDescription) {
  UTRACE("Creating vertex input binding description...");

  pOutDescription->binding = 0;
  pOutDescription->stride = sizeof(FVertex);
  pOutDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  UTRACE("Created vertex input binding description!");
  return UTRUE;
}


b32 getVertexInputAttributeDescription(
    std::vector<VkVertexInputAttributeDescription>* pOutDescriptionVector) {
  UTRACE("Creating vertex input attribute description...");

  pOutDescriptionVector->resize(2);

  pOutDescriptionVector->at(0).location = 0;
  pOutDescriptionVector->at(0).binding = 0;
  pOutDescriptionVector->at(0).format = VK_FORMAT_R32G32B32_SFLOAT;
  pOutDescriptionVector->at(0).offset = offsetof(FVertex, position);

  pOutDescriptionVector->at(0).location = 0;
  pOutDescriptionVector->at(0).binding = 1;
  pOutDescriptionVector->at(0).format = VK_FORMAT_R32G32B32_SFLOAT;
  pOutDescriptionVector->at(0).offset = offsetof(FVertex, color);

  UTRACE("Created vertex input attribute description!");
  return UTRUE;
}


}
