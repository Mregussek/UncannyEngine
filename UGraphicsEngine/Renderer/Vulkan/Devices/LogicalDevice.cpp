
#include "LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan {


class FLogicalDeviceQueueCreateInfoCreator {
public:

  void AddQueueFamilyToDeviceQueueCreateInfo(FQueueFamilyIndex queueFamilyIndex, FQueueIndex queueIndex);

  [[nodiscard]] const std::vector<VkDeviceQueueCreateInfo>& GetDeviceQueueCreateInfoVector() const { return m_DeviceQueueCreateInfoVector; }

private:

  // @brief Checks if queue family is present in DeviceQueueCreateInfo Vector
  // @return pair of boolean and index, where boolean indicates true if queue family index is present and
  //  u32 index indicates position in vector of such queue create info
  [[nodiscard]] std::pair<b32, u64> IsQueueFamilyPresent(FQueueFamilyIndex queueFamilyIndex) const;

  std::vector<VkDeviceQueueCreateInfo> m_DeviceQueueCreateInfoVector{};
  f32 m_QueuePriority{ 1.f };

};



void FLogicalDevice::Create(const FLogicalDeviceAttributes& attributes, VkPhysicalDevice vkPhysicalDevice) {
  m_Attributes = attributes;

  FLogicalDeviceQueueCreateInfoCreator creator{};
  creator.AddQueueFamilyToDeviceQueueCreateInfo(m_Attributes.GetGraphicsQueueFamilyIndex(),
                                                m_Attributes.GetGraphicsQueueIndex());
  creator.AddQueueFamilyToDeviceQueueCreateInfo(m_Attributes.GetPresentQueueFamilyIndex(),
                                                m_Attributes.GetPresentQueueIndex());
  creator.AddQueueFamilyToDeviceQueueCreateInfo(m_Attributes.GetTransferQueueFamilyIndex(),
                                                m_Attributes.GetTransferQueueIndex());
  creator.AddQueueFamilyToDeviceQueueCreateInfo(m_Attributes.GetComputeQueueFamilyIndex(),
                                                m_Attributes.GetComputeQueueIndex());

  const auto& requiredExtensions = m_Attributes.GetRequiredExtensions();
  const auto& deviceQueueCreateInfo = creator.GetDeviceQueueCreateInfoVector();
  const auto& deviceFeatures = m_Attributes.GetDeviceFeatures();

  VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.queueCreateInfoCount = deviceQueueCreateInfo.size();
  createInfo.pQueueCreateInfos = deviceQueueCreateInfo.data();
  createInfo.enabledLayerCount = 0;             // deprecated!
  createInfo.ppEnabledLayerNames = nullptr;    // deprecated!
  createInfo.enabledExtensionCount = requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();
  createInfo.pEnabledFeatures = &deviceFeatures;

  VkResult result = vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &m_Device);
  AssertVkAndThrow(result);

  InitializeQueues();
}


void FLogicalDevice::Destroy() {
  if (m_Device != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(m_Device);
    vkDestroyDevice(m_Device, nullptr);
  }
}


void FLogicalDevice::Wait() const {
  vkDeviceWaitIdle(m_Device);
}


void FLogicalDevice::InitializeQueues() {
  VkQueue graphicsQueueHandle{ VK_NULL_HANDLE };
  vkGetDeviceQueue(m_Device, m_Attributes.GetGraphicsQueueFamilyIndex(), m_Attributes.GetGraphicsQueueIndex(),
                   &graphicsQueueHandle);
  m_GraphicsQueue.Initialize(graphicsQueueHandle, m_Attributes.GetGraphicsQueueFamilyIndex());

  VkQueue presentQueueHandle{ VK_NULL_HANDLE };
  vkGetDeviceQueue(m_Device, m_Attributes.GetPresentQueueFamilyIndex(), m_Attributes.GetPresentQueueIndex(),
                   &presentQueueHandle);
  m_PresentQueue.Initialize(presentQueueHandle, m_Attributes.GetPresentQueueFamilyIndex());

  VkQueue transferQueueHandle{ VK_NULL_HANDLE };
  vkGetDeviceQueue(m_Device, m_Attributes.GetTransferQueueFamilyIndex(), m_Attributes.GetTransferQueueIndex(),
                   &transferQueueHandle);
  m_TransferQueue.Initialize(transferQueueHandle, m_Attributes.GetTransferQueueFamilyIndex());

  VkQueue computeQueueHandle{ VK_NULL_HANDLE };
  vkGetDeviceQueue(m_Device, m_Attributes.GetComputeQueueFamilyIndex(), m_Attributes.GetComputeQueueIndex(),
                   &computeQueueHandle);
  m_ComputeQueue.Initialize(computeQueueHandle, m_Attributes.GetComputeQueueFamilyIndex());
}



void FLogicalDeviceQueueCreateInfoCreator::AddQueueFamilyToDeviceQueueCreateInfo(FQueueFamilyIndex queueFamilyIndex,
                                                                                 FQueueIndex queueIndex) {
  auto [isPresent, index] = IsQueueFamilyPresent(queueFamilyIndex);
  if (!isPresent) {
    VkDeviceQueueCreateInfo &createInfo = m_DeviceQueueCreateInfoVector.emplace_back();
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.queueFamilyIndex = queueFamilyIndex;
    createInfo.queueCount = 1;
    createInfo.pQueuePriorities = &m_QueuePriority;
    return;
  }

  // If possible to increase queue count, do it
  // I want to enable at graphics queue family usage for graphics queue_index = 0 and for presentation queue_index = 1
  if (queueIndex > m_DeviceQueueCreateInfoVector.at(index).queueCount - 1) {
    m_DeviceQueueCreateInfoVector.at(index).queueCount += 1;
  }
}


std::pair<b32, u64> FLogicalDeviceQueueCreateInfoCreator::IsQueueFamilyPresent(FQueueFamilyIndex queueFamilyIndex) const {
  auto it = std::ranges::find_if(m_DeviceQueueCreateInfoVector, [queueFamilyIndex](const VkDeviceQueueCreateInfo& createInfo) -> b32 {
    return queueFamilyIndex == createInfo.queueFamilyIndex;
  });
  if (it != m_DeviceQueueCreateInfoVector.end()) {
    return std::make_pair(UTRUE, std::distance(m_DeviceQueueCreateInfoVector.begin(), it));
  }

  return std::make_pair(UFALSE, UUNUSED);
}


}
