
#include "DebugUtils.h"
#include "Utilities.h"
#include <utilities/Logger.h>


namespace uncanny::vkf
{


void emplaceRequiredDebugInstanceLayers(std::vector<const char*>* pRequiredLayers) {
  UTRACE("Em-placing khronos validation layer to instance vulkan...");
  pRequiredLayers->push_back("VK_LAYER_KHRONOS_validation");
}


void emplaceRequiredDebugInstanceExtensions(std::vector<const char*>* pRequiredExtensions) {
  UTRACE("Em-placing debug utils extension to instance vulkan...");
  pRequiredExtensions->push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}


VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFunc(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                 VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                 void* pUserData);


b32 FDebugUtilsVulkan::init(const FDebugUtilsInitDependenciesVulkan& deps) {
  UTRACE("Initializing debug utils vulkan...");

  auto vkCreateDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(deps.instance,
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

  AssertResultVulkan( vkCreateDebugUtilsMessengerEXT(deps.instance, &debugInfo, nullptr, &m_VkDebugUtilsMsg) );

  UDEBUG("Initialized debug utils vulkan!");
  return UTRUE;
}


void FDebugUtilsVulkan::terminate(VkInstance instance) {
  UTRACE("Terminating debug utils vulkan...");

  if (m_VkDebugUtilsMsg != VK_NULL_HANDLE) {
    auto vkDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT(instance, m_VkDebugUtilsMsg, nullptr);
  }
  else {
    UWARN("Debug Utils Messenger is not closed, as it wasn't created!");
  }

  UDEBUG("Terminated debug utils vulkan!");
}


VkBool32 VKAPI_CALL debugCallbackFunc(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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
