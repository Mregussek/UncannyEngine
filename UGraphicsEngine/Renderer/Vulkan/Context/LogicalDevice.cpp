
#include "LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


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
  [[nodiscard]] std::pair<b32, u64> IsQueueFamilyPresent(FQueueFamilyIndex queueFamilyIndex) const;

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

  const std::vector<const char*>& extensions = m_Attributes.GetRequiredExtensions();
  const VkPhysicalDeviceFeatures2& deviceFeatures2 = m_Attributes.GetDeviceFeatures2();

  VkDeviceCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &deviceFeatures2,
    .flags = 0,
    .queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size()),
    .pQueueCreateInfos = queueCreateInfos.data(),
    .enabledLayerCount = 0,           // deprecated!
    .ppEnabledLayerNames = nullptr,  // deprecated!
    .enabledExtensionCount = static_cast<u32>(extensions.size()),
    .ppEnabledExtensionNames = extensions.data(),
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
  auto [isPresent, index] = IsQueueFamilyPresent(queueFamilyIndex);
  if (!isPresent)
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

  // If possible to increase queue count, do it, I want to enable at graphics queue family usage for graphics
  // queue_index = 0 and for presentation queue_index = 1
  if (queueIndex > m_DeviceQueueCreateInfoVector.at(index).queueCount - 1)
  {
    m_DeviceQueueCreateInfoVector.at(index).queueCount += 1;
  }
}


std::pair<b32, u64> FQueueCreateInfoCreator::IsQueueFamilyPresent(FQueueFamilyIndex queueFamilyIndex) const
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
