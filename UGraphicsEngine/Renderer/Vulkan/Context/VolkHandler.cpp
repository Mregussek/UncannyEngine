
#define VOLK_IMPLEMENTATION
#include <volk.h>

#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "VolkHandler.h"


namespace uncanny::vulkan
{


void FVolkHandler::Initialize()
{
  VkResult result = volkInitialize();
  AssertVkAndThrow(result, "Could not initialize volk library");
}


void FVolkHandler::LoadInstance(VkInstance vkInstance)
{
  volkLoadInstance(vkInstance);
}


}
