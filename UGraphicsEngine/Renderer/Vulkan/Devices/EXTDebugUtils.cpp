
#include "EXTDebugUtils.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan {


VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFunc(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                 VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                 void* pUserData);


void FEXTDebugUtils::Create(VkInstance vkInstance) {
  auto vkCreateDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance,
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
  VkResult result = vkCreateDebugUtilsMessengerEXT(vkInstance, &debugInfo, nullptr, &m_DebugUtils);
  AssertVkAndThrow(result);
}


void FEXTDebugUtils::Destroy(VkInstance vkInstance) {
  if (m_DebugUtils == VK_NULL_HANDLE) {
    return;
  }
  auto vkDestroyDebugUtilsMessengerEXT =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
  vkDestroyDebugUtilsMessengerEXT(vkInstance, m_DebugUtils, nullptr);
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
