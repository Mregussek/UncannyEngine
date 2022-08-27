
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>
#include <filesystem/FileManager.h>


namespace uncanny
{


static b32 createTriangleGraphicsPipeline();


b32 FRenderContextVulkan::createGraphicsPipelinesGeneral() {
  UTRACE("Creating graphics pipelines general...");

  b32 createdTrianglePipeline{ createTriangleGraphicsPipeline() };
  if (not createdTrianglePipeline) {
    UERROR("Could not create triangle graphics pipeline!");
    return UFALSE;
  }

  UDEBUG("Created graphics pipelines general!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeGraphicsPipelinesGeneral() {
  UTRACE("Closing graphics pipelines general...");

  UDEBUG("Closed graphics pipelines general!");
  return UTRUE;
}


b32 createTriangleGraphicsPipeline() {
  UTRACE("Creating triangle graphics pipeline...");

  const char* vertex{ "shaders/triangle.vert.spv" };
  std::vector<char> vertexCode{};
  b32 loadedVertex{ FFileManager::loadFile(vertex, &vertexCode) };
  if (not loadedVertex) {
    UERROR("Could not load vertex shader!");
    return UFALSE;
  }

  const char* fragment{ "shaders/triangle.frag.spv" };
  std::vector<char> fragCode{};
  b32 loadedFragment{ FFileManager::loadFile(fragment, &fragCode) };
  if (not loadedFragment) {
    UERROR("Could not load fragment shader!");
    return UFALSE;
  }

  UDEBUG("Created triangle graphics pipeline!");
  return UTRUE;
}


}
