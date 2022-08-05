
#include "VulkanSwapchain.h"
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>


namespace uncanny
{


void getRequiredSwapchainExtensions(std::vector<const char*>* pRequiredExtensions) {
  UTRACE("Adding swapchain extension to required ones...");
  pRequiredExtensions->push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}


}
