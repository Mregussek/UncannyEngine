
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include "VulkanDebugUtils.h"
#include <utilities/Logger.h>


namespace uncanny
{


VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFunc(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);


void getRequiredDebugInstanceLayers(std::vector<const char*>* pRequiredLayers) {
  if constexpr (U_VK_DEBUG) {
    UTRACE("Adding khronos validation layer to VkInstance...");
    pRequiredLayers->push_back("VK_LAYER_KHRONOS_validation");
  }
}


void getRequiredDebugInstanceExtensions(std::vector<const char*>* pRequiredExtensions) {
  if constexpr (U_VK_DEBUG) {
    UTRACE("Adding debug utils extension to VkInstance...");
    pRequiredExtensions->push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
}


b32 FRenderContextVulkan::createDebugUtilsMessenger() {
  UTRACE("Adding debug report callback to Vulkan...");

  auto vkCreateDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mVkInstance,
                                                                "vkCreateDebugUtilsMessengerEXT");
  VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
  debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debugInfo.pNext = nullptr;
  debugInfo.flags = 0;
  debugInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  debugInfo.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  debugInfo.pfnUserCallback = debugCallbackFunc;
  debugInfo.pUserData = nullptr;

  U_VK_ASSERT( vkCreateDebugUtilsMessengerEXT(mVkInstance, &debugInfo, nullptr, &mVkDebugUtilsMsg) );

  UDEBUG("Created Debug Utils Messenger!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeDebugUtilsMessenger() {
  UTRACE("Closing Debug Utils Messenger...");

  if (mVkDebugUtilsMsg != VK_NULL_HANDLE) {
    auto vkDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            mVkInstance, "vkDestroyDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT(mVkInstance, mVkDebugUtilsMsg, nullptr);
  }
  else {
    UWARN("Debug Utils Messenger is not closed, as it wasn't created!");
  }

  UDEBUG("Closed Debug Utils Messenger!");
  return UTRUE;
}


VkBool32 debugCallbackFunc(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                           void* pUserData) {
  const auto error = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  const auto warning = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  const auto info = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
  const auto verbose = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
  const auto performance = messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

  if (error) {
    UERROR("{} : {}\n", "ERROR", pCallbackData->pMessage);
  }
  else if (warning) {
    UERROR("{} : {}\n", "WARNING", pCallbackData->pMessage);
  }
  else if (info) {
    UERROR("{} : {}\n", "INFO", pCallbackData->pMessage);
  }
  else if (verbose) {
    UERROR("{} : {}\n", "VERBOSE", pCallbackData->pMessage);
  }
  else if (performance) {
    UERROR("{} : {}\n", "PERFORMANCE", pCallbackData->pMessage);
  }

  return VK_FALSE;
}


}
