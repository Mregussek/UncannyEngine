
#include "Queues.h"
#include <utilities/Logger.h>


namespace uncanny::vkf
{


b32 FQueuesVulkan::init(const FQueuesInitDependenciesVulkan& deps) {
  UTRACE("Initializing queues vulkan...");

  if (deps.queueFamilyIndexGraphics == deps.queueFamilyIndexTransfer) {
    UTRACE("Queues vulkan have only one queue family available, graphics and transfer will share queues to each other");

    if (deps.availableQueuesGraphics == 1) {
      UTRACE("Queues vulkan have only one available queue for queue family {}!", deps.queueFamilyIndexGraphics);
      m_QueueIndexRendering = 0;
      m_QueueIndexPresentation = 0;
      m_QueueIndexTransfer = 0;
    }
    // If there is at least two available queues for graphics and transfer as they share the same queue family
    else {
      UTRACE("Queues vulkan have at least two available queues for queue family {}!", deps.queueFamilyIndexGraphics);
      m_QueueIndexRendering = 0;
      m_QueueIndexPresentation = 0;
      m_QueueIndexTransfer = 1;
    }
  }
  // If graphics and transfer have other queue family indexes...
  else {
    UTRACE("Queues vulkan have at least two queue families available, graphics and transfer will be separated!");
    if (deps.availableQueuesGraphics == 1) {
      UTRACE("Queues vulkan for graphics have only one queue available, rendering and presentation will be shared!");
      m_QueueIndexRendering = 0;
      m_QueueIndexPresentation = 0;
    }
    else {
      UTRACE("Queues vulkan for graphics have at least two queues available, rendering and present will be separated!");
      m_QueueIndexRendering = 0;
      m_QueueIndexPresentation = 1;
    }
    m_QueueIndexTransfer = 0;
  }

  vkGetDeviceQueue(deps.logicalDevice, deps.queueFamilyIndexGraphics, m_QueueIndexRendering,
                   &m_VkQueueRendering);
  vkGetDeviceQueue(deps.logicalDevice, deps.queueFamilyIndexGraphics, m_QueueIndexPresentation,
                   &m_VkQueuePresentation);
  vkGetDeviceQueue(deps.logicalDevice, deps.queueFamilyIndexTransfer, m_QueueIndexTransfer,
                   &m_VkQueueTransfer);

  UDEBUG("Initialized queues vulkan! ");
  return UTRUE;
}


void FQueuesVulkan::terminate() {
  UTRACE("Terminating queues vulkan...");

  m_QueueIndexRendering = UUNUSED;
  m_QueueIndexPresentation = UUNUSED;
  m_QueueIndexTransfer = UUNUSED;
  m_VkQueueRendering = VK_NULL_HANDLE;
  m_VkQueuePresentation = VK_NULL_HANDLE;
  m_VkQueueTransfer = VK_NULL_HANDLE;

  UDEBUG("Terminated queues vulkan!");
}


}
