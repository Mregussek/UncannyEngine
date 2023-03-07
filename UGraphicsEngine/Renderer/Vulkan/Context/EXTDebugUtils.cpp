
#include "EXTDebugUtils.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan
{


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFunc(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData);


static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportFunc(VkDebugReportFlagsEXT flags,
                                                      VkDebugReportObjectTypeEXT objectType,
                                                      uint64_t object,
                                                      size_t location,
                                                      int32_t messageCode,
                                                      const char* pLayerPrefix,
                                                      const char* pMessage,
                                                      void* UserData);


void FEXTDebugUtils::Create(VkInstance vkInstance)
{
  auto vkCreateDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
  VkDebugUtilsMessengerCreateInfoEXT utilsInfo{};
  utilsInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  utilsInfo.pNext = nullptr;
  utilsInfo.flags = 0;
  utilsInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  utilsInfo.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  utilsInfo.pfnUserCallback = debugCallbackFunc;
  utilsInfo.pUserData = nullptr;
  VkResult result = vkCreateDebugUtilsMessengerEXT(vkInstance, &utilsInfo, nullptr, &m_DebugUtils);
  AssertVkAndThrow(result);

  VkDebugReportCallbackCreateInfoEXT reportInfo{};
  reportInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  reportInfo.pNext = nullptr;
  reportInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT |
      VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
      VK_DEBUG_REPORT_ERROR_BIT_EXT |
      VK_DEBUG_REPORT_DEBUG_BIT_EXT;
  reportInfo.pfnCallback = debugReportFunc;
  reportInfo.pUserData = nullptr;
}


void FEXTDebugUtils::Destroy(VkInstance vkInstance)
{
  if (m_DebugUtils != VK_NULL_HANDLE)
  {
    auto vkDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT(vkInstance, m_DebugUtils, nullptr);
  }
  if (m_DebugReport != VK_NULL_HANDLE)
  {
    vkDestroyDebugReportCallbackEXT(vkInstance, m_DebugReport, nullptr);
  }
}


VkBool32 debugCallbackFunc(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                           void* pUserData)
{
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
  {
    UERROR("{} : {}\n", "ERROR", pCallbackData->pMessage);
  }
  else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
  {
    UERROR("{} : {}\n", "WARNING", pCallbackData->pMessage);
  }
  else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
  {
    UERROR("{} : {}\n", "INFO", pCallbackData->pMessage);
  }
  else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
  {
    UERROR("{} : {}\n", "VERBOSE", pCallbackData->pMessage);
  }
  else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
  {
    UERROR("{} : {}\n", "PERFORMANCE", pCallbackData->pMessage);
  }

  return VK_FALSE;
}


VkBool32 debugReportFunc(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
                         size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage,
                         void* UserData)
{
  if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
  {
    return VK_FALSE;
  }

  UERROR("DebugReport: {}: {}", pLayerPrefix, pMessage);
  return VK_FALSE;
}


}
