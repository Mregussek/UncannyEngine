
#include "LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan
{


class FQueueCreateInfoCreator
{
public:

  void AddQueueFamilyInfo(FQueueFamilyIndex queueFamilyIndex, FQueueIndex queueIndex);

  [[nodiscard]] const std::vector<VkDeviceQueueCreateInfo>& GetDeviceQueueCreateInfoVector() const
  {
    return m_DeviceQueueCreateInfoVector;
  }

private:

  // @brief Checks if queue family is present in DeviceQueueCreateInfo Vector
  // @return pair of boolean and index, where boolean indicates true if queue family index is present and
  //  u32 index indicates position in vector of such queue create info
  [[nodiscard]] std::pair<b8, u32> IsQueueFamilyPresent(FQueueFamilyIndex queueFamilyIndex) const;

private:

  std::vector<VkDeviceQueueCreateInfo> m_DeviceQueueCreateInfoVector{};
  f32 m_QueuePriority{ 1.f };

};



void FLogicalDevice::Create(const FLogicalDeviceAttributes& attributes, VkPhysicalDevice vkPhysicalDevice)
{
  m_Attributes = attributes;

  FQueueCreateInfoCreator creator{};
  creator.AddQueueFamilyInfo(m_Attributes.GetGraphicsFamilyIndex(), m_Attributes.GetGraphicsQueueIndex());
  creator.AddQueueFamilyInfo(m_Attributes.GetPresentFamilyIndex(), m_Attributes.GetPresentQueueIndex());
  creator.AddQueueFamilyInfo(m_Attributes.GetTransferFamilyIndex(), m_Attributes.GetTransferQueueIndex());
  creator.AddQueueFamilyInfo(m_Attributes.GetComputeFamilyIndex(), m_Attributes.GetComputeQueueIndex());

  const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos = creator.GetDeviceQueueCreateInfoVector();

  const std::vector<const char*>& requiredExtensions = m_Attributes.GetRequiredExtensions();
  const VkPhysicalDeviceFeatures2& deviceFeatures2 = m_Attributes.GetDeviceFeatures2();

  VkDeviceCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &deviceFeatures2,
    .flags = 0,
    .queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size()),
    .pQueueCreateInfos = queueCreateInfos.data(),
    .enabledLayerCount = 0,           // deprecated!
    .ppEnabledLayerNames = nullptr,   // deprecated!
    .enabledExtensionCount = static_cast<u32>(requiredExtensions.size()),
    .ppEnabledExtensionNames = requiredExtensions.data(),
    .pEnabledFeatures = nullptr
  };

  VkResult result = vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &m_Device);
  AssertVkAndThrow(result);

  InitializeQueues();
}


void FLogicalDevice::Destroy()
{
  if (m_Device != VK_NULL_HANDLE)
  {
    vkDestroyDevice(m_Device, nullptr);
  }
}


void FLogicalDevice::WaitIdle() const
{
  vkDeviceWaitIdle(m_Device);
}


void FLogicalDevice::InitializeQueues()
{
  VkQueue graphicsQueueHandle{ VK_NULL_HANDLE };
  vkGetDeviceQueue(m_Device, m_Attributes.GetGraphicsFamilyIndex(), m_Attributes.GetGraphicsQueueIndex(),
                   &graphicsQueueHandle);
  m_GraphicsQueue.Initialize(graphicsQueueHandle, m_Attributes.GetGraphicsFamilyIndex());

  VkQueue presentQueueHandle{ VK_NULL_HANDLE };
  vkGetDeviceQueue(m_Device, m_Attributes.GetPresentFamilyIndex(), m_Attributes.GetPresentQueueIndex(),
                   &presentQueueHandle);
  m_PresentQueue.Initialize(presentQueueHandle, m_Attributes.GetPresentFamilyIndex());

  VkQueue transferQueueHandle{ VK_NULL_HANDLE };
  vkGetDeviceQueue(m_Device, m_Attributes.GetTransferFamilyIndex(), m_Attributes.GetTransferQueueIndex(),
                   &transferQueueHandle);
  m_TransferQueue.Initialize(transferQueueHandle, m_Attributes.GetTransferFamilyIndex());

  VkQueue computeQueueHandle{ VK_NULL_HANDLE };
  vkGetDeviceQueue(m_Device, m_Attributes.GetComputeFamilyIndex(), m_Attributes.GetComputeQueueIndex(),
                   &computeQueueHandle);
  m_ComputeQueue.Initialize(computeQueueHandle, m_Attributes.GetComputeFamilyIndex());
}



void FQueueCreateInfoCreator::AddQueueFamilyInfo(FQueueFamilyIndex queueFamilyIndex, FQueueIndex queueIndex)
{
  if (queueFamilyIndex == VK_QUEUE_FAMILY_IGNORED)
  {
    UERROR("Wrong queue family index given!");
    return;
  }

  auto [isPresent, index] = IsQueueFamilyPresent(queueFamilyIndex);
  if (not isPresent)
  {
    VkDeviceQueueCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = queueFamilyIndex,
      .queueCount = 1,
      .pQueuePriorities = &m_QueuePriority
    };

    m_DeviceQueueCreateInfoVector.push_back(createInfo);
    return;
  }

  // If possible to increase queue count, do it. Graphics and Present queue families may have the same assigned number
  // I want to enable one queue for graphics and one for presentation if both has the same queue family.
  // If we want to enable this, proposed queueIndex will be always higher than 0 (default queue index in queue family)
  // requested queue count in DeviceQueueCreateInfo would be 1, so to check we decrease queue count.
  if (queueIndex > m_DeviceQueueCreateInfoVector[index].queueCount - 1)
  {
    m_DeviceQueueCreateInfoVector[index].queueCount += 1;
  }
}


std::pair<b8, u32> FQueueCreateInfoCreator::IsQueueFamilyPresent(FQueueFamilyIndex queueFamilyIndex) const
{
  for (u32 i = 0; i < m_DeviceQueueCreateInfoVector.size(); i++)
  {
    if (queueFamilyIndex == m_DeviceQueueCreateInfoVector[i].queueFamilyIndex)
    {
      return std::make_pair(UTRUE, i);
    }
  }
  return std::make_pair(UFALSE, UUNUSED);
}


}
