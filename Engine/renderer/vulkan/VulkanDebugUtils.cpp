
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
  if constexpr (U_VK_DEBUG) {
    UTRACE("Adding debug report callback to Vulkan...");

    auto vkCreateDebugUtilsMessengerEXT =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstanceVk,
                                                                  "vkCreateDebugUtilsMessengerEXT");
    VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugInfo.pfnUserCallback = debugCallbackFunc;
    U_VK_ASSERT( vkCreateDebugUtilsMessengerEXT(mInstanceVk, &debugInfo, nullptr, &mDebugUtilsMsg) );

    UDEBUG("Created Debug Utils Messenger!");
    return UTRUE;
  }

  return UFALSE;
}


b32 FRenderContextVulkan::closeDebugUtilsMessenger() {
  if (mDebugUtilsMsg != VK_NULL_HANDLE) {
    UTRACE("Closing Debug Utils Messenger...");

    auto vkDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstanceVk,
                                                                   "vkDestroyDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT(mInstanceVk, mDebugUtilsMsg, nullptr);

    UDEBUG("Closed Debug Utils Messenger!");
    return UTRUE;
  }
}


VkBool32 debugCallbackFunc(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                           void* pUserData) {
  const auto error = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  const auto warning = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  const auto info = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
  const auto verbose = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

  if (error) {
    printf("%s : %s\n", "ERROR", pCallbackData->pMessage);
  }
  else if (warning) {
    printf("%s : %s\n", "WARNING", pCallbackData->pMessage);
  }
  else if (info) {
    printf("%s : %s\n", "INFO", pCallbackData->pMessage);
  }
  else if (verbose) {
    printf("%s : %s\n", "VERBOSE", pCallbackData->pMessage);
  }

  return VK_FALSE;
}


}
